#include "oct/Proto_Lexer.h"
#include <rgx/Compiler.h>

using namespace cppr;
using namespace rgx;

namespace oct
{
	Proto_Lexer::Proto_Lexer(const Memory_Context& context)
		:mem_context(context),
		line_number(0),
		column_number(0),
		actions(mem_context),
		rgx_tapes(mem_context),
		rgx_vm(mem_context)
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
			if(run(rgx_vm, stream, *tape_it, tmp_res))
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
}