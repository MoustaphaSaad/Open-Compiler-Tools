#pragma once

#include <cpprelude/defines.h>
#include <cpprelude/Ranges.h>
#include <cpprelude/Dynamic_Array.h>
#include <cpprelude/Hash_Map.h>
#include <cpprelude/IO.h>

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
		//splits the execution of a thread to two threads in different paths but favors the second path (non greedy)
		//[SPLT2: bytecode::INST]
		//[0: bytecode::OFFSET]
		//[10: bytecode::OFFSET]
		SPLT2,
		//jumps to the provided offset
		//[JUMP: bytecode::INST]
		//[-10: bytecode::OFFSET]
		JUMP,
		//do set or operation, chooses a single value from the provided list
		//[SET: bytecode::INST]
		//[3: bytecode::COUNT]
		//['a': bytecode::DATA]
		//['b': bytecode::DATA]
		//['c': bytecode::DATA]
		SET,
		//do set nor operation, checks if the data is not contained in the provided list
		//[NSET: bytecode::INST]
		//[3: bytecode::COUNT]
		//['a': bytecode::DATA]
		//['b': bytecode::DATA]
		//['c': bytecode::DATA]
		//checks that data is not a nor b nor c
		NSET,
		//an any data this
		//[RUNE: bytecode::INST]
		//[ANY: bytecode::INST]
		ANY,
		//Stops the program with success
		//[HALT: bytecode::INST]
		HALT
	};

	struct Bytecode
	{
		enum TYPE
		{
			INST,
			DATA,
			COUNT,
			OFFSET
		};

		TYPE type;
		union
		{
			ISA ins;
			cppr::Rune data;
			cppr::u32 count;
			cppr::i32 offset;
		};

		Bytecode(){}

		inline static Bytecode
		make_ins(ISA value)
		{
			Bytecode result;
			result.type = TYPE::INST;
			result.ins = value;
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
		make_count(cppr::u32 value)
		{
			Bytecode result;
			result.type = TYPE::COUNT;
			result.count = value;
			return result;
		}

		inline static Bytecode
		make_offset(cppr::i32 value)
		{
			Bytecode result;
			result.type = TYPE::OFFSET;
			result.offset = value;
			return result;
		}
	};

	using Tape = cppr::Dynamic_Array<Bytecode>;

	struct Cached_Tape
	{
		using Cache_Type = cppr::Hash_Map<cppr::usize, cppr::Hash_Set<cppr::Rune>>;
		Tape tape;
		Cache_Type cache;

		Cached_Tape(const cppr::Memory_Context& context = cppr::os->global_memory)
			:tape(context),
			 cache(context)
		{}
	};

	inline static cppr::usize
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
			case ISA::SPLT2:
				return vprints(trait, "SPLT2");
			case ISA::JUMP:
				return vprints(trait, "JUMP");
			case ISA::SET:
				return vprints(trait, "SET");
			case ISA::NSET:
				return vprints(trait, "NSET");
			case ISA::ANY:
				return vprints(trait, "ANY");
			case ISA::HALT:
				return vprints(trait, "HALT");
			case ISA::NONE:
			default:
				return vprints(trait, "NONE");
		}
	}

	inline static cppr::usize
	print_str(cppr::IO_Trait* trait, const cppr::Print_Format& format, const Bytecode& code)
	{
		switch(code.type)
		{
			case Bytecode::INST:
				return vprintf(trait, "INST[{}]", code.ins);
			case Bytecode::DATA:
				return vprintf(trait, "DATA[{:c}]", code.data);
			case Bytecode::COUNT:
				return vprintf(trait, "COUNT[{}]", code.count);
			case Bytecode::OFFSET:
				return vprintf(trait, "OFFSET[{}]", code.offset);
			default:
				return 0;
		}
	}

	inline static cppr::usize
	print_str(cppr::IO_Trait* trait, const cppr::Print_Format& format, const Tape& program)
	{
		cppr::usize result = 0;
		for(cppr::usize i = 0; i < program.count(); ++i)
		{
			result += vprintf(trait, "{:0>10}: {}\n", i, program[i]);
		}
		return result;
	}
}