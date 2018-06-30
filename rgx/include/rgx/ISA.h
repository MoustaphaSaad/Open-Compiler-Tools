#pragma once

#include <cpprelude/defines.h>
#include <cpprelude/Ranges.h>
#include <cpprelude/Dynamic_Array.h>
#include <cpprelude/Hash_Map.h>
#include <cpprelude/IO.h>
#include <algorithm>

namespace rgx
{
	enum class ISA
	{
		//invalid instruction
		NONE = 0,

		//matches a single rune
		//[RUNE: bytecode::INST]
		//['c': bytecode::DATA]
		RUNE,

		//matches a count of runes
		//[MTCH: bytecode::INST]
		//[3: bytecode::COUNT]
		//['a': bytecode::DATA]
		//['b': bytecode::DATA]
		//['c': bytecode::DATA]
		MTCH,

		//splits the execution of a thread to two threads in different paths but favors the first path (greedy)
		//[SPLT: bytecode::INST]
		//[0: bytecode::OFFSET]
		//[10: bytecode::OFFSET]
		SPLT,

		//jumps to the provided offset
		//[JUMP: bytecode::INST]
		//[-10: bytecode::OFFSET]
		JUMP,

		//do set or operation, chooses a single value from the provided list
		//[SET: bytecode::INST]
		//[5: bytecode::OFFSET]
		//[3: bytecode::COUNT]
		//[RNG: bytecode::INST]
		//['a': bytecode::DATA]
		//['z': bytecode::DATA]
		//['_': bytecode::DATA]
		//['@': bytecode::DATA]
		SET,

		//do set nor operation, checks if the data is not contained in the provided list
		//[NSET: bytecode::INST]
		//[3: bytecode::COUNT]
		//[RNG: bytecode::INST]
		//['a': bytecode::DATA]
		//['z': bytecode::DATA]
		//['_': bytecode::DATA]
		//['@': bytecode::DATA]
		//checks that data is not a nor b nor c ... z, also that's not `_` or `@`
		NSET,

		//specify a start and end to the next rune
		//[RNG: bytecode::INST]
		//['a': bytecode::DATA]
		//['z': bytecode::DATA]
		//specifies the range from a-z
		RNG,

		//an any data this
		//[RUNE: bytecode::INST]
		//[ANY: bytecode::INST]
		ANY,

		//Stops the program with success
		//[HALT: bytecode::INST]
		HALT,

		//Pushes a value to the vm stack
		//[PUSH: bytecode::INST]
		//[3: bytecode::VALUE]
		PUSH
	};

	struct Bytecode
	{
		enum TYPE
		{
			INST,
			DATA,
			COUNT,
			OFFSET,
			VALUE
		};

		TYPE type;
		union
		{
			ISA ins;
			cppr::Rune data;
			u32 count;
			i32 offset;
			i32 value;
		};

		Bytecode(){}

		inline static Bytecode
		make_ins(ISA val)
		{
			Bytecode result;
			result.type = TYPE::INST;
			result.ins = val;
			return result;
		}

		inline static Bytecode
		make_data(cppr::Rune c)
		{
			Bytecode result;
			result.type = TYPE::DATA;
			result.data = c;
			return result;
		}

		inline static Bytecode
		make_count(u32 val)
		{
			Bytecode result;
			result.type = TYPE::COUNT;
			result.count = val;
			return result;
		}

		inline static Bytecode
		make_offset(i32 val)
		{
			Bytecode result;
			result.type = TYPE::OFFSET;
			result.offset = val;
			return result;
		}

		inline static Bytecode
		make_value(i32 val)
		{
			Bytecode result;
			result.type = TYPE::VALUE;
			result.value = val;
			return result;
		}
	};

	using Tape = cppr::Dynamic_Array<Bytecode>;

	inline static usize
	print_str(cppr::IO_Trait* trait, const cppr::Print_Format& format, const ISA& ins)
	{
		switch(ins)
		{
			case ISA::RUNE:
				return vprints(trait, "RUNE");
			case ISA::MTCH:
				return vprints(trait, "MTCH");
			case ISA::SPLT:
				return vprints(trait, "SPLT");
			case ISA::JUMP:
				return vprints(trait, "JUMP");
			case ISA::SET:
				return vprints(trait, "SET");
			case ISA::NSET:
				return vprints(trait, "NSET");
			case ISA::RNG:
				return vprints(trait, "RNG");
			case ISA::ANY:
				return vprints(trait, "ANY");
			case ISA::HALT:
				return vprints(trait, "HALT");
			case ISA::PUSH:
				return vprints(trait, "PUSH");
			case ISA::NONE:
			default:
				return vprints(trait, "NONE");
		}
	}

	inline static usize
	print_str(cppr::IO_Trait* trait, const cppr::Print_Format& format, const Bytecode& code)
	{
		switch(code.type)
		{
			case Bytecode::INST:
				return vprintf(trait, "INST[{}]", code.ins);
			case Bytecode::DATA:
				return vprintf(trait, "DATA[{}]", code.data);
			case Bytecode::COUNT:
				return vprintf(trait, "COUNT[{}]", code.count);
			case Bytecode::OFFSET:
				return vprintf(trait, "OFFSET[{}]", code.offset);
			case Bytecode::VALUE:
				return vprintf(trait, "VALUE[{}]", code.value);
			default:
				return 0;
		}
	}

	inline static usize
	print_str(cppr::IO_Trait* trait, const cppr::Print_Format& format, const Tape& program)
	{
		usize result = 0;
		for(usize i = 0; i < program.count(); ++i)
		{
			result += vprintf(trait, "{:0>10}: {}\n", i, program[i]);
		}
		return result;
	}

	inline static void
	pretty_print(const Tape& program, cppr::IO_Trait* trait = cppr::os->unbuf_stdout)
	{
		cppr::String_Range BRANCH_COLOR, CLEAR_COLOR;
		#if defined(OS_WINDOWS)
		{
			BRANCH_COLOR = cppr::make_strrng("\x1b[36m");
			CLEAR_COLOR = cppr::make_strrng("\x1b[37m");
		}
		#elif defined(OS_LINUX)
		{
			BRANCH_COLOR = cppr::make_strrng("\e[36m");
			CLEAR_COLOR = cppr::make_strrng("\e[39m");
		}
		#endif

		cppr::Dynamic_Array<usize> jump_blocks;
		for(usize i = 0; i < program.count(); ++i)
		{
			if (program[i].type == Bytecode::INST &&
				program[i].ins == ISA::SPLT)
			{
				jump_blocks.insert_back(program[i + 1].offset + i + 3);
				jump_blocks.insert_back(program[i + 2].offset + i + 3);
			}
			else if(program[i].type == Bytecode::INST &&
					program[i].ins == ISA::JUMP)
			{
				jump_blocks.insert_back(program[i+1].offset + i + 2);
			}
		}

		auto highlight = jump_blocks.all();
		std::sort(highlight.begin(), highlight.end());
		highlight = highlight.range(highlight.begin(), std::unique(highlight.begin(), highlight.end()));

		for(usize i = 0; i < program.count(); ++i)
		{
			if(highlight.front() == i)
			{
				vprintf(trait, "{2}{0:0>10}{3}: {1}\n", i, program[i], BRANCH_COLOR, CLEAR_COLOR);
				highlight.pop_front();
			}
			else
			{
				vprintf(trait, "{:0>10}: {}\n", i, program[i]);
			}

			if (program[i].type == Bytecode::INST &&
				program[i].ins == ISA::SPLT)
			{
				vprintf(trait, "{:0>10}: {}", i + 1, program[i + 1]);
				vprintf(trait, " -> ABS[{:0>10}]\n", i + program[i + 1].offset + 3);
				vprintf(trait, "{:0>10}: {}", i + 2, program[i + 2]);
				vprintf(trait, " -> ABS[{:0>10}]\n", i + program[i + 2].offset + 3);

				i += 2;
			}
			else if(program[i].type == Bytecode::INST &&
					program[i].ins == ISA::JUMP)
			{
				vprintf(trait, "{:0>10}: {}", i + 1, program[i + 1]);
				vprintf(trait, " -> ABS[{:0>10}]\n", i + program[i + 1].offset + 2);
				++i;
			}
		}
	}
}