#pragma once

#include "oct/api.h"
#include "oct/Proto_Lexer.h"

namespace oct
{
	struct META_TOKENS
	{
		enum : cppr::u32
		{
			NONE,
			WHITESPACE,				//any whitespace
			COMMENT,				//single line comment
			ASSIGN,					//=
			SEMICOLON,				//;
			COLON,					//:
			ARROW,					//>
			KEYWORD_TOKEN,			//token
			KEYWORD_IGNORE,			//ignore
			ID,						//any identifier
			STRING					//anything between double quotes
		};
	};

	API_OCT bool
	meta_lexer_create(Proto_Lexer& lexer);
}