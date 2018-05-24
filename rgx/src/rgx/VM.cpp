#include "rgx/VM.h"

using namespace cppr;

namespace rgx
{
	inline static bool
	_exec_ins(VM_State& vm, Thread_State& thread, const Tape& program)
	{
		switch(thread.code.front().ins)
		{
			case ISA::RUNE:
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

			case ISA::MTCH:
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

			case ISA::SPLT2:
			case ISA::SPLT:
			{
				auto current_ins = thread.code.front();

				//get the first branch
				thread.code.pop_front();
				i32 A = thread.code.front().offset;

				//get the second branch
				thread.code.pop_front();
				i32 B = thread.code.front().offset;

				thread.code.pop_front();

				if(current_ins.ins == ISA::SPLT)
				{
					thread.code = program.range(thread.code.begin() + A, program.end());
					vm.threads.emplace(
						Thread_State
						{
							program.range(thread.code.begin() + B, program.end()),
							thread.data
						});
				}
				else
				{
					thread.code = program.range(thread.code.begin() + B, program.end());
					vm.threads.emplace(
						Thread_State
						{
							program.range(thread.code.begin() + A, program.end()),
							thread.data
						});
				}

				return true;
			}

			case ISA::JUMP:
			{
				//get the offset
				thread.code.pop_front();
				i32 offset = thread.code.front().offset;
				thread.code.pop_front();

				thread.code = program.range(thread.code.begin() + offset, program.end());
				return true;
			}

			case ISA::NSET:
			case ISA::SET:
			{
				if(thread.data.empty())
					return false;

				usize ins_index = thread.code.begin() - program.begin();
				auto current_ins = thread.code.front();

				//get the count
				thread.code.pop_front();
				u32 count = thread.code.front().count;

				//go to the first data
				thread.code.pop_front();

				if(current_ins.ins == ISA::SET)
				{
					bool result = false;
					Rune data = thread.data.front();
					for(u32 i = 0; i < count; ++i, thread.code.pop_front())
					{
						if(!result &&
						   thread.code.front().data == data)
						{
							thread.data.pop_front();
							result = true;
						}
					}
					return result;
				}
				//NSET
				else
				{
					bool result = true;
					Rune data = thread.data.front();
					for(u32 i = 0; i < count; ++i, thread.code.pop_front())
						if(result && thread.code.front().data == data)
							result = false;
					if(result)
						thread.data.pop_front();
					return result;
				}
			}

			case ISA::HALT:
			case ISA::NONE:
			default:
				return false;
		}
	}

	bool
	_run(VM_State& vm, const String_Range& input, const Tape& program,
					 Match_Result& res, MATCH_MODE mode)
	{
		vm.clear();
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

				//check that we currently have instruction byte to execute
				if(thread.code.front().type != Bytecode::INST)
					break;

				//check if this is the halt bytecode to stop the vm with success
				if(thread.code.front().ins == ISA::HALT)
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
				if(!_exec_ins(vm, thread, program))
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
}