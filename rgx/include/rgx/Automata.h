#pragma once

#include "rgx/api.h"
#include <cpprelude/Hash_Map.h>

namespace rgx
{
	struct DFA
	{
		cppr::Hash_Map<cppr::usize, cppr::Hash_Map<cppr::byte, cppr::usize>> transitions;
		cppr::usize state_counter;
		cppr::usize halt_state;

		DFA()
			:state_counter(0),
			 halt_state(-1)
		{}

		cppr::usize
		start()
		{
			return 0;
		}

		bool
		transit(cppr::usize& state, cppr::byte b) const
		{
			auto input_table_it = transitions.lookup(state);
			if(input_table_it == transitions.end())
				return false;

			auto& input_table = input_table_it->value;
			//check if end state then go there
			auto it = input_table.lookup(-1);
			if (it != input_table.end())
			{
				state = it->value;
				return false;
			}

			it = input_table.lookup(b);
			if(it != input_table.end())
			{
				state = it->value;
				
				//check if end state then go there
				input_table_it = transitions.lookup(state);
				if(input_table_it == transitions.end())
					return true;
				auto& input_table2 = input_table_it->value;
				it = input_table2.lookup(-1);
				if(it != input_table2.end())
				{
					state = it->value;
					return false;
				}
				return true;
			}

			state = -1;
			return false;
		}

		cppr::usize
		state_add()
		{
			return state_counter++;
		}

		cppr::usize
		trasition_add(cppr::usize state, cppr::byte b)
		{
			auto& input_table = transitions[state];
			auto it = input_table.lookup(b);
			if(it != input_table.end())
				return it->value;

			auto new_state = state_add();
			input_table[b] = new_state;
			return new_state;
		}

		void
		trasition_add(cppr::usize state, cppr::byte b, cppr::usize dest)
		{
			transitions[state][b] = dest;
		}

		cppr::usize
		halt(cppr::usize state)
		{
			if(halt_state == -1)
				halt_state = state_add();
			transitions[state][-1] = halt_state;
			return halt_state;
		}
	};
}