#pragma once

#include "rgx/api.h"
#include "rgx/ISA.h"
#include <cpprelude/Memory_Context.h>
#include <cpprelude/Double_List.h>
#include <cpprelude/Dynamic_Array.h>

namespace rgx
{
	struct Linear_Block
	{
		usize address, count;

		union
		{
			struct
			{
				usize new_address, new_count;
			} opt;

			struct
			{
				usize label;
			} codegen;
		};
	};

	struct Branch
	{
		enum TYPE
		{
			NONE,
			SPLT,
			JUMP
		};

		TYPE type;
		usize address;

		//we have two possibilities for the branches
		union
		{
			struct
			{
				Linear_Block* block;
			} jmp;

			struct
			{
				Linear_Block* block_1;
				Linear_Block* block_2;
			} splt;
		};

		//this is custom data for the usage of the structure
		union
		{
			struct
			{
				usize new_address;
			} opt;
		};
	};

	struct Tape_Structure
	{
		cppr::Double_List<Linear_Block> blocks;
		cppr::Dynamic_Array<Branch> branches;

		API_RGX
		Tape_Structure(const cppr::Memory_Context& context = cppr::os->global_memory);

		API_RGX void
		clear();

		API_RGX void
		reset();
	};

	API_RGX void
	build_structure(const Tape& program, Tape_Structure& structure);

	API_RGX void
	optimize(const Tape& input, Tape& output, const cppr::Memory_Context& context);
}