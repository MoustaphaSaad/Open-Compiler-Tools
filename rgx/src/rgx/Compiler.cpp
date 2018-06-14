#include "rgx/Compiler.h"
#include <cpprelude/Stack_Array.h>
#include <cpprelude/Hash_Map.h>
#include <cpprelude/Double_List.h>
#include <cpprelude/Allocators.h>
using namespace cppr;

namespace rgx
{
	/*
	 * every operation has a bytecode template comment at it's begining
	 * here's the guideline to read the bytecode template
	 * [A] -> everything inside square brackets is a bytecode
	 * {A} -> everything inside a curly braces is a sequence of bytecode
	 */

	enum class OPERATORS
	{
		//this operators are ordered by presedence from lower to higher
		OR = 0,
		CONCAT = 1,
		PLUS = 2,
		PLUS_NON_GREEDY = 3,
		STAR = 4,
		STAR_NON_GREEDY = 5,
		OPTIONAL = 6,
		OPTIONAL_NON_GREEDY = 7,
		//negative operators to not interfere with the presendence
		OPEN_PAREN = -1,
		CLOSE_PAREN = -2
	};

	struct Bytecode_Compiler
	{
		//compiler memory context
		Memory_Context mem_context;
		//operands whcih is an actual programs
		Stack_Array<Tape> operands;
		//operators which is just a stack of operators type
		Stack_Array<OPERATORS> operators;


		//flags
		bool ignore;
		bool recommend_concat;

		Bytecode_Compiler(const Memory_Context& context)
			:mem_context(context),
			 operands(context),
			 operators(context),
			 ignore(false),
			 recommend_concat(false)
		{}
	};

	inline static bool
	_is_operator(byte b)
	{
		return (b == '\\' ||
				b == '|'  ||
				b == '*'  ||
				b == '('  ||
				b == ')'  ||
				b == '+'  ||
				b == '?'  ||
				b == '['  ||
				b == ']'  ||
				b == '.'  );
	}

	inline static bool
	_is_operator(Rune c)
	{
		return (c == '\\' ||
				c == '|'  ||
				c == '*'  ||
				c == '('  ||
				c == ')'  ||
				c == '+'  ||
				c == '?'  ||
				c == '['  ||
				c == ']'  ||
				c == '.'  );
	}

	inline static RGX_ERROR
	_op_concat(Bytecode_Compiler* compiler)
	{
		/**
		 * bytecode template
		 * {A}
		 * {B}
		 */

		usize count = compiler->operands.count();
		if(count < 2)
			return RGX_ERROR::OPERANDS_COUNT_MISMATCH;

		Tape& B = compiler->operands.top();
		Tape& A = compiler->operands._array[count - 2];

		usize old_A_count = A.count();
		A.expand_back_raw(B.count());
		copy<Bytecode>(A.range(old_A_count, A.count()), B.all());

		compiler->operands.pop();
		return RGX_ERROR::OK;
	}

	inline static RGX_ERROR
	_op_or(Bytecode_Compiler* compiler)
	{
		/**
		 * bytecode template:
		 * [SPLT] [0], [A.count() + 2]
		 * {A}
		 * [JMP] [B.count()]
		 * {B}
		 */

		if(compiler->operands.count() < 2)
			return RGX_ERROR::OPERANDS_COUNT_MISMATCH;

		Tape B(std::move(compiler->operands.top()));
		compiler->operands.pop();

		Tape A(std::move(compiler->operands.top()));
		compiler->operands.pop();

		Tape C(compiler->mem_context);
		C.reserve(A.count() + B.count() + 5);

		C.insert_back(Bytecode::make_ins(ISA::SPLT));
		C.insert_back(Bytecode::make_offset(0));
		//+2 for the jmp over B
		C.insert_back(Bytecode::make_offset(A.count() + 2));

		C.expand_back_raw(A.count());
		copy<Bytecode>(C.range(3, C.count()), A.all());

		//JMP over B
		C.insert_back(Bytecode::make_ins(ISA::JUMP));
		C.insert_back(Bytecode::make_offset(B.count()));

		usize c_count = C.count();
		C.expand_back_raw(B.count());

		copy<Bytecode>(C.range(c_count, C.count()), B.all());

		compiler->operands.push(std::move(C));
		return RGX_ERROR::OK;
	}

	inline static RGX_ERROR
	_op_star(Bytecode_Compiler* compiler, bool is_greedy)
	{
		/**
		 * bytecode template:
		 * [SPLT] [0] [A.count() + 2]
		 * {A}
		 * [JUMP] [-(A.count() + 5)]
		 */

		if(compiler->operands.count() < 1)
			return RGX_ERROR::OPERANDS_COUNT_MISMATCH;

		Tape A(std::move(compiler->operands.top()));
		compiler->operands.pop();

		Tape C(compiler->mem_context);
		C.reserve(A.count() + 5);

		C.insert_back(Bytecode::make_ins(ISA::SPLT));
		if(is_greedy)
		{
			C.insert_back(Bytecode::make_offset(0));
			//+2 for the jump to begin size
			C.insert_back(Bytecode::make_offset(A.count() + 2));
		}
		else
		{
			//+2 for the jump to begin size
			C.insert_back(Bytecode::make_offset(A.count() + 2));
			C.insert_back(Bytecode::make_offset(0));
		}

		//add A
		C.expand_back_raw(A.count());
		copy<Bytecode>(C.range(3, C.count()), A.all());

		C.insert_back(Bytecode::make_ins(ISA::JUMP));
		//jump back to the begining the SPLT instruction thus we add +5
		C.insert_back(Bytecode::make_offset(-1 * (A.count() + 5)));

		compiler->operands.push(std::move(C));
		return RGX_ERROR::OK;
	}

	inline static RGX_ERROR
	_op_plus(Bytecode_Compiler* compiler, bool is_greedy)
	{
		/**
		 * bytecode template: (AA*)
		 * {A}
		 * [SPLT] [-(A.count() + 3)] [0]
		 */

		if(compiler->operands.count() < 1)
			return RGX_ERROR::OPERANDS_COUNT_MISMATCH;

		Tape& A = compiler->operands.top();

		//+3 for the split ins
		usize begin_offset = A.count() + 3;
		A.insert_back(Bytecode::make_ins(ISA::SPLT));

		if(is_greedy)
		{
			A.insert_back(Bytecode::make_offset(-begin_offset));
			A.insert_back(Bytecode::make_offset(0));
		}
		else
		{
			A.insert_back(Bytecode::make_offset(0));
			A.insert_back(Bytecode::make_offset(-begin_offset));
		}

		return RGX_ERROR::OK;
	}

	inline static RGX_ERROR
	_op_optional(Bytecode_Compiler* compiler, bool is_greedy)
	{
		/**
		 * bytecode template:
		 * [SPLT] [0] [A.count()]
		 * {A}
		 */

		if(compiler->operands.count() < 1)
			return RGX_ERROR::OPERANDS_COUNT_MISMATCH;

		Tape A(std::move(compiler->operands.top()));
		compiler->operands.pop();

		Tape C(compiler->mem_context);
		C.reserve(A.count() + 3);

		C.insert_back(Bytecode::make_ins(ISA::SPLT));
		if(is_greedy)
		{
			C.insert_back(Bytecode::make_offset(0));
			C.insert_back(Bytecode::make_offset(A.count()));
		}
		else
		{
			C.insert_back(Bytecode::make_offset(A.count()));
			C.insert_back(Bytecode::make_offset(0));
		}

		C.expand_back_raw(A.count());
		copy<Bytecode>(C.range(3, C.count()), A.all());

		compiler->operands.push(std::move(C));
		return RGX_ERROR::OK;
	}

	inline static RGX_ERROR
	_eval_op(Bytecode_Compiler* compiler)
	{
		if(compiler->operators.empty())
			return RGX_ERROR::EVAL_NO_OPERATOR;

		auto op = compiler->operators.top();
		compiler->operators.pop();
		switch (op)
		{
			case rgx::OPERATORS::CONCAT:
				return _op_concat(compiler);

			case rgx::OPERATORS::OR:
				return _op_or(compiler);

			case rgx::OPERATORS::STAR:
				return _op_star(compiler, true);

			case rgx::OPERATORS::STAR_NON_GREEDY:
				return _op_star(compiler, false);

			case rgx::OPERATORS::PLUS:
				return _op_plus(compiler, true);

			case rgx::OPERATORS::PLUS_NON_GREEDY:
				return _op_plus(compiler, false);

			case rgx::OPERATORS::OPTIONAL:
				return _op_optional(compiler, true);

			case rgx::OPERATORS::OPTIONAL_NON_GREEDY:
				return _op_optional(compiler, false);
		}
		return RGX_ERROR::GENERIC_ERROR;
	}

	inline static RGX_ERROR
	_push_op(Bytecode_Compiler* compiler, OPERATORS op)
	{
		while(!compiler->operators.empty() &&
			  compiler->operators.top() >= op)
		{
			auto res = _eval_op(compiler);
			if(res != RGX_ERROR::OK)
				return res;
		}

		compiler->operators.push(op);
		return RGX_ERROR::OK;
	}

	inline static RGX_ERROR
	_handle_state(Bytecode_Compiler* compiler)
	{
		if(compiler->recommend_concat)
		{
			//push a concat operation
			RGX_ERROR res = _push_op(compiler, OPERATORS::CONCAT);
			if(res != RGX_ERROR::OK)
				return res;

			//reset the concat flag
			compiler->recommend_concat = false;
		}
		return RGX_ERROR::OK;
	}

	inline static RGX_ERROR
	_compile(const String_Range& expression, Bytecode_Compiler* compiler)
	{
		auto rgx_it = expression.begin();
		auto rgx_end = expression.end();
		while(rgx_it != rgx_end)
		{
			Rune c = *rgx_it;

			//if found \ and we are not in ignore state then set the ignore state
			if(c == '\\' && !compiler->ignore)
			{
				compiler->ignore = true;
				++rgx_it;
				continue;
			}
			//if found | and we are not in ignore state then add the or op
			else if(c == '|' && !compiler->ignore)
			{
				RGX_ERROR res = _push_op(compiler, OPERATORS::OR);
				if(res != RGX_ERROR::OK)
					return res;

				compiler->ignore = false;
				compiler->recommend_concat = false;
				++rgx_it;
				continue;
			}
			//if found * and we are not in ignore state then add the star op
			else if(c == '*' && !compiler->ignore)
			{
				//check to see if it's greedy operator
				OPERATORS op = OPERATORS::STAR;
				auto maybe_nongreedy = rgx_it;
				++maybe_nongreedy;
				if(maybe_nongreedy != rgx_end)
				{
					if(*maybe_nongreedy == '?')
					{
						op = OPERATORS::STAR_NON_GREEDY;
						rgx_it = maybe_nongreedy;
					}
				}

				RGX_ERROR res = _push_op(compiler, op);
				if(res != RGX_ERROR::OK)
					return res;

				compiler->ignore = false;
				compiler->recommend_concat = true;
				++rgx_it;
				continue;
			}
			//if found + and we are not in ignore state then add the plus op
			else if(c == '+' && !compiler->ignore)
			{
				//check to see if it's greedy operator
				OPERATORS op = OPERATORS::PLUS;
				auto maybe_nongreedy = rgx_it;
				++maybe_nongreedy;
				if(maybe_nongreedy != rgx_end)
				{
					if(*maybe_nongreedy == '?')
					{
						op = OPERATORS::PLUS_NON_GREEDY;
						rgx_it = maybe_nongreedy;
					}
				}

				RGX_ERROR res = _push_op(compiler, op);
				if(res != RGX_ERROR::OK)
					return res;

				compiler->ignore = false;
				compiler->recommend_concat = true;
				++rgx_it;
				continue;
			}
			//if found ? and we are not in ignore state then add the optional op
			else if(c == '?' && !compiler->ignore)
			{
				//check to see if it's greedy operator
				OPERATORS op = OPERATORS::OPTIONAL;
				auto maybe_nongreedy = rgx_it;
				++maybe_nongreedy;
				if(maybe_nongreedy != rgx_end)
				{
					if(*maybe_nongreedy == '?')
					{
						op = OPERATORS::OPTIONAL_NON_GREEDY;
						rgx_it = maybe_nongreedy;
					}
				}

				RGX_ERROR res = _push_op(compiler, op);
				if(res != RGX_ERROR::OK)
					return res;

				compiler->ignore = false;
				compiler->recommend_concat = true;
				++rgx_it;
				continue;
			}
			//if found . and we are not in ignore state then add the any rune
			else if(c == '.' && !compiler->ignore)
			{
				//handle the state
				RGX_ERROR res = _handle_state(compiler);
				if(res != RGX_ERROR::OK)
					return res;

				Tape block(compiler->mem_context);
				block.insert_back(Bytecode::make_ins(ISA::RUNE));
				block.insert_back(Bytecode::make_ins(ISA::ANY));
				compiler->operands.push(std::move(block));

				compiler->ignore = false;
				compiler->recommend_concat = true;
				++rgx_it;
				continue;
			}
			//if found ( and we are not in ignore state then ad the OPEN_PAREN operator
			else if(c == '(' && !compiler->ignore)
			{
				//handle the state
				RGX_ERROR res = _handle_state(compiler);
				if(res != RGX_ERROR::OK)
					return res;

				compiler->operators.push(OPERATORS::OPEN_PAREN);

				compiler->recommend_concat = false;
				compiler->ignore = false;
				++rgx_it;
				continue;
			}
			//if found ) and we are not in ignore state then ad the CLOSE_PARENT operator
			else if(c == ')' && !compiler->ignore)
			{
				//evaluate all the operators in inside the ()
				while(!compiler->operators.empty() && compiler->operators.top() != OPERATORS::OPEN_PAREN)
				{
					RGX_ERROR res = _eval_op(compiler);
					if(res != RGX_ERROR::OK)
						return res;
				}

				//pop the OPEN_PAREN
				compiler->operators.pop();

				compiler->recommend_concat = true;
				compiler->ignore = false;
				++rgx_it;
				continue;
			}
			//if found [ and we are not in ignore state then perform the operator
			else if(c == '[' && !compiler->ignore)
			{
				RGX_ERROR res = _handle_state(compiler);
				if(res != RGX_ERROR::OK)
					return res;

				//create a new block
				Tape block(compiler->mem_context);
				block.insert_back(Bytecode::make_ins(ISA::SET));
				//set a placeholder offset
				block.insert_back(Bytecode::make_offset(0));
				//set a placeholder count
				block.insert_back(Bytecode::make_count(0));

				usize set_element_count = 0;

				++rgx_it;
				c = *rgx_it;

				//check if it's nor set
				if(c == '^')
				{
					block[0] = Bytecode::make_ins(ISA::NSET);
					++rgx_it;
					c = *rgx_it;
				}

				bool local_ignore = false;
				while(rgx_it != rgx_end)
				{
					//if we detected a range
					if(c == '\\' && !local_ignore)
					{
						local_ignore = true;
					}
					else if(c == ']' && !local_ignore)
					{
						break;
					}
					else if(c == '-' && !local_ignore)
					{
						//get the startin grune
						auto& back = block.back();

						//didn't find a startin rune error
						if(back.type != Bytecode::DATA)
							return RGX_ERROR::SET_RANGE_START_RUNE_NOT_FOUND;

						//get the start rune
						Rune start = back.data;

						//get the end rune
						++rgx_it;
						c = *rgx_it;
						Rune end = c;

						//invalid range
						if(end < start)
						{
							return RGX_ERROR::SET_RANGE_INVALID;
						}
						//if already tthe end is the start then we break out of the loop
						else if(end == start)
						{
							//advance and break the loop
							++rgx_it;
							c = *rgx_it;
							break;
						}

						//continue adding the runes
						block.back() = Bytecode::make_ins(ISA::RNG);
						block.insert_back(Bytecode::make_data(start));
						block.insert_back(Bytecode::make_data(end));

						local_ignore = false;
					}
					//not a range
					else
					{
						block.insert_back(Bytecode::make_data(c));
						++set_element_count;
						local_ignore = false;
					}

					++rgx_it;
					c = *rgx_it;
				}

				//error
				if(c != ']')
					return RGX_ERROR::SET_CLOSE_BRACKET_NOT_FOUND;

				//adjust the offset
				//-3 for the first [SET, OFFSET, COUNT]
				block[1] = Bytecode::make_offset(block.count() - 3);
				//adjust the count
				block[2] = Bytecode::make_count(set_element_count);
				//push the block
				compiler->operands.emplace(std::move(block));

				compiler->recommend_concat = true;
				compiler->ignore = false;
				++rgx_it;
				continue;
			}
			//if no regex specific rune found then this is a simple rune to match
			else
			{
				RGX_ERROR res = _handle_state(compiler);
				if(res != RGX_ERROR::OK)
					return res;

				//assume that this is a continuous stream of simple runes and emit a compact mtch instruction
				compiler->operands.emplace(compiler->mem_context);
				Tape& block = compiler->operands.top();
				block.insert_back(Bytecode::make_ins(ISA::RUNE));
				block.insert_back(Bytecode::make_data(c));

				++rgx_it;
				c = *rgx_it;
				auto next_rgx_it = rgx_it;
				++next_rgx_it;
				auto next_c = *next_rgx_it;

				while(!_is_operator(c) &&
					  rgx_it != rgx_end &&
					  next_rgx_it != rgx_end &&
					  !_is_operator(next_c))
				{
					block.insert_back(Bytecode::make_ins(ISA::RUNE));
					block.insert_back(Bytecode::make_data(c));

					rgx_it = next_rgx_it;
					c = next_c;

					++next_rgx_it;
					next_c = *next_rgx_it;
				}

				compiler->recommend_concat = true;
				compiler->ignore = false;
				continue;
			}
		}

		while(!compiler->operators.empty())
		{
			auto res = _eval_op(compiler);
			if(res != RGX_ERROR::OK)
				return res;
		}
		return RGX_ERROR::OK;
	}

	//optimize declarations
	void
	_optimize_tape(const Tape& input, Tape& output, const Memory_Context& context);

	RGX_ERROR
	compile(const String_Range& expression,
			Tape& program, bool optimize,
			const Memory_Context& context)
	{
		Bytecode_Compiler compiler(context);

		RGX_ERROR compiler_result = _compile(expression, &compiler);
		if(compiler_result != RGX_ERROR::OK)
			return compiler_result;

		//add the halt command at the end of the program
		compiler.operands.top().insert_back(Bytecode::make_ins(ISA::HALT));

		if(optimize)
		{
			_optimize_tape(compiler.operands.top(), program, context);
		}
		else
		{
			program = compiler.operands.top();
			compiler.operands.pop();
		}

		return RGX_ERROR::OK;
	}

	//optimization code
	struct Seq_Block
	{
		usize address = 0;
		usize count = 0;

		usize new_address = 0;
		usize new_count = 0;
	};

	struct Branch
	{
		constexpr static const usize MAX_BLOCKS = 2;

		usize address = 0;
		usize new_address = 0;
		ISA ins = ISA::NONE;
		Seq_Block* blocks[MAX_BLOCKS] = {nullptr};
	};

	struct Optimizer
	{
		Double_List<Seq_Block> blocks;
		Dynamic_Array<Branch> branches;

		Optimizer(const Memory_Context& context)
			:blocks(context),
			 branches(context)
		{}
	};

	Seq_Block*
	_link_jump_with_block(Optimizer& optimizer, usize jump_address)
	{
		for(auto it = optimizer.blocks.begin();
			it != optimizer.blocks.end();
			++it)
		{
			Seq_Block& block = *it;

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

				Seq_Block new_block;
				new_block.address = jump_address;
				new_block.count = old_count - block.count;
				optimizer.blocks.insert_after(it, new_block);
			}
		}

		return nullptr;
	}

	void
	_detect_branches_and_split_blocks(Optimizer& optimizer, const Tape& input)
	{
		for(usize i = 0; i < input.count(); ++i)
		{
			const Bytecode& code = input[i];
			if(code.type != Bytecode::INST)
				continue;

			if (code.ins == ISA::SPLT)
			{
				Branch splt;
				splt.address = i;
				splt.ins = code.ins;

				//+3 for the size of the splt instruction in bytecode
				usize first_branch_address = input[i + 1].offset + i + 3;
				usize second_branch_address = input[i + 2].offset + i + 3;

				splt.blocks[0] = _link_jump_with_block(optimizer, first_branch_address);
				splt.blocks[1] = _link_jump_with_block(optimizer, second_branch_address);

				optimizer.branches.insert_back(splt);
			}
			else if(code.ins == ISA::JUMP)
			{
				Branch jump;

				jump.address = i;
				jump.ins = code.ins;
				//+2 for the size of the jump instruction in bytecode
				usize jump_address = input[i + 1].offset + i + 2;
				jump.blocks[0] = _link_jump_with_block(optimizer, jump_address);

				optimizer.branches.insert_back(jump);
			}
		}
	}

	inline static void
	_optimize_block(const Tape& input, Tape& output, Seq_Block& block, Slice<Branch>& branches)
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
				branches.front().new_address = output.count();
				branches.pop_front();
				output.insert_back(input[i]);
			}
			else
			{
				output.insert_back(input[i]);
			}
		}

		block.new_count = output.count() - old_count;
	}

	inline static void
	_write_optimized_blocks(Optimizer& optimizer, const Tape& input, Tape& output)
	{
		auto branches = optimizer.branches.all();
		for(auto& block: optimizer.blocks)
		{
			block.new_address = output.count();
			_optimize_block(input, output, block, branches);
		}
	}

	inline static void
	_branches_fixup(Optimizer& optimizer, Tape& output)
	{
		for(const auto& branch: optimizer.branches)
		{
			switch(branch.ins)
			{
				case ISA::SPLT:
				{
					//fix the branches
					//+3 for the size of the splt instruction
					output[branch.new_address + 1].offset = branch.blocks[0]->new_address - (branch.new_address + 3);
					output[branch.new_address + 2].offset = branch.blocks[1]->new_address - (branch.new_address + 3);
				}
				break;

				case ISA::JUMP:
				{
					//+2 for the size of the jump instruction
					output[branch.new_address + 1].offset = branch.blocks[0]->new_address - (branch.new_address + 2);
				}
				break;

				default:
				break;
			}
		}
	}

	void
	_optimize_tape(const Tape& input, Tape& output, const Memory_Context& context)
	{
		/**
		 * The optimization techniques is simple we need to compress the:
		 * Rune
		 * a
		 * Rune
		 * b
		 * Rune
		 * c
		 * 
		 * to:
		 * MTCH
		 * 3
		 * a
		 * b
		 * c
		 * 
		 * This involves detecting branching and split the code to blocks then optimize the blocks
		 * individually
		 */

		output.reserve(input.count());
		Optimizer optimizer(context);

		//init the entire program as a single big block
		Seq_Block global_block;
		global_block.address = 0;
		global_block.count = input.count();
		optimizer.blocks.insert_back(global_block);

		//extract the branchs and split the above single big block
		_detect_branches_and_split_blocks(optimizer, input);

		//write the optimized blocks
		_write_optimized_blocks(optimizer, input, output);

		//fix the branches addresses
		_branches_fixup(optimizer, output);
	}
}
