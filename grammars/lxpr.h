#pragma once
#include <cpprelude/defines.h>
#include <oct/Proto_Lexer.h>
#include <oct/Proto_Parser.h>
namespace lxpr
{
	enum TOKENS: cppr::u32
	{
		TOKEN_NONE,
		T_WHITESPACE,
		T_COMMENT,
		T_ASSIGN,
		T_SEMICOLON,
		T_SCOPE,
		T_COLON,
		T_ARROW,
		T_TOKEN,
		T_IGNORE,
		T_LEXER_NAME,
		T_ID,
		T_STRING,
	};
	
	bool lexer_create(oct::Proto_Lexer& lexer);
}
