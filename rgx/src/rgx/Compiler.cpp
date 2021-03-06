#include "rgx/Compiler.h"
#include "rgx/Optimizer.h"
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

	RGX_ERROR
	compile(const String_Range& expression,
			Tape& program, bool optimize_flag,
			const Memory_Context& context)
	{
		Bytecode_Compiler compiler(context);

		RGX_ERROR compiler_result = _compile(expression, &compiler);
		if(compiler_result != RGX_ERROR::OK)
			return compiler_result;

		//add the halt command at the end of the program
		compiler.operands.top().insert_back(Bytecode::make_ins(ISA::HALT));

		if(optimize_flag)
		{
			optimize(compiler.operands.top(), program, context);
		}
		else
		{
			program = compiler.operands.top();
			compiler.operands.pop();
		}

		return RGX_ERROR::OK;
	}
}
