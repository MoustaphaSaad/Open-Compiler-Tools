#pragma once

#include "oct/api.h"
#include <cpprelude/Ranges.h>
#include <cpprelude/Memory_Context.h>
#include <cpprelude/Dynamic_Array.h>
#include <cpprelude/OS.h>
#include <rgx/VM.h>

namespace oct
{
	struct Token
	{
		i32 type;
		usize line, column;
		cppr::String_Range str;
	};

	struct Token_Action
	{
		bool
		(*action)(const cppr::String_Range& match_str, cppr::String_Range& stream_str,
				  Token& token);
		i32 type;
		bool ignore;
	};

	struct Proto_Lexer
	{
		cppr::Memory_Context mem_context;
		usize line_number, column_number;
		cppr::Dynamic_Array<Token_Action> actions;
		cppr::Dynamic_Array<rgx::Tape> rgx_tapes;
		rgx::VM_State rgx_vm;

		API_OCT Proto_Lexer(const cppr::Memory_Context& context = cppr::os->global_memory);

		API_OCT bool
		push_token(const cppr::String_Range& regexp, const Token_Action& action);

		API_OCT bool
		token(cppr::String_Range& str, Token& token);
	};
}