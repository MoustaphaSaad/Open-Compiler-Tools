#include "json.h"
using namespace cppr;
using namespace rgx;
using namespace oct;
namespace json
{
	bool lexer_create(Proto_Lexer& lexer)
	{
		if(!lexer.push_token("([ \t\n\r]+)"_rng, Token_Action{ nullptr, TOKENS::T_WHITESPACE, true })) return false;
		if(!lexer.push_token("({)"_rng, Token_Action{ nullptr, TOKENS::T_OPEN_CURLY, false })) return false;
		if(!lexer.push_token("(})"_rng, Token_Action{ nullptr, TOKENS::T_CLOSE_CURLY, false })) return false;
		if(!lexer.push_token("(\\[)"_rng, Token_Action{ nullptr, TOKENS::T_OPEN_BRACKET, false })) return false;
		if(!lexer.push_token("(\\])"_rng, Token_Action{ nullptr, TOKENS::T_CLOSE_BRACKET, false })) return false;
		if(!lexer.push_token("(,)"_rng, Token_Action{ nullptr, TOKENS::T_COMMA, false })) return false;
		if(!lexer.push_token("(:)"_rng, Token_Action{ nullptr, TOKENS::T_COLON, false })) return false;
		if(!lexer.push_token("(null)"_rng, Token_Action{ nullptr, TOKENS::T_NULL, false })) return false;
		if(!lexer.push_token("(true)"_rng, Token_Action{ nullptr, TOKENS::T_TRUE, false })) return false;
		if(!lexer.push_token("(false)"_rng, Token_Action{ nullptr, TOKENS::T_FALSE, false })) return false;
		if(!lexer.push_token("((-?([0-9][0-9]*)))|((-?([0-9][0-9]*))(//.([0-9][0-9]*)))|((-?([0-9][0-9]*))((e|e\\+|e-|E|E\\+|E-)([0-9][0-9]*)))|((-?([0-9][0-9]*))(//.([0-9][0-9]*))((e|e\\+|e-|E|E\\+|E-)([0-9][0-9]*)))"_rng, Token_Action{ nullptr, TOKENS::T_NUMBER, false })) return false;
		if(!lexer.push_token("([0-9][0-9]*)"_rng, Token_Action{ nullptr, TOKENS::T_DIGITS, false })) return false;
		if(!lexer.push_token("(e|e\\+|e-|E|E\\+|E-)"_rng, Token_Action{ nullptr, TOKENS::T_E, false })) return false;
		if(!lexer.push_token("(-?([0-9][0-9]*))"_rng, Token_Action{ nullptr, TOKENS::T_INT, false })) return false;
		if(!lexer.push_token("(//.([0-9][0-9]*))"_rng, Token_Action{ nullptr, TOKENS::T_FRAC, false })) return false;
		if(!lexer.push_token("((e|e\\+|e-|E|E\\+|E-)([0-9][0-9]*))"_rng, Token_Action{ nullptr, TOKENS::T_EXP, false })) return false;
		if(!lexer.push_token("(\"(\\\\.|[^\"])*\")"_rng, Token_Action{ nullptr, TOKENS::T_STRING, false })) return false;
		return true;
	}
}
