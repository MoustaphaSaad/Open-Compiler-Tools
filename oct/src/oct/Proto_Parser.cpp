#include "oct/Proto_Parser.h"

using namespace cppr;
namespace oct
{
	Proto_Parser::Proto_Parser(const Memory_Context& context)
		:mem_context(context),
		 token_buffer(mem_context),
		 lexer(nullptr)
	{}

	Proto_Parser::Proto_Parser(Proto_Lexer* lexer, const Memory_Context& context)
		:mem_context(context),
		 token_buffer(mem_context),
		 lexer(lexer)
	{}

	Proto_Parser::Peek_Context_Type
	Proto_Parser::new_peek_context() const
	{
		return 0;
	}

	bool
	Proto_Parser::token_peek(String_Range& stream, Token& token)
	{
		if(!token_buffer.empty())
		{
			token = token_buffer.front();
			return true;
		}

		if(lexer == nullptr)
			return false;

		Token tmp_token;
		if(lexer->token(stream, tmp_token))
		{
			token = tmp_token;
			token_buffer.insert_back(tmp_token);
			return true;
		}
		
		return false;
	}

	bool
	Proto_Parser::token_peek(String_Range& stream, Token& token,
							 Proto_Parser::Peek_Context_Type& context)
	{
		if(context < token_buffer.count())
		{
			token = token_buffer[context];
			++context;
			return true;
		}

		if(lexer == nullptr)
			return false;

		Token tmp_token;
		if(lexer->token(stream, tmp_token))
		{
			token = tmp_token;
			token_buffer.insert_back(tmp_token);
			context = token_buffer.count();
			return true;
		}
		
		return false;
	}

	bool
	Proto_Parser::token_flush(Proto_Parser::Peek_Context_Type& context)
	{
		if(context <= token_buffer.count())
		{
			token_buffer.remove_front(context);
			context = new_peek_context();
			return true;
		}
		return false;
	}

	usize
	Proto_Parser::token_skip(String_Range& stream, usize count)
	{
		usize i = 0;
		for(i = 0; i < count; ++i)
		{
			if(!token_buffer.empty())
			{
				token_buffer.remove_front();
				continue;
			}

			if(lexer == nullptr)
				break;

			Token tmp_token;
			if(!lexer->token(stream, tmp_token))
				break;
		}
		return i;
	}

	bool
	Proto_Parser::token_expect(String_Range& stream, u32 token_type)
	{
		if(!token_buffer.empty())
		{
			if(token_buffer.front().type == token_type)
			{
				token_buffer.remove_front();
				return true;
			}
			return false;
		}

		if(lexer == nullptr)
			return false;

		Token tmp_token;
		if(lexer->token(stream, tmp_token))
		{
			if(tmp_token.type != token_type)
			{
				token_buffer.insert_back(tmp_token);
				return false;
			}
			return true;
		}

		return false;
	}

	bool
	Proto_Parser::token_expect(String_Range& stream, u32 token_type, Token& token)
	{
		if(!token_buffer.empty())
		{
			if(token_buffer.front().type == token_type)
			{
				token = token_buffer.front();
				token_buffer.remove_front();
				return true;
			}
			return false;
		}

		if(lexer == nullptr)
			return false;

		Token tmp_token;
		if(lexer->token(stream, tmp_token))
		{
			if(tmp_token.type != token_type)
			{
				token_buffer.insert_back(tmp_token);
				return false;
			}

			token = tmp_token;
			return true;
		}

		return false;
	}

	bool
	Proto_Parser::token_expect(cppr::String_Range& stream, Token& token)
	{
		if(!token_buffer.empty())
		{
			token = token_buffer.front();
			token_buffer.remove_front();
			return true;
		}

		if(lexer == nullptr)
			return false;

		return lexer->token(stream, token);
	}
}