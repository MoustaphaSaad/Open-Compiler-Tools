#pragma once
#include <cpprelude/defines.h>
#include <oct/Proto_Lexer.h>
#include <oct/Proto_Parser.h>
namespace json
{
	enum TOKENS: cppr::u32
	{
		TOKEN_NONE,
		T_WHITESPACE,
		T_OPEN_CURLY,
		T_CLOSE_CURLY,
		T_OPEN_BRACKET,
		T_CLOSE_BRACKET,
		T_COMMA,
		T_COLON,
		T_NULL,
		T_TRUE,
		T_FALSE,
		T_NUMBER,
		T_DIGITS,
		T_E,
		T_INT,
		T_FRAC,
		T_EXP,
		T_STRING,
	};
	
	bool lexer_create(oct::Proto_Lexer& lexer);
}
