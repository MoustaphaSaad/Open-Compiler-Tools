#include "oct/Lxpr.h"

using namespace cppr;
using namespace rgx;

namespace oct
{
	bool
	meta_lexer_create(Proto_Lexer& lexer)
	{
		if(!lexer.push_token("[ \t\n\r]+"_const_str, Token_Action
		{
			nullptr,
			META_TOKENS::WHITESPACE,
			true
		}))
			return false;

		if(!lexer.push_token("//[^\n]*"_const_str, Token_Action
		{
			nullptr,
			META_TOKENS::COMMENT,
			true
		}))
			return false;

		if(!lexer.push_token("="_const_str, Token_Action
		{
			nullptr,
			META_TOKENS::ASSIGN,
			false
		}))
			return false;

		if(!lexer.push_token(";"_const_str, Token_Action
		{
			nullptr,
			META_TOKENS::SEMICOLON,
			false
		}))
			return false;

		if(!lexer.push_token(":"_const_str, Token_Action
		{
			nullptr,
			META_TOKENS::COLON,
			false
		}))
			return false;

		if(!lexer.push_token(">"_const_str, Token_Action
		{
			nullptr,
			META_TOKENS::ARROW,
			false
		}))
			return false;

		if(!lexer.push_token("token"_const_str, Token_Action
		{
			nullptr,
			META_TOKENS::KEYWORD_TOKEN,
			false
		}))
			return false;

		if(!lexer.push_token("ignore"_const_str, Token_Action
		{
			nullptr,
			META_TOKENS::KEYWORD_IGNORE,
			false
		}))
			return false;

		if(!lexer.push_token("[a-zA-Z_][a-zA-Z0-9_]*"_const_str, Token_Action
		{
			nullptr,
			META_TOKENS::ID,
			false
		}))
			return false;

		if(!lexer.push_token("\"(\\\\.|[^\"])*\""_const_str, Token_Action
		{
			nullptr,
			META_TOKENS::STRING,
			false
		}))
			return false;

		// if(!lexer.push_token("\""_const_str, Token_Action
		// {
		// 	[](const String_Range& match, String_Range& stream, Token& tkn)
		// 	{
		// 		bool ignore = false;
		// 		auto str = stream;
		// 		for(str.pop_front(); !str.empty(); str.pop_front())
		// 		{
		// 			Rune c = str.front();
		// 			if(c == '\\')
		// 			{
		// 				ignore = true;
		// 			}
		// 			else if(c == '"' && !ignore)
		// 			{
		// 				tkn.str = stream.range(match.end(), str.begin());
		// 				//pop the last '"'
		// 				str.pop_front();
		// 				stream = str;
		// 				return true;
		// 			}
		// 			else
		// 			{
		// 				ignore = false;
		// 			}
		// 		}
		// 		return false;
		// 	},
		// 	META_TOKENS::STRING,
		// 	false
		// }))
		// 	return false;

		return true;
	}
}