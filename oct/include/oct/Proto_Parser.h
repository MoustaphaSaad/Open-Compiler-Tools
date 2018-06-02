#pragma once

#include "oct/api.h"
#include "oct/Proto_Lexer.h"
#include <cpprelude/Ring_Array.h>

namespace oct
{
	struct Proto_Parser
	{
		using Peek_Context_Type = cppr::usize;

		cppr::Memory_Context mem_context;
		cppr::Ring_Array<Token> token_buffer;
		Proto_Lexer* lexer;

		API_OCT Proto_Parser(const cppr::Memory_Context& context = cppr::os->global_memory);

		API_OCT Proto_Parser(Proto_Lexer* lexer,
							 const cppr::Memory_Context& context = cppr::os->global_memory);

		API_OCT Peek_Context_Type
		new_peek_context() const;

		API_OCT bool
		token_peek(cppr::String_Range& stream, Token& token);

		API_OCT bool
		token_peek(cppr::String_Range& stream, Token& token, Peek_Context_Type& context);

		API_OCT bool
		token_flush(Peek_Context_Type& context);

		API_OCT bool
		token_expect(cppr::String_Range& stream, cppr::u32 token_type);

		API_OCT bool
		token_expect(cppr::String_Range& stream, cppr::u32 token_type, Token& token);
	};
}