#include "oct/Proto_Lexer.h"
#include <rgx/Compiler.h>
#include <rgx/Optimizer.h>

using namespace cppr;
using namespace rgx;

namespace oct
{
	Proto_Lexer2::Proto_Lexer2(const Memory_Context& context)
		:mem_context(context),
		 line_number(0),
		 column_number(0),
		 actions(mem_context),
		 vm(mem_context),
		 program(mem_context)
	{}

	inline static void
	_op_or(const Tape& A, const Tape& B, Tape& C)
	{
		C.reserve_raw(A.count() + B.count() + 5);

		C.insert_back(Bytecode::make_ins(ISA::SPLT));
		C.insert_back(Bytecode::make_offset(0));
		//+2 for the jmp over B
		C.insert_back(Bytecode::make_offset(A.count() + 2));

		C.expand_back_raw(A.count());
		copy<Bytecode>(C.range(3, C.count()), A.all());

		//jmp over B
		C.insert_back(Bytecode::make_ins(ISA::JUMP));
		C.insert_back(Bytecode::make_offset(B.count()));

		usize c_count = C.count();
		C.expand_back_raw(B.count());
		copy<Bytecode>(C.range(c_count, C.count()), B.all());
	}

	bool
	Proto_Lexer2::push_token(const String_Range& regexp, const Token_Action& action)
	{
		Tape exp(mem_context);
		if(compile(regexp, exp, false, mem_context) != RGX_ERROR::OK)
			return false;

		if(program.empty())
		{
			exp.back() = Bytecode::make_ins(ISA::PUSH);
			exp.insert_back(Bytecode::make_value(actions.count()));
			program = std::move(exp);
		}
		else
		{
			exp.back() = Bytecode::make_ins(ISA::PUSH);
			exp.insert_back(Bytecode::make_value(actions.count()));

			Tape new_program(mem_context);
			_op_or(program, exp, new_program);
			program = std::move(new_program);
		}

		actions.insert_back(action);
		return true;
	}

	void
	Proto_Lexer2::build()
	{
		program.insert_back(Bytecode::make_ins(ISA::HALT));

		Tape opt_program(mem_context);
		optimize(program, opt_program, mem_context);
		program = std::move(opt_program);
	}

	bool
	Proto_Lexer2::token(cppr::String_Range& str, Token& out_token)
	{
		String_Iterator it;
		Match_Result res;
		while(true)
		{
			if(!run(vm, str, program, res, MATCH_MODE::FIRST_MATCH))
				return false;

			i32 ix = vm.stack.back();
			Token_Action& ideal = actions[ix];

			if(ideal.ignore)
			{
				str = str.range(res.str.end(), str.end());
				continue;
			}

			Token tkn;
			tkn.type = ideal.type;
			tkn.line = line_number;
			tkn.column = column_number;
			tkn.str = res.str;
			it = res.str.end();

			auto tmp_str = str;
			if (ideal.action != nullptr &&
				!ideal.action(res.str, tmp_str, tkn))
			{
				it = tmp_str.end();
			}

			for(auto c_it = str.begin();
				c_it != it;
				++c_it)
			{
				auto c = *c_it;
				if(c == '\n')
				{
					++line_number;
					column_number = 0;
				}
				else
				{
					++column_number;
				}
			}

			out_token = tkn;
			str = str.range(it, str.end());
			return true;
		}
		return false;
	}

	Proto_Lexer::Proto_Lexer(const Memory_Context& context)
		:mem_context(context),
		line_number(0),
		column_number(0),
		actions(mem_context),
		rgx_tapes(mem_context),
		rgx_vm(mem_context),
		rgx_vm2(mem_context),
		single_tape(mem_context)
	{}

	bool
	Proto_Lexer::push_token(const String_Range& regexp, const Token_Action& action)
	{
		rgx_tapes.emplace_back(mem_context);
		if(compile(regexp, rgx_tapes.back(), true, mem_context) != RGX_ERROR::OK)
		{
			rgx_tapes.remove_back();
			return false;
		}

		actions.insert_back(action);
		return true;
	}

	bool
	Proto_Lexer::push_token_2(const String_Range& regexp, const Token_Action& action)
	{
		Tape exp_tape(mem_context);
		if(compile(regexp, exp_tape, true, mem_context) != RGX_ERROR::OK)
			return false;

		exp_tape.remove_back();
		exp_tape.emplace_back(Bytecode::make_ins(ISA::PUSH));
		exp_tape.emplace_back(Bytecode::make_value(actions.count()));

		if(single_tape.empty())
		{
			single_tape = std::move(exp_tape);
			single_tape.insert_back(Bytecode::make_ins(ISA::HALT));
		}
		else
		{
			single_tape.remove_back();

			Tape other(mem_context);
			other.insert_back(Bytecode::make_ins(ISA::SPLT));
			other.insert_back(Bytecode::make_offset(0));
			other.insert_back(Bytecode::make_offset(single_tape.count() + 2));
			other.insert_back(single_tape.all());
			other.insert_back(Bytecode::make_ins(ISA::JUMP));
			other.insert_back(Bytecode::make_offset(exp_tape.count()));
			other.insert_back(exp_tape.all());
			other.insert_back(Bytecode::make_ins(ISA::HALT));

			single_tape = std::move(other);
		}

		actions.insert_back(action);
		return true;
	}

	bool
	Proto_Lexer::token(String_Range& stream, Token& token)
	{
		auto action_it = actions.begin();
		auto tape_it = rgx_tapes.begin();

		Token longest_token;
		String_Iterator longest_it;

		//loop through the available tapes
		while(tape_it != rgx_tapes.end())
		{
			Match_Result tmp_res;
			//execute the tape
			if(run(rgx_vm, stream, *tape_it, tmp_res, MATCH_MODE::FIRST_MATCH))
			{
				//if this token should be ignored then do exactly that
				if(action_it->ignore)
				{
					stream = stream.range(tmp_res.str.end(), stream.end());
					tape_it = rgx_tapes.begin();
					action_it = actions.begin();
					continue;
				}

				//check if the max result is not initialized
				//or if the result is larger than the current max_res then handle it
				if(longest_it.ptr == nullptr ||
				   tmp_res.str.bytes.ptr > longest_it.ptr)
				{
					//setup the longest token
					longest_token.type = action_it->type;
					longest_token.line = line_number;
					longest_token.column = column_number;
					longest_token.str = tmp_res.str;
					longest_it = tmp_res.str.end();
					//check for the action
					if(action_it->action != nullptr)
					{
						auto tmp_stream = stream;
						if(action_it->action(tmp_res.str, tmp_stream, longest_token))
							longest_it = tmp_stream.begin();
					}
				}
			}
			++tape_it;
			++action_it;
		}

		if(longest_it.ptr == nullptr)
			return false;
		
		token = longest_token;
		for(const auto& c: longest_token.str)
		{
			switch(c.data)
			{
				case '\n':
					++line_number;
					column_number = 0;
				break;

				default:
					++column_number;
				break;
			}
		}
		stream = stream.range(longest_it, stream.end());
		return true;
	}

	bool
	Proto_Lexer::token_2(String_Range& stream, Token& token)
	{
		while(true)
		{
			String_Iterator longest_it;

			Match_Result res;
			if(run(rgx_vm2, stream, single_tape, res, MATCH_MODE::FIRST_MATCH))
			{
				auto ix = rgx_vm2.stack.back();
				rgx_vm2.stack.remove_back();
				auto& action = actions[ix];

				if(action.ignore)
				{
					stream = stream.range(res.str.end(), stream.end());
					continue;
				}

				Token tkn;
				tkn.type = action.type;
				tkn.line = line_number;
				tkn.column = column_number;
				tkn.str = res.str;
				longest_it = res.str.end();

				if(action.action != nullptr)
				{
					auto tmp = stream;
					if(action.action(res.str, tmp, tkn))
						longest_it = tmp.begin();
				}

				for(const auto& c: tkn.str)
				{
					if(c.data == '\n')
					{
						++line_number;
						column_number = 0;
					}
					else
					{
						++column_number;
					}
				}

				stream = stream.range(longest_it, stream.end());
				token = tkn;
				return true;
			}
			else
			{
				return false;
			}
		}
	}
}