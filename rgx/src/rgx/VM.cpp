#include "rgx/VM.h"
#include <cpprelude/Dynamic_Array.h>
#include <cpprelude/Double_List.h>
#include <cpprelude/Panic.h>

using namespace cppr;

namespace rgx
{
	inline static bool
	_ins_error(VM_State& vm, Thread_State& thread, const Tape& program)
	{
		return false;
	}

	inline static bool
	_ins_rune(VM_State& vm, Thread_State& thread, const Tape& program)
	{
		//check if data is empty
		if(thread.data.empty())
			return false;

		thread.code.pop_front();

		//handle the any instruction
		if(thread.code.front().type == Bytecode::INST &&
			thread.code.front().ins == ISA::ANY)
		{
			thread.code.pop_front();
			thread.data.pop_front();
			return true;
		}
		//compare the expected data with the actual data
		else if(thread.code.front().data == thread.data.front())
		{
			thread.code.pop_front();
			thread.data.pop_front();
			return true;
		}

		return false;
	}

	inline static bool
	_ins_mtch(VM_State& vm, Thread_State& thread, const Tape& program)
	{
		//check if data pointer haven't reached the end of data
		if(thread.data.empty())
			return false;

		thread.code.pop_front();
		u32 count = thread.code.front().count;

		thread.code.pop_front();
		for(u32 i = 0; i < count; ++i)
		{
			if(thread.data.empty() ||
				thread.code.front().data != thread.data.front())
				return false;

			thread.code.pop_front();
			thread.data.pop_front();
		}
		return true;
	}

	inline static bool
	_ins_splt(VM_State& vm, Thread_State& thread, const Tape& program)
	{
		//get the first branch
		thread.code.pop_front();
		i32 A = thread.code.front().offset;

		//get the second branch
		thread.code.pop_front();
		i32 B = thread.code.front().offset;

		thread.code.pop_front();

		auto& top_thread = vm.threads.top();
		Thread_State new_thread
		{
			program.range(thread.code.begin() + B, program.end()),
			thread.data
		};
		thread.code = program.range(thread.code.begin() + A, program.end());

		if (top_thread.code != new_thread.code ||
			top_thread.data != new_thread.data)
			vm.threads.push(new_thread);

		return true;
	}

	inline static bool
	_ins_jump(VM_State& vm, Thread_State& thread, const Tape& program)
	{
		//get the offset
		thread.code.pop_front();
		i32 offset = thread.code.front().offset;
		thread.code.pop_front();

		thread.code = program.range(thread.code.begin() + offset, program.end());
		return true;
	}

	inline static bool
	_ins_set(VM_State& vm, Thread_State& thread, const Tape& program)
	{
		if(thread.data.empty())
			return false;

		auto current_ins = thread.code.front();

		//get the count
		thread.code.pop_front();
		i32 skip_offset = thread.code.front().offset;

		thread.code.pop_front();
		u32 count = thread.code.front().count;

		//go to the first data
		thread.code.pop_front();

		auto skipped_thread_code = thread.code.range(thread.code.begin() + skip_offset, thread.code.end());

		bool found = false;
		Rune next_rune = thread.data.front();
		for(usize i = 0; i < count; ++i, thread.code.pop_front())
		{
			auto& set_code = thread.code.front();
			if(set_code.type == Bytecode::INST &&
				set_code.ins == ISA::RNG)
			{
				thread.code.pop_front();
				Rune start = thread.code.front().data;
				thread.code.pop_front();
				Rune end = thread.code.front().data;

				if(next_rune >= start && next_rune <= end)
				{
					found = true;
					break;
				}
			}
			else if(set_code.type == Bytecode::DATA)
			{
				if(next_rune == set_code.data)
				{
					found = true;
					break;
				}
			}
		}

		thread.code = skipped_thread_code;
		//set
		if(current_ins.ins == ISA::SET)
		{
			if(found == true)
				thread.data.pop_front();
			return found;
		}
		//nset
		else
		{
			if(found == false)
				thread.data.pop_front();
			return !found;
		}
	}

	inline static bool
	_ins_push(VM_State& vm, Thread_State& thread, const Tape& program)
	{
		thread.code.pop_front();
		vm.stack.emplace_back(thread.code.front().value);
		thread.code.pop_front();
		return true;
	}

	using Ins_Proc = bool(*)(VM_State&, Thread_State&, const Tape&);

	Ins_Proc INS_PROCS[] = {
		_ins_error,			//NONE
		_ins_rune,			//RUNE
		_ins_mtch,			//MTCH
		_ins_splt,			//SPLT
		_ins_jump,			//JUMP
		_ins_set,			//SET
		_ins_set,			//NSET
		_ins_error,			//RNG
		_ins_error,			//ANY
		_ins_error,			//HALT
		_ins_push			//PUSH
	};

	bool
	_run(VM_State& vm, const String_Range& input, const Tape& program,
		 Match_Result& res, MATCH_MODE mode)
	{
		vm.clear();
		vm.threads._array.reserve(program.count() / 4);
		vm.threads.emplace(Thread_State{program.all(), input.all()});

		while(!vm.threads.empty())
		{
			Thread_State thread(std::move(vm.threads.top()));
			vm.threads.pop();

			//execute the thread
			while(true)
			{
				//check that we haven't reached the end of the code
				if(thread.code.empty())
					break;

				auto& code = thread.code.front();

				//check that we currently have instruction byte to execute
				if(code.type != Bytecode::INST)
					break;

				//check if this is the halt bytecode to stop the vm with success
				if(code.ins == ISA::HALT)
				{
					switch (mode)
					{
						case MATCH_MODE::FIRST_MATCH:
							res.accepted = true;
							res.str = input.range(input.begin(), thread.data.begin());
							return true;

						case MATCH_MODE::SHORTEST_MATCH:
						case MATCH_MODE::LONGEST_MATCH:
							vm.matches.emplace_back(input.range(input.begin(), thread.data.begin()));
							break;
					}
				}
			
				//execute the instruction
				if(!INS_PROCS[static_cast<i32>(code.ins)](vm, thread, program))
					break;
			}
		}

		if(vm.matches.empty())
			return false;

		res.accepted = true;
		res.str = vm.matches.front();
		if(mode == MATCH_MODE::SHORTEST_MATCH)
		{
			for(const auto& match: vm.matches.range(1, vm.matches.count()))
				if(res.str.count() > match.count())
					res.str = match;
		}
		else
		{
			for(const auto& match: vm.matches.range(1, vm.matches.count()))
				if(res.str.count() < match.count())
					res.str = match;
		}

		return true;
	}

	bool
	run(VM_State& vm,
		const cppr::String_Range& input,
		const Tape& program,
		Match_Result& res,
		MATCH_MODE mode)
	{
		return _run(vm, input, program, res, mode);
	}

	bool
	run(VM_State& vm,
		const cppr::String_Range& input,
		const Tape& program,
		MATCH_MODE mode)
	{
		Match_Result res;
		return _run(vm, input, program, res, mode);
	}

	//CPP code gen
	/*
	struct Gen_Block
	{
		usize address = 0, count = 0, label = -1;
	};

	struct Gen_Branch
	{
		constexpr static const usize MAX_BLOCKS = 2;

		usize address = 0;
		ISA ins = ISA::NONE;
		Gen_Block* blocks[MAX_BLOCKS] = { nullptr };
	};

	struct CPP_Generator
	{
		Double_List<Gen_Block> blocks;
		Dynamic_Array<Gen_Branch> branches;
		usize label_counter = 0;
	};

	inline static Gen_Block*
	_link_jump_with_block(CPP_Generator& gen, usize jump_address)
	{
		for(auto it = gen.blocks.begin();
			it != gen.blocks.end();
			++it)
		{
			Gen_Block& block = *it;

			if(jump_address == block.address)
			{
				return &block;
			}
			//if jump_address is within the current block
			else if(jump_address > block.address &&
					jump_address < (block.address + block.count))
			{
				usize old_count = block.count;
				block.count = jump_address - block.address;

				Gen_Block new_block;
				new_block.address = jump_address;
				new_block.count = old_count - block.count;
				new_block.label = gen.label_counter++;
				gen.blocks.insert_after(it, new_block);
			}
		}

		return nullptr;
	}

	inline static void
	_detect_branches_and_split_blocks(CPP_Generator& gen, const Tape& input)
	{
		for(usize i = 0; i < input.count(); ++i)
		{
			const Bytecode& code = input[i];
			if(code.type != Bytecode::INST)
				continue;

			if (code.ins == ISA::SPLT)
			{
				Gen_Branch splt;
				splt.address = i;
				splt.ins = code.ins;

				//+3 for the size of the splt instruction in bytecode
				usize first_Gen_Branch_address = input[i + 1].offset + i + 3;
				usize second_Gen_Branch_address = input[i + 2].offset + i + 3;

				splt.blocks[0] = _link_jump_with_block(gen, first_Gen_Branch_address);
				splt.blocks[1] = _link_jump_with_block(gen, second_Gen_Branch_address);

				gen.branches.insert_back(splt);
			}
			else if(code.ins == ISA::JUMP)
			{
				Gen_Branch jump;

				jump.address = i;
				jump.ins = code.ins;
				//+2 for the size of the jump instruction in bytecode
				usize jump_address = input[i + 1].offset + i + 2;
				jump.blocks[0] = _link_jump_with_block(gen, jump_address);

				gen.branches.insert_back(jump);
			}
		}
	}

	inline static void
	_emit_block(IO_Trait* io, const Tape& program, Gen_Block& block, Slice<Gen_Branch>& branches)
	{
		vprintf(io, "\t\tcase {}:{{\n", block.label);
		usize block_extent = block.address + block.count;
		bool fallthrough = true;
		for(usize i = block.address; i < block_extent; ++i)
		{
			const auto& code = program[i];
			if(code.type != Bytecode::INST)
				continue;

			switch(code.ins)
			{
				case ISA::RUNE:
					++i;
					if (program[i].type == Bytecode::INST &&
						program[i].ins == ISA::ANY)
					{
						vprints(io, "\t\t\tif(it == input.end()) { current_state = static_cast<usize>(-1); break; }\n");
						vprints(io, "\t\t\t++it;\n\n");
					}
					else
					{
						vprints(io, "\t\t\tif(it == input.end()) { current_state = static_cast<usize>(-1); break; }\n");
						vprints(io, "\t\t\tc = *it;\n");
						vprintf(io, "\t\t\tif(c != '{}') {{ current_state = static_cast<usize>(-1); break; }\n", program[i].data);
						vprints(io, "\t\t\t++it;\n\n");
					}
					fallthrough = true;
				break;

				case ISA::MTCH:
				{
					++i;
					u32 count = program[i].count;
					++i;
					for(usize j = 0; j < count; ++j)
					{
						vprints(io, "\t\t\tif(it == input.end()) { current_state = static_cast<usize>(-1); break; }\n");
						vprints(io, "\t\t\tc = *it;\n");
						vprintf(io, "\t\t\tif(c != '{}') {{ current_state = static_cast<usize>(-1); break; }\n", program[i + j].data);
						vprints(io, "\t\t\t++it;\n\n");
					}
					fallthrough = true;
					i += count - 1;
				}
				break;

				case ISA::SPLT:
				{
					vprints(io, "\t\t\tif(state_pointer == STATES_MAX) return false;\n");
					vprintf(io, "\t\t\tcurrent_state = {};\n", branches.front().blocks[0]->label);
					vprintf(io, "\t\t\tstates[state_pointer] = {};\n", branches.front().blocks[1]->label);
					vprints(io, "\t\t\tstates_data[state_pointer] = it;\n");
					vprints(io, "\t\t\t++state_pointer;\n");
					fallthrough = false;
					branches.pop_front();
					i += 2;
				}
				break;

				case ISA::JUMP:
				{
					vprintf(io, "\t\t\tcurrent_state = {};\n", branches.front().blocks[0]->label);
					fallthrough = false;
					branches.pop_front();
					++i;
				}
				break;

				case ISA::SET:
				{
					++i;
					u32 count = program[i].count;
					++i;
					vprints(io, "\t\t\tif(it == input.end()) { current_state = static_cast<usize>(-1); break; }\n");
					vprints(io, "\t\t\tc = *it;\n");

					vprints(io, "\t\t\tif(\n");
					for(usize j = 0; j < count; ++j)
					{
						vprintf(io, "\t\t\t\tc == '{}'", program[i + j].data);
						if(j != count - 1)
							vprints(io, " ||\n");
						else
							vprints(io, ")\n");
					}
					i += count - 1;
					vprints(io, "\t\t\t{\n");
						vprints(io, "\t\t\t\t++it;\n");
					vprints(io, "\t\t\t}\n");
					vprints(io, "\t\t\telse\n");
					vprints(io, "\t\t\t{\n");
						vprintf(io, "\t\t\t\tcurrent_state = static_cast<usize>(-1); break;\n", block.label);
					vprints(io, "\t\t\t}\n");
					fallthrough = true;
				}
				break;

				case ISA::NSET:
				{
					++i;
					u32 count = program[i].count;
					++i;
					vprints(io, "\t\t\tif(it == input.end()) { current_state = static_cast<usize>(-1); break; }\n");
					vprints(io, "\t\t\tc = *it;\n");

					vprints(io, "\t\t\tif(\n");
					for(usize j = 0; j < count; ++j)
					{
						vprintf(io, "\t\t\t\tc != '{}'", program[i + j].data);
						if(j != count - 1)
							vprints(io, " &&\n");
						else
							vprints(io, ")\n");
					}
					i += count - 1;
					vprints(io, "\t\t\t{\n");
						vprints(io, "\t\t\t\t++it;\n");
					vprints(io, "\t\t\t}\n");
					vprints(io, "\t\t\telse\n");
					vprints(io, "\t\t\t{\n");
						vprintf(io, "\t\t\t\tcurrent_state = static_cast<usize>(-1); break;\n", block.label);
					vprints(io, "\t\t\t}\n");
					fallthrough = true;
				}
				break;

				case ISA::HALT:
					vprints(io, "\t\t\thalt = true;\n");
					fallthrough = false;
				break;

				default:
				break;
			}
		}
		if(fallthrough)
			vprintf(io, "\t\t\tcurrent_state = {};\n", block.label + 1);
		vprints(io, "\t\t\t} break;\n");
	}

	inline static void
	_emit_program(IO_Trait* io, CPP_Generator& gen, const Tape& program)
	{
		auto branches = gen.branches.all();
		for(auto& block: gen.blocks)
		{
			_emit_block(io, program, block, branches);
		}
	}

	bool
	cpp_gen(IO_Trait* io, const Tape& program)
	{
		CPP_Generator gen;

		//init the entire program as a single big block
		Gen_Block global_block;
		global_block.address = 0;
		global_block.count = program.count();
		global_block.label = gen.label_counter++;
		gen.blocks.insert_back(global_block);

		_detect_branches_and_split_blocks(gen, program);

		//relabel the blocks
		usize i = 0;
		for(auto& block: gen.blocks)
			block.label = i++;

		vprints(io, "bool\n");
		vprints(io, "func(const cppr::String_Range& input, rgx::Match_Result& res)\n");
		vprints(io, "{\n");
			vprints(io, "\tauto it = input.begin();\n");
			vprints(io, "\tRune c{};\n");
			vprints(io, "\tconstexpr usize STATES_MAX = 32;\n");
			vprints(io, "\tusize states[STATES_MAX] = {0};\n");
			vprints(io, "\tString_Iterator states_data[STATES_MAX];\n");
			vprints(io, "\tusize state_pointer = 0;\n");
			vprints(io, "\tusize current_state = 0;\n");
			vprints(io, "\tbool halt = false;\n");
			vprints(io, "\twhile(!halt && current_state != static_cast<usize>(-1))\n");
			vprints(io, "\t{\n");
				vprints(io, "\t\tswitch(current_state)\n");
				vprints(io, "\t\t{\n");
					_emit_program(io, gen, program);
					vprints(io, "\t\tdefault:{\n");
						vprints(io, "\t\t\tcurrent_state = static_cast<usize>(-1);\n");
					vprints(io, "\t\t} break;\n");
				vprints(io, "\t\t}\n");
				vprints(io, "\t\tif(current_state == static_cast<usize>(-1) && state_pointer != 0){\n");
					vprints(io, "\t\t\t--state_pointer;\n");
					vprints(io, "\t\t\tcurrent_state = states[state_pointer];\n");
					vprints(io, "\t\t\tit = states_data[state_pointer];\n");
				vprints(io, "\t\t}\n");
			vprints(io, "\t}\n");
			vprints(io, "\tif(halt) { res.accepted = true; res.str = input.range(input.begin(), it); return true; }\n");
			vprints(io, "\treturn false;\n");
		vprints(io, "}\n");
		return true;
	}
	*/
}