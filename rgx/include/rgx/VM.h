#pragma once

#include "rgx/api.h"
#include "rgx/ISA.h"
#include <cpprelude/Memory_Context.h>
#include <cpprelude/Stack_Array.h>
#include <cpprelude/Dynamic_Array.h>
#include <cpprelude/OS.h>
#include <cpprelude/Ranges.h>
#include <cpprelude/IO_Trait.h>
#include <cpprelude/Loom.h>
#include <atomic>

namespace rgx
{
	enum class MATCH_MODE
	{
		FIRST_MATCH,
		SHORTEST_MATCH,
		LONGEST_MATCH
	};

	struct Thread_State
	{
		Tape::Const_Range_Type code;
		cppr::String_Range data;
	};

	struct Match_Result
	{
		cppr::String_Range str;
		bool accepted = false;
	};

	struct VM_State
	{
		cppr::Memory_Context mem_context;
		cppr::Stack_Array<Thread_State> threads;
		cppr::Dynamic_Array<cppr::String_Range> matches;
		cppr::Dynamic_Array<i32> stack;

		VM_State(const cppr::Memory_Context& context = cppr::os->global_memory)
			:mem_context(context),
			 threads(mem_context),
			 matches(mem_context),
			 stack(mem_context)
		{}

		void
		clear()
		{
			threads.clear();
			matches.clear();
			stack.clear();
		}
	};

	API_RGX bool
	run(VM_State& vm,
		const cppr::String_Range& input,
		const Tape& program,
		Match_Result& res,
		MATCH_MODE mode = MATCH_MODE::FIRST_MATCH);

	API_RGX bool
	run(VM_State& vm,
		const cppr::String_Range& input,
		const Tape& program,
		MATCH_MODE mode = MATCH_MODE::FIRST_MATCH);
}