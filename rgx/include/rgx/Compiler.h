#pragma once
#include "rgx/api.h"
#include "rgx/ISA.h"
#include <cpprelude/Ranges.h>
#include <cpprelude/Memory_Context.h>

namespace rgx
{
	enum class RGX_ERROR
	{
		OK,
		GENERIC_ERROR,
		OPERANDS_COUNT_MISMATCH,
		EVAL_NO_OPERATOR,
		UNIDENTIFIED_OPERATOR,
		SET_RANGE_START_RUNE_NOT_FOUND,
		SET_RANGE_INVALID,
		SET_CLOSE_BRACKET_NOT_FOUND
	};

	API_RGX RGX_ERROR
	compile(const cppr::String_Range& expression,
			Tape& program,
			bool optimize = true,
			const cppr::Memory_Context& context = cppr::os->global_memory);
}