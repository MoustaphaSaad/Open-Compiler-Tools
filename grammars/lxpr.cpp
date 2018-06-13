#include "lxpr.h"
using namespace cppr;
using namespace rgx;
using namespace oct;
namespace lxpr
{
	bool lexer_create(Proto_Lexer& lexer)
	{
		if(!lexer.push_token("[ \t\n\r]+"_rng, Token_Action{ call, TOKENS::T_WHITESPACE, true })) return false;
		if(!lexer.push_token("//[^\n]*"_rng, Token_Action{ nullptr, TOKENS::T_COMMENT, true })) return false;
		if(!lexer.push_token("="_rng, Token_Action{ nullptr, TOKENS::T_ASSIGN, false })) return false;
		if(!lexer.push_token(";"_rng, Token_Action{ nullptr, TOKENS::T_SEMICOLON, false })) return false;
		if(!lexer.push_token("::"_rng, Token_Action{ nullptr, TOKENS::T_SCOPE, false })) return false;
		if(!lexer.push_token(":"_rng, Token_Action{ nullptr, TOKENS::T_COLON, false })) return false;
		if(!lexer.push_token(">"_rng, Token_Action{ nullptr, TOKENS::T_ARROW, false })) return false;
		if(!lexer.push_token("token"_rng, Token_Action{ nullptr, TOKENS::T_TOKEN, false })) return false;
		if(!lexer.push_token("ignore"_rng, Token_Action{ nullptr, TOKENS::T_IGNORE, false })) return false;
		if(!lexer.push_token("lexer_name"_rng, Token_Action{ nullptr, TOKENS::T_LEXER_NAME, false })) return false;
		if(!lexer.push_token("[a-zA-Z_][a-zA-Z0-9_]*"_rng, Token_Action{ nullptr, TOKENS::T_ID, false })) return false;
		if(!lexer.push_token("\"(\\\\.|[^\"])*\""_rng, Token_Action{ nullptr, TOKENS::T_STRING, false })) return false;
		return true;
	}
}
