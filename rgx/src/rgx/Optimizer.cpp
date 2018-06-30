#include "rgx/Optimizer.h"

using namespace cppr;

namespace rgx
{
	Tape_Structure::Tape_Structure(const Memory_Context& context)
		:blocks(context),
		 branches(context)
	{}

	void
	Tape_Structure::clear()
	{
		blocks.clear();
		branches.clear();
	}

	void
	Tape_Structure::reset()
	{
		blocks.reset();
		branches.reset();
	}


	inline static Linear_Block*
	_link_jump_with_block(Tape_Structure& st, usize address)
	{
		for(auto it = st.blocks.begin();
			it != st.blocks.end();
			++it)
		{
			Linear_Block& block = *it;

			if(address > block.address &&
			   address < (block.address + block.count))
			{
				usize old_count = block.count;
				block.count = address - block.address;

				Linear_Block new_block{};
				new_block.address = address;
				new_block.count = old_count - block.count;
				st.blocks.insert_after(it, new_block);
			}
			else if(address == block.address)
			{
				return &block;
			}
		}

		return nullptr;
	}

	inline static void
	_detect_branches_and_split_blocks(Tape_Structure& st, const Tape& program)
	{
		for(usize i = 0; i < program.count(); ++i)
		{
			const Bytecode& code = program[i];
			if(code.type != Bytecode::INST)
				continue;

			if(code.ins == ISA::SPLT)
			{
				Branch branch{};
				branch.address = i;
				branch.type = Branch::SPLT;
				
				//+3 for the size of the splt instruction in bytecode
				usize first_block_address = program[i + 1].offset + i + 3;
				usize second_block_address = program[i + 2].offset + i + 3;

				branch.splt.block_1 = _link_jump_with_block(st, first_block_address);
				branch.splt.block_2 = _link_jump_with_block(st, second_block_address);

				st.branches.insert_back(branch);
			}
			else if(code.ins == ISA::JUMP)
			{
				Branch branch;
				branch.address = i;
				branch.type = Branch::JUMP;

				//+2 for the size of the jumps instruction in bytecode
				usize jump_address = program[i + 1].offset + i + 2;
				branch.jmp.block = _link_jump_with_block(st, jump_address);

				st.branches.insert_back(branch);
			}
		}
	}

	void
	build_structure(const Tape& program, Tape_Structure& structure)
	{
		Linear_Block global_block{};
		global_block.address = 0;
		global_block.count = program.count();
		structure.blocks.insert_back(global_block);

		_detect_branches_and_split_blocks(structure, program);
	}


	//optimization code
	inline static void
	_optimize_block(const Tape& input, Tape& output, Linear_Block& block, Slice<Branch>& branches)
	{
		usize old_count = output.count();
		usize block_extent = block.address + block.count;
		for(usize i = block.address; i < block_extent; ++i)
		{
			//optimize the rune instructions into mtch instruction
			if (input[i].type == Bytecode::INST &&
				input[i].ins  == ISA::RUNE &&
				input[i+1].type == Bytecode::DATA)
			{
				usize i_it;
				usize runes_count = 0;
				for(i_it = i; i_it < block_extent; ++i_it)
				{
					if (input[i_it].type == Bytecode::INST &&
						input[i_it].ins == ISA::RUNE)
					{
						if(input[i_it + 1].type != Bytecode::DATA)
							break;
						++runes_count;
						++i_it;
					}
					else
					{
						break;
					}
				}

				//if it's just one rune then no need to compress it
				if(runes_count == 1)
				{
					output.insert_back(input[i]);
				}
				else
				{
					output.insert_back(Bytecode::make_ins(ISA::MTCH));
					output.insert_back(Bytecode::make_count(runes_count));

					i_it = i + 1;
					for(usize j = 0; j < runes_count; ++j, i_it += 2)
						output.insert_back(input[i_it]);

					i = i_it - 2;
				}
			}
			//map branches to its new addresses
			else if(!branches.empty() && branches.front().address == i)
			{
				branches.front().opt.new_address = output.count();
				branches.pop_front();
				output.insert_back(input[i]);
			}
			else
			{
				output.insert_back(input[i]);
			}
		}

		block.opt.new_count = output.count();
	}

	inline static void
	_write_optimized_blocks(Tape_Structure& st, const Tape& input, Tape& output)
	{
		auto branches = st.branches.all();
		for(auto& block: st.blocks)
		{
			block.opt.new_address = output.count();
			_optimize_block(input, output, block, branches);
		}
	}

	inline static void
	_branches_fixup(Tape_Structure& st, Tape& output)
	{
		for(const auto& branch: st.branches)
		{
			switch(branch.type)
			{
				case Branch::SPLT:
				{
					//fix the branches
					//+3 for the size of the splt instruction
					output[branch.opt.new_address + 1].offset =
						branch.splt.block_1->opt.new_address - (branch.opt.new_address + 3);

					output[branch.opt.new_address + 2].offset =
						branch.splt.block_2->opt.new_address - (branch.opt.new_address + 3);
				}
				break;

				case Branch::JUMP:
				{
					//+2 for the size of the jump instruction
					output[branch.opt.new_address + 1].offset =
						branch.jmp.block->opt.new_address - (branch.opt.new_address + 2);
				}
				break;

				default:
				break;
			}
		}
	}
	
	void
	optimize(const Tape& input, Tape& output, const Memory_Context& context)
	{
		Tape_Structure st(context);
		build_structure(input, st);

		_write_optimized_blocks(st, input, output);
		_branches_fixup(st, output);
	}
}