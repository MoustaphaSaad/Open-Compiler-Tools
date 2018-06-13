#include "benchmark.h"

#include <rgx/Compiler.h>
#include <rgx/VM.h>
#include <rgx/Automata.h>
#include <oct/Proto_Lexer.h>
#include <oct/Proto_Parser.h>

#include <cpprelude/IO.h>
#include <cpprelude/Benchmark.h>
#include <cpprelude/String.h>
#include <cpprelude/File.h>
#include <cpprelude/Allocators.h>
#include <cpprelude/Panic.h>

#include <string>
#include <regex>

using namespace cppr;
using namespace rgx;
using namespace oct;

const char* corpse[] =
{
	"Mostafa",
	"mostafa",
	"Hani",
	"hani",
	"moustapha",
	"saad",
	"Saad",
	"moustaphaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
	"abc",
	"def",
	"ab",
	"abccccccccc",
	"a@b.c",
	"moustapha.saad.abdelhamed@gmail.com",
	"mostafa.saad@symbyo.com",
	"ueskwhgfueakwf@fsdsdfsd.sdcsdf",
	"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
	"\"Mostafa Says Hello\"",
	"\"Mostafa Says \\\"Hello\\\"\""
};
usize corpse_index = 18;
Stack_Allocator rgx_stack(MEGABYTES(25));
//auto rgx_stack = os->global_memory;

void
rand_data()
{
	corpse_index = rand() % 19;
}

bool
compiled_linear_match(const cppr::String_Range& input, rgx::Match_Result& res)
{
	auto it = input.begin();
	Rune c{};
	constexpr usize STATES_MAX = 1024;
	usize states[STATES_MAX] = {0};
	String_Iterator states_data[STATES_MAX];
	usize state_pointer = 0;
	usize current_state = 0;
	bool halt = false;
	while(!halt && current_state != static_cast<usize>(-1))
	{
			switch(current_state)
			{
			case 0:{
					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'm') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'o') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'u') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 's') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 't') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'a') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'p') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'h') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'a') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'a') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'a') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'a') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'a') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'a') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'a') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'a') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'a') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'a') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'a') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'a') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'a') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'a') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'a') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'a') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'a') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'a') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'a') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'a') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'a') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'a') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'a') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'a') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'a') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'a') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'a') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'a') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'a') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'a') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'a') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'a') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'a') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'a') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'a') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'a') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'a') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'a') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'a') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'a') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'a') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'a') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'a') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'a') { current_state = static_cast<usize>(-1); break; }
					++it;

					halt = true;
					} break;
			}
			if(current_state == static_cast<usize>(-1) && state_pointer != 0){
					--state_pointer;
					current_state = states[state_pointer];
					it = states_data[state_pointer];
			}
	}
	if(halt) { res.accepted = true; res.str = input.range(input.begin(), it); return true; }
	return false;
}

bool
compiled_or(const cppr::String_Range& input, rgx::Match_Result& res)
{
	auto it = input.begin();
	Rune c{};
	constexpr usize STATES_MAX = 1024;
	usize states[STATES_MAX] = {0};
	String_Iterator states_data[STATES_MAX];
	usize state_pointer = 0;
	usize current_state = 0;
	bool halt = false;
	while(!halt && current_state != static_cast<usize>(-1))
	{
			switch(current_state)
			{
			case 0:{
					if(state_pointer == STATES_MAX) return false;
					current_state = 1;
					states[state_pointer] = 5;
					states_data[state_pointer] = it;
					++state_pointer;
					current_state = 1;
					} break;
			case 1:{
					if(state_pointer == STATES_MAX) return false;
					current_state = 2;
					states[state_pointer] = 3;
					states_data[state_pointer] = it;
					++state_pointer;
					current_state = 2;
					} break;
			case 2:{
					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 's') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'a') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'a') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'd') { current_state = static_cast<usize>(-1); break; }
					++it;

					current_state = 4;
					current_state = 3;
					} break;
			case 3:{
					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'h') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'a') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'n') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'i') { current_state = static_cast<usize>(-1); break; }
					++it;

					current_state = 4;
					} break;
			case 4:{
					current_state = 6;
					current_state = 5;
					} break;
			case 5:{
					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'm') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'o') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'u') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 's') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 't') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'a') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'p') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'h') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != 'a') { current_state = static_cast<usize>(-1); break; }
					++it;

					current_state = 6;
					} break;
			case 6:{
					halt = true;
					current_state = 7;
					} break;
			default:{
					current_state = static_cast<usize>(-1);
			} break;
			}
			if(current_state == static_cast<usize>(-1) && state_pointer != 0){
					--state_pointer;
					current_state = states[state_pointer];
					it = states_data[state_pointer];
			}
	}
	if(halt) { res.accepted = true; res.str = input.range(input.begin(), it); return true; }
	return false;
}

bool
compiled_star(const cppr::String_Range& input, rgx::Match_Result& res)
{
	auto it = input.begin();
	Rune c{};
	constexpr usize STATES_MAX = 1024;
	usize states[STATES_MAX] = {0};
	String_Iterator states_data[STATES_MAX];
	usize state_pointer = 0;
	usize current_state = 0;
	bool halt = false;
	while(!halt && current_state != static_cast<usize>(-1))
	{
		switch(current_state)
		{
		case 0:{
				if(state_pointer == STATES_MAX) return false;
				current_state = 1;
				states[state_pointer] = 5;
				states_data[state_pointer] = it;
				++state_pointer;
				} break;
		case 1:{
				if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
				c = *it;
				if(c != 'a') { current_state = static_cast<usize>(-1); break; }
				++it;

				if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
				c = *it;
				if(c != 'b') { current_state = static_cast<usize>(-1); break; }
				++it;

				current_state = 2;
				} break;
		case 2:{
				if(state_pointer == STATES_MAX) return false;
				current_state = 3;
				states[state_pointer] = 4;
				states_data[state_pointer] = it;
				++state_pointer;
				} break;
		case 3:{
				if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
				c = *it;
				if(c != 'c') { current_state = static_cast<usize>(-1); break; }
				++it;

				current_state = 2;
				} break;
		case 4:{
				current_state = 6;
				} break;
		case 5:{
				if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
				c = *it;
				if(c != 'd') { current_state = static_cast<usize>(-1); break; }
				++it;

				if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
				c = *it;
				if(c != 'e') { current_state = static_cast<usize>(-1); break; }
				++it;

				if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
				c = *it;
				if(c != 'f') { current_state = static_cast<usize>(-1); break; }
				++it;

				current_state = 6;
				} break;
		case 6:{
				halt = true;
				} break;
		default:{
				current_state = static_cast<usize>(-1);
		} break;
		}
		if(current_state == static_cast<usize>(-1) && state_pointer != 0){
				--state_pointer;
				current_state = states[state_pointer];
				it = states_data[state_pointer];
		}
	}
	if(halt) { res.accepted = true; res.str = input.range(input.begin(), it); return true; }
	return false;
}

bool
compiled_plus(const cppr::String_Range& input, rgx::Match_Result& res)
{
	auto it = input.begin();
	Rune c{};
	constexpr usize STATES_MAX = 1024;
	usize states[STATES_MAX] = {0};
	String_Iterator states_data[STATES_MAX];
	usize state_pointer = 0;
	usize current_state = 0;
	bool halt = false;
	while(!halt && current_state != static_cast<usize>(-1))
	{
		switch(current_state)
		{
		case 0:{
				if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
				c = *it;
				if(c != 'a') { current_state = static_cast<usize>(-1); break; }
				++it;

				if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
				c = *it;
				if(c != 'b') { current_state = static_cast<usize>(-1); break; }
				++it;

				if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
				c = *it;
				if(c != 'c') { current_state = static_cast<usize>(-1); break; }
				++it;

				current_state = 1;
				} break;
		case 1:{
				if(state_pointer == STATES_MAX) return false;
				current_state = 2;
				states[state_pointer] = 3;
				states_data[state_pointer] = it;
				++state_pointer;
				} break;
		case 2:{
				if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
				c = *it;
				if(c != 'c') { current_state = static_cast<usize>(-1); break; }
				++it;

				current_state = 1;
				} break;
		case 3:{
				halt = true;
				} break;
		default:{
				current_state = static_cast<usize>(-1);
		} break;
		}
		if(current_state == static_cast<usize>(-1) && state_pointer != 0){
				--state_pointer;
				current_state = states[state_pointer];
				it = states_data[state_pointer];
		}
	}
	if(halt) { res.accepted = true; res.str = input.range(input.begin(), it); return true; }
	return false;
}

bool
compiled_email(const cppr::String_Range& input, rgx::Match_Result& res)
{
	auto it = input.begin();
	Rune c{};
	constexpr usize STATES_MAX = 1024;
	usize states[STATES_MAX] = {0};
	String_Iterator states_data[STATES_MAX];
	usize state_pointer = 0;
	usize current_state = 0;
	bool halt = false;
	while(!halt && current_state != static_cast<usize>(-1))
	{
			switch(current_state)
			{
			case 0:{
					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(
							c == 'a' ||
							c == 'b' ||
							c == 'c' ||
							c == 'd' ||
							c == 'e' ||
							c == 'f' ||
							c == 'g' ||
							c == 'h' ||
							c == 'i' ||
							c == 'j' ||
							c == 'k' ||
							c == 'l' ||
							c == 'm' ||
							c == 'n' ||
							c == 'o' ||
							c == 'p' ||
							c == 'q' ||
							c == 'r' ||
							c == 's' ||
							c == 't' ||
							c == 'u' ||
							c == 'v' ||
							c == 'w' ||
							c == 'x' ||
							c == 'y' ||
							c == 'z' ||
							c == 'A' ||
							c == 'B' ||
							c == 'C' ||
							c == 'D' ||
							c == 'E' ||
							c == 'F' ||
							c == 'G' ||
							c == 'H' ||
							c == 'I' ||
							c == 'J' ||
							c == 'K' ||
							c == 'L' ||
							c == 'M' ||
							c == 'N' ||
							c == 'O' ||
							c == 'P' ||
							c == 'Q' ||
							c == 'R' ||
							c == 'S' ||
							c == 'T' ||
							c == 'U' ||
							c == 'V' ||
							c == 'W' ||
							c == 'X' ||
							c == 'Y' ||
							c == 'Z' ||
							c == '0' ||
							c == '1' ||
							c == '2' ||
							c == '3' ||
							c == '4' ||
							c == '5' ||
							c == '6' ||
							c == '7' ||
							c == '8' ||
							c == '9' ||
							c == '_' ||
							c == '.' ||
							c == '+' ||
							c == '-')
					{
							++it;
					}
					else
					{
							current_state = static_cast<usize>(-1); break;
					}
					current_state = 1;
					} break;
			case 1:{
					if(state_pointer == STATES_MAX) return false;
					current_state = 2;
					states[state_pointer] = 3;
					states_data[state_pointer] = it;
					++state_pointer;
					} break;
			case 2:{
					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(
							c == 'a' ||
							c == 'b' ||
							c == 'c' ||
							c == 'd' ||
							c == 'e' ||
							c == 'f' ||
							c == 'g' ||
							c == 'h' ||
							c == 'i' ||
							c == 'j' ||
							c == 'k' ||
							c == 'l' ||
							c == 'm' ||
							c == 'n' ||
							c == 'o' ||
							c == 'p' ||
							c == 'q' ||
							c == 'r' ||
							c == 's' ||
							c == 't' ||
							c == 'u' ||
							c == 'v' ||
							c == 'w' ||
							c == 'x' ||
							c == 'y' ||
							c == 'z' ||
							c == 'A' ||
							c == 'B' ||
							c == 'C' ||
							c == 'D' ||
							c == 'E' ||
							c == 'F' ||
							c == 'G' ||
							c == 'H' ||
							c == 'I' ||
							c == 'J' ||
							c == 'K' ||
							c == 'L' ||
							c == 'M' ||
							c == 'N' ||
							c == 'O' ||
							c == 'P' ||
							c == 'Q' ||
							c == 'R' ||
							c == 'S' ||
							c == 'T' ||
							c == 'U' ||
							c == 'V' ||
							c == 'W' ||
							c == 'X' ||
							c == 'Y' ||
							c == 'Z' ||
							c == '0' ||
							c == '1' ||
							c == '2' ||
							c == '3' ||
							c == '4' ||
							c == '5' ||
							c == '6' ||
							c == '7' ||
							c == '8' ||
							c == '9' ||
							c == '_' ||
							c == '.' ||
							c == '+' ||
							c == '-')
					{
							++it;
					}
					else
					{
							current_state = static_cast<usize>(-1); break;
					}
					current_state = 1;
					} break;
			case 3:{
					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != '@') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(
							c == 'a' ||
							c == 'b' ||
							c == 'c' ||
							c == 'd' ||
							c == 'e' ||
							c == 'f' ||
							c == 'g' ||
							c == 'h' ||
							c == 'i' ||
							c == 'j' ||
							c == 'k' ||
							c == 'l' ||
							c == 'm' ||
							c == 'n' ||
							c == 'o' ||
							c == 'p' ||
							c == 'q' ||
							c == 'r' ||
							c == 's' ||
							c == 't' ||
							c == 'u' ||
							c == 'v' ||
							c == 'w' ||
							c == 'x' ||
							c == 'y' ||
							c == 'z' ||
							c == 'A' ||
							c == 'B' ||
							c == 'C' ||
							c == 'D' ||
							c == 'E' ||
							c == 'F' ||
							c == 'G' ||
							c == 'H' ||
							c == 'I' ||
							c == 'J' ||
							c == 'K' ||
							c == 'L' ||
							c == 'M' ||
							c == 'N' ||
							c == 'O' ||
							c == 'P' ||
							c == 'Q' ||
							c == 'R' ||
							c == 'S' ||
							c == 'T' ||
							c == 'U' ||
							c == 'V' ||
							c == 'W' ||
							c == 'X' ||
							c == 'Y' ||
							c == 'Z' ||
							c == '0' ||
							c == '1' ||
							c == '2' ||
							c == '3' ||
							c == '4' ||
							c == '5' ||
							c == '6' ||
							c == '7' ||
							c == '8' ||
							c == '9' ||
							c == '-')
					{
							++it;
					}
					else
					{
							current_state = static_cast<usize>(-1); break;
					}
					current_state = 4;
					} break;
			case 4:{
					if(state_pointer == STATES_MAX) return false;
					current_state = 5;
					states[state_pointer] = 6;
					states_data[state_pointer] = it;
					++state_pointer;
					} break;
			case 5:{
					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(
							c == 'a' ||
							c == 'b' ||
							c == 'c' ||
							c == 'd' ||
							c == 'e' ||
							c == 'f' ||
							c == 'g' ||
							c == 'h' ||
							c == 'i' ||
							c == 'j' ||
							c == 'k' ||
							c == 'l' ||
							c == 'm' ||
							c == 'n' ||
							c == 'o' ||
							c == 'p' ||
							c == 'q' ||
							c == 'r' ||
							c == 's' ||
							c == 't' ||
							c == 'u' ||
							c == 'v' ||
							c == 'w' ||
							c == 'x' ||
							c == 'y' ||
							c == 'z' ||
							c == 'A' ||
							c == 'B' ||
							c == 'C' ||
							c == 'D' ||
							c == 'E' ||
							c == 'F' ||
							c == 'G' ||
							c == 'H' ||
							c == 'I' ||
							c == 'J' ||
							c == 'K' ||
							c == 'L' ||
							c == 'M' ||
							c == 'N' ||
							c == 'O' ||
							c == 'P' ||
							c == 'Q' ||
							c == 'R' ||
							c == 'S' ||
							c == 'T' ||
							c == 'U' ||
							c == 'V' ||
							c == 'W' ||
							c == 'X' ||
							c == 'Y' ||
							c == 'Z' ||
							c == '0' ||
							c == '1' ||
							c == '2' ||
							c == '3' ||
							c == '4' ||
							c == '5' ||
							c == '6' ||
							c == '7' ||
							c == '8' ||
							c == '9' ||
							c == '-')
					{
							++it;
					}
					else
					{
							current_state = static_cast<usize>(-1); break;
					}
					current_state = 4;
					} break;
			case 6:{
					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(c != '.') { current_state = static_cast<usize>(-1); break; }
					++it;

					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(
							c == 'a' ||
							c == 'b' ||
							c == 'c' ||
							c == 'd' ||
							c == 'e' ||
							c == 'f' ||
							c == 'g' ||
							c == 'h' ||
							c == 'i' ||
							c == 'j' ||
							c == 'k' ||
							c == 'l' ||
							c == 'm' ||
							c == 'n' ||
							c == 'o' ||
							c == 'p' ||
							c == 'q' ||
							c == 'r' ||
							c == 's' ||
							c == 't' ||
							c == 'u' ||
							c == 'v' ||
							c == 'w' ||
							c == 'x' ||
							c == 'y' ||
							c == 'z' ||
							c == 'A' ||
							c == 'B' ||
							c == 'C' ||
							c == 'D' ||
							c == 'E' ||
							c == 'F' ||
							c == 'G' ||
							c == 'H' ||
							c == 'I' ||
							c == 'J' ||
							c == 'K' ||
							c == 'L' ||
							c == 'M' ||
							c == 'N' ||
							c == 'O' ||
							c == 'P' ||
							c == 'Q' ||
							c == 'R' ||
							c == 'S' ||
							c == 'T' ||
							c == 'U' ||
							c == 'V' ||
							c == 'W' ||
							c == 'X' ||
							c == 'Y' ||
							c == 'Z' ||
							c == '0' ||
							c == '1' ||
							c == '2' ||
							c == '3' ||
							c == '4' ||
							c == '5' ||
							c == '6' ||
							c == '7' ||
							c == '8' ||
							c == '9' ||
							c == '-' ||
							c == '.')
					{
							++it;
					}
					else
					{
							current_state = static_cast<usize>(-1); break;
					}
					current_state = 7;
					} break;
			case 7:{
					if(state_pointer == STATES_MAX) return false;
					current_state = 8;
					states[state_pointer] = 9;
					states_data[state_pointer] = it;
					++state_pointer;
					} break;
			case 8:{
					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(
							c == 'a' ||
							c == 'b' ||
							c == 'c' ||
							c == 'd' ||
							c == 'e' ||
							c == 'f' ||
							c == 'g' ||
							c == 'h' ||
							c == 'i' ||
							c == 'j' ||
							c == 'k' ||
							c == 'l' ||
							c == 'm' ||
							c == 'n' ||
							c == 'o' ||
							c == 'p' ||
							c == 'q' ||
							c == 'r' ||
							c == 's' ||
							c == 't' ||
							c == 'u' ||
							c == 'v' ||
							c == 'w' ||
							c == 'x' ||
							c == 'y' ||
							c == 'z' ||
							c == 'A' ||
							c == 'B' ||
							c == 'C' ||
							c == 'D' ||
							c == 'E' ||
							c == 'F' ||
							c == 'G' ||
							c == 'H' ||
							c == 'I' ||
							c == 'J' ||
							c == 'K' ||
							c == 'L' ||
							c == 'M' ||
							c == 'N' ||
							c == 'O' ||
							c == 'P' ||
							c == 'Q' ||
							c == 'R' ||
							c == 'S' ||
							c == 'T' ||
							c == 'U' ||
							c == 'V' ||
							c == 'W' ||
							c == 'X' ||
							c == 'Y' ||
							c == 'Z' ||
							c == '0' ||
							c == '1' ||
							c == '2' ||
							c == '3' ||
							c == '4' ||
							c == '5' ||
							c == '6' ||
							c == '7' ||
							c == '8' ||
							c == '9' ||
							c == '-' ||
							c == '.')
					{
							++it;
					}
					else
					{
							current_state = static_cast<usize>(-1); break;
					}
					current_state = 7;
					} break;
			case 9:{
					halt = true;
					} break;
			default:{
					current_state = static_cast<usize>(-1);
			} break;
			}
			if(current_state == static_cast<usize>(-1) && state_pointer != 0){
					--state_pointer;
					current_state = states[state_pointer];
					it = states_data[state_pointer];
			}
	}
	if(halt) { res.accepted = true; res.str = input.range(input.begin(), it); return true; }
	return false;
}

bool
compiled_cid(const cppr::String_Range& input, rgx::Match_Result& res)
{
	auto it = input.begin();
	Rune c{};
	constexpr usize STATES_MAX = 1024;
	usize states[STATES_MAX] = {0};
	String_Iterator states_data[STATES_MAX];
	usize state_pointer = 0;
	usize current_state = 0;
	bool halt = false;
	while(!halt && current_state != static_cast<usize>(-1))
	{
			switch(current_state)
			{
			case 0:{
					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(
							c == 'a' ||
							c == 'b' ||
							c == 'c' ||
							c == 'd' ||
							c == 'e' ||
							c == 'f' ||
							c == 'g' ||
							c == 'h' ||
							c == 'i' ||
							c == 'j' ||
							c == 'k' ||
							c == 'l' ||
							c == 'm' ||
							c == 'n' ||
							c == 'o' ||
							c == 'p' ||
							c == 'q' ||
							c == 'r' ||
							c == 's' ||
							c == 't' ||
							c == 'u' ||
							c == 'v' ||
							c == 'w' ||
							c == 'x' ||
							c == 'y' ||
							c == 'z' ||
							c == 'A' ||
							c == 'B' ||
							c == 'C' ||
							c == 'D' ||
							c == 'E' ||
							c == 'F' ||
							c == 'G' ||
							c == 'H' ||
							c == 'I' ||
							c == 'J' ||
							c == 'K' ||
							c == 'L' ||
							c == 'M' ||
							c == 'N' ||
							c == 'O' ||
							c == 'P' ||
							c == 'Q' ||
							c == 'R' ||
							c == 'S' ||
							c == 'T' ||
							c == 'U' ||
							c == 'V' ||
							c == 'W' ||
							c == 'X' ||
							c == 'Y' ||
							c == 'Z' ||
							c == '_')
					{
							++it;
					}
					else
					{
							current_state = static_cast<usize>(-1); break;
					}
					current_state = 1;
					} break;
			case 1:{
					if(state_pointer == STATES_MAX) return false;
					current_state = 2;
					states[state_pointer] = 3;
					states_data[state_pointer] = it;
					++state_pointer;
					} break;
			case 2:{
					if(it == input.end()) { current_state = static_cast<usize>(-1); break; }
					c = *it;
					if(
							c == 'a' ||
							c == 'b' ||
							c == 'c' ||
							c == 'd' ||
							c == 'e' ||
							c == 'f' ||
							c == 'g' ||
							c == 'h' ||
							c == 'i' ||
							c == 'j' ||
							c == 'k' ||
							c == 'l' ||
							c == 'm' ||
							c == 'n' ||
							c == 'o' ||
							c == 'p' ||
							c == 'q' ||
							c == 'r' ||
							c == 's' ||
							c == 't' ||
							c == 'u' ||
							c == 'v' ||
							c == 'w' ||
							c == 'x' ||
							c == 'y' ||
							c == 'z' ||
							c == 'A' ||
							c == 'B' ||
							c == 'C' ||
							c == 'D' ||
							c == 'E' ||
							c == 'F' ||
							c == 'G' ||
							c == 'H' ||
							c == 'I' ||
							c == 'J' ||
							c == 'K' ||
							c == 'L' ||
							c == 'M' ||
							c == 'N' ||
							c == 'O' ||
							c == 'P' ||
							c == 'Q' ||
							c == 'R' ||
							c == 'S' ||
							c == 'T' ||
							c == 'U' ||
							c == 'V' ||
							c == 'W' ||
							c == 'X' ||
							c == 'Y' ||
							c == 'Z' ||
							c == '0' ||
							c == '1' ||
							c == '2' ||
							c == '3' ||
							c == '4' ||
							c == '5' ||
							c == '6' ||
							c == '7' ||
							c == '8' ||
							c == '9' ||
							c == '_')
					{
							++it;
					}
					else
					{
							current_state = static_cast<usize>(-1); break;
					}
					current_state = 1;
					} break;
			case 3:{
					halt = true;
					} break;
			default:{
					current_state = static_cast<usize>(-1);
			} break;
			}
			if(current_state == static_cast<usize>(-1) && state_pointer != 0){
					--state_pointer;
					current_state = states[state_pointer];
					it = states_data[state_pointer];
			}
	}
	if(halt) { res.accepted = true; res.str = input.range(input.begin(), it); return true; }
	return false;
}

bool
bm_linear_match(Stopwatch& watch)
{
	watch.start();
	bool result;
	{
		rgx_stack.free_all();
		Tape exp(rgx_stack);
		VM_State vm(rgx_stack);
		compile("moustaphaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"_rng, exp, true, rgx_stack);
		result = run(vm, make_strrng(corpse[corpse_index]), exp);
	}
	watch.stop();
	return result;
}

bool
bm_compiled_linear_match(Stopwatch& watch)
{
	watch.start();
	bool result;
	{
		Match_Result res;
		result = compiled_linear_match(make_strrng(corpse[corpse_index]), res);
	}
	watch.stop();
	return result;
}

bool
bm_std_linear_match(Stopwatch& watch)
{
	watch.start();
	bool result;
	{
		std::regex exp("moustaphaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
		std::smatch mtch;
		std::string data(corpse[corpse_index]);
		std::regex_match(data, mtch, exp);
		result = !mtch.empty();
	}
	watch.stop();
	return result;
}


bool
bm_or(Stopwatch& watch)
{
	watch.start();
	bool result;
	{
		rgx_stack.free_all();
		Tape exp(rgx_stack);
		VM_State vm(rgx_stack);
		compile("saad|hani|moustapha"_rng, exp, true, rgx_stack);
		result = run(vm, make_strrng(corpse[corpse_index]), exp);
	}
	watch.stop();
	return result;
}

bool
bm_compiled_or(Stopwatch& watch)
{
	watch.start();
	bool result;
	{
		Match_Result res;
		result = compiled_or(make_strrng(corpse[corpse_index]), res);
	}
	watch.stop();
	return result;
}

bool
bm_std_or(Stopwatch& watch)
{
	watch.start();
	bool result;
	{
		std::regex exp("saad|hani|moustapha");
		std::smatch mtch;
		std::string data(corpse[corpse_index]);
		std::regex_match(data, mtch, exp);
		result = !mtch.empty();
	}
	watch.stop();
	return result;
}


bool
bm_star(Stopwatch& watch)
{
	watch.start();
	bool result;
	{
		rgx_stack.free_all();
		Tape exp(rgx_stack);
		VM_State vm(rgx_stack);
		compile("abc*|def"_rng, exp, true, rgx_stack);
		result = run(vm, make_strrng(corpse[corpse_index]), exp);
	}
	watch.stop();
	return result;
}

bool
bm_compiled_star(Stopwatch& watch)
{
	watch.start();
	bool result;
	{
		Match_Result res;
		result = compiled_star(make_strrng(corpse[corpse_index]), res);
	}
	watch.stop();
	return result;
}

bool
bm_std_star(Stopwatch& watch)
{
	watch.start();
	bool result;

	{
		std::regex exp("abc*|def");
		std::smatch mtch;
		std::string data(corpse[corpse_index]);
		std::regex_match(data, mtch, exp);
		result = !mtch.empty();
	}
	watch.stop();
	return result;
}

bool
bm_plus(Stopwatch& watch)
{
	watch.start();
	bool result;
	{
		rgx_stack.free_all();
		Tape exp(rgx_stack);
		VM_State vm(rgx_stack);
		compile("abc+"_rng, exp, true, rgx_stack);
		result = run(vm, make_strrng(corpse[corpse_index]), exp);
	}
	watch.stop();
	return result;
}

bool
bm_compiled_plus(Stopwatch& watch)
{
	watch.start();
	bool result;
	{
		Match_Result res;
		result = compiled_plus(make_strrng(corpse[corpse_index]), res);
	}
	watch.stop();
	return result;
}

bool
bm_std_plus(Stopwatch& watch)
{
	watch.start();
	bool result;
	{
		std::regex exp("abc+");
		std::smatch mtch;
		std::string data(corpse[corpse_index]);
		std::regex_match(data, mtch, exp);
		result = !mtch.empty();
	}
	watch.stop();
	return result;
}


bool
bm_email(Stopwatch& watch)
{
	watch.start();
	bool result;
	{
		rgx_stack.free_all();
		Cached_Tape exp(rgx_stack);
		VM_State vm(rgx_stack);
		compile("[a-zA-Z0-9_.+\\-]+@[a-zA-Z0-9\\-]+\\.[a-zA-Z0-9\\-.]+"_rng, exp, true, rgx_stack);
		result = run(vm, make_strrng(corpse[corpse_index]), exp);
	}
	watch.stop();
	return result;
}

bool
bm_compiled_email(Stopwatch& watch)
{
	watch.start();
	bool result;
	{
		Match_Result res;
		result = compiled_email(make_strrng(corpse[corpse_index]), res);
	}
	watch.stop();
	return result;
}

bool
bm_std_email(Stopwatch& watch)
{
	watch.start();
	bool result;
	{
		std::regex exp("[a-zA-Z0-9_.+-]+@[a-zA-Z0-9-]+\\.[a-zA-Z0-9-.]+");
		std::smatch mtch;
		std::string data(corpse[corpse_index]);
		std::regex_match(data, mtch, exp);
		result = !mtch.empty();
	}
	watch.stop();
	return result;
}


bool
bm_cid(Stopwatch& watch)
{
	watch.start();
	bool result;
	{
		rgx_stack.free_all();
		Tape exp(rgx_stack);
		VM_State vm(rgx_stack);
		compile("[a-zA-Z_][a-zA-Z0-9_]*"_rng, exp, true, rgx_stack);
		result = run(vm, make_strrng(corpse[corpse_index]), exp);
	}
	watch.stop();
	return result;
}

bool
bm_compiled_cid(Stopwatch& watch)
{
	watch.start();
	bool result;
	{
		Match_Result res;
		result = compiled_cid(make_strrng(corpse[corpse_index]), res);
		int w =32;
	}
	watch.stop();
	return result;
}

bool
bm_std_cid(Stopwatch& watch)
{
	watch.start();
	bool result;
	{
		std::regex exp("[a-zA-Z_][a-zA-Z0-9_]*");
		std::smatch mtch;
		std::string data(corpse[corpse_index]);
		std::regex_match(data, mtch, exp);
		result = !mtch.empty();
	}
	watch.stop();
	return result;
}


bool
bm_backtrack(Stopwatch& watch)
{
	watch.start();
	bool result;
	{
		rgx_stack.free_all();
		Tape exp(rgx_stack);
		VM_State vm(rgx_stack);
		compile("a?a?a?a?a?a?a?a?a?a?a?a?aaaaaaaaa"_rng, exp, true, rgx_stack);
		result = run(vm, make_strrng(corpse[corpse_index]), exp);
	}
	watch.stop();
	return result;
}

bool
bm_std_backtrack(Stopwatch& watch)
{
	watch.start();
	bool result;
	{
		std::regex exp("a?a?a?a?a?a?a?a?a?a?a?a?aaaaaaaaa");
		std::smatch mtch;
		std::string data(corpse[corpse_index]);
		std::regex_match(data, mtch, exp);
		result = !mtch.empty();
	}
	watch.stop();
	return result;
}


bool
bm_string(Stopwatch& watch)
{
	watch.start();
	bool result;
	{
		rgx_stack.free_all();
		Tape exp(rgx_stack);
		VM_State vm(rgx_stack);
		compile("\"(\\\\.|[^\"])*\""_rng, exp, true, rgx_stack);
		result = run(vm, make_strrng(corpse[corpse_index]), exp);
	}
	watch.stop();
	return result;
}

bool
bm_std_string(Stopwatch& watch)
{
	watch.start();
	bool result;
	{
		std::regex exp("\"(\\\\.|[^\"])*\"");
		std::smatch mtch;
		std::string data(corpse[corpse_index]);
		std::regex_match(data, mtch, exp);
		result = !mtch.empty();
	}
	watch.stop();
	return result;
}

void
do_stuff(const Tape& program, DFA& dfa)
{
	Stack_Array<usize> states;
	Stack_Array<Tape::const_iterator> ips;

	auto ip = program.begin();
	auto sp = dfa.state_add();
	while(ip != program.end())
	{
		switch(ip->ins)
		{
		case ISA::RUNE:
			++ip;
			sp = dfa.trasition_add(sp, ip->data.data);
			break;

		case ISA::MTCH:
		{
			++ip;
			u32 count = ip->count;
			++ip;
			for(usize i = 0; i < count; ++i, ++ip)
				sp = dfa.trasition_add(sp, ip->data.data);
		}
			break;

		case ISA::SPLT:
		{
			++ip;
			i32 A_offset = ip->offset;
			++ip;
			i32 B_offset = ip->offset;
			++ip;

			ips.push(ip + B_offset);
			states.push(sp);
			ip += A_offset;
		}
			break;

		case ISA::JUMP:
		{
			++ip;
			i32 offset = ip->offset;
			++ip;
			ip += offset;
		}
			break;

		case ISA::SET:
		{
			++ip;
			u32 count = ip->count;
			++ip;
			usize dest = dfa.state_add();
			for(usize i = 0; i < count; ++i, ++ip)
				dfa.trasition_add(sp, ip->data.data, dest);
			sp = dest;
		}
			break;

		case ISA::HALT:
			sp = dfa.halt(sp);
			if(!ips.empty())
			{
				ip = ips.top();
				ips.pop();
				sp = states.top();
				states.pop();
			}
			else
			{
				++ip;
			}
			break;
		default:
			++ip;
			break;
		}
	}
}

bool
run(DFA& dfa, const String_Range& stream)
{
	auto sp = dfa.start();
	for(auto c: stream)
	{
		if(dfa.transit(sp, c.data) == false)
		{
			return sp == dfa.halt_state;
		}
	}
	return false;
}

struct NFA_Node
{
	enum TYPE { BYTE, SPLT, HALT };

	TYPE type;
	union
	{
		byte data;
		NFA_Node* branches[2];
	};
};

struct NFA
{
	NFA_Node* start;
	Dynamic_Array<NFA_Node*> dangling;
};


struct NFA_Node2
{
	enum TYPE { BYTE, SPLT, HALT };

	TYPE type;
	byte data;
	usize branches[2];
};

struct NFA2
{
	Dynamic_Array<NFA_Node2> nodes;
	Dynamic_Array<usize> dangling;
	usize start;
};

void
bench1()
{
	Stopwatch watch;

	watch.reset();
	compile_2("moustaphaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"_rng);
	watch.stop();
	println(watch.milliseconds(), "ms");

	watch.reset();
	Tape program;
	compile("moustaphaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"_rng, program);
	watch.stop();
	println(watch.milliseconds(), "ms");
}

void
bench2()
{
	Stopwatch watch;

	watch.reset();
	compile_2("saad|hani|moustapha"_rng);
	watch.stop();
	println(watch.milliseconds(), "ms");

	watch.reset();
	Tape program;
	compile("saad|hani|moustapha"_rng, program);
	watch.stop();
	println(watch.milliseconds(), "ms");
}

void
bench3()
{
	Stopwatch watch;

	watch.reset();
	compile_2("abc*|def"_rng);
	watch.stop();
	println(watch.milliseconds(), "ms");

	watch.reset();
	Tape program;
	compile("abc*|def"_rng, program);
	watch.stop();
	println(watch.milliseconds(), "ms");
}

void
debug()
{
	bench1();
	bench2();
	bench3();
	return;
	Arena_Allocator node_arena;
	node_arena.block_size = MEGABYTES(25);
	int r = rand() % 10;

	NFA nfa;

	Stopwatch watch;
	watch.start();
	NFA_Node* n = nfa.start, *p = nullptr;
	for(usize i = 0 ; i < 5000000; ++i)
	{
		n = node_arena.alloc<NFA_Node>().ptr;
		if(p != nullptr)
			p->branches[0] = n;
		if(i % r == 0)
			nfa.dangling.insert_back(n);
		p = n;
	}
	for(auto n: nfa.dangling)
		n->branches[0] = nullptr;
	watch.stop();
	println(watch.milliseconds(), "ms");


	NFA2 nfa2;

	watch.reset();
	usize p2 = -1;
	for(usize i = 0 ; i < 5000000; ++i)
	{
		nfa2.nodes.insert_back(NFA_Node2{});
		if(p2 != -1)
			nfa2.nodes[p2].branches[0] = nfa2.nodes.count() - 1;
		if(i % r == 0)
			nfa2.dangling.insert_back(nfa2.nodes.count() - 1);
		p2 = nfa2.nodes.count() - 1;
	}
	
	for(auto i: nfa2.dangling)
		nfa2.nodes[i].branches[0] = 0;
	watch.stop();
	println(watch.milliseconds(), "ms");
	/*
	DFA dfa;
	Tape program;
	compile("[a-zA-Z0-9][a-zA-Z0-9_]*"_rng, program, false);

	Stopwatch watch;
	watch.start();
		do_stuff(program, dfa);
		bool debug = run(dfa, "def"_rng);
		debug = run(dfa, "abc"_rng);
		debug = run(dfa, "ghi"_rng);
		debug = run(dfa, "ghai"_rng);
	watch.stop();

	cppr::printf("dfa took: {}ms\n", watch.milliseconds());

	watch.reset();
	watch.start();
	VM_State vm;
	debug = run(vm, "def"_rng, program);
	debug = run(vm, "abc"_rng, program);
	debug = run(vm, "ghi"_rng, program);
	debug = run(vm, "ghai"_rng, program);
	watch.stop();

	cppr::printf("vm took: {}ms\n", watch.milliseconds());
	*/
	
	/*{
		Tape program;
		compile("[a-zA-Z_][a-zA-Z0-9_]*"_rng, program);
		println(program);
		cpp_gen(os->unbuf_stdout, program);
	}*/
	//return;

	// File f = unwrap(File::open("lxpr-code/test01.lxpr", IO_MODE::READ, OPEN_MODE::OPEN_ONLY), OS_ERROR::OK, "unable to open the file"_const_str);
	// auto content_mem = os->alloc<byte>(f.size() + 1);
	// auto content_slice = content_mem.all();
	// vreadb(f, content_slice);
	// content_mem[f.size()] = 0;
	// String content(std::move(content_mem));

	// Stopwatch watch;
	// watch.start();
	// Proto_Lexer lexer(rgx_stack);
	// meta_lexer_create(lexer);
	// Proto_Parser parser(&lexer);

	// Token tkn;
	// auto stream = content.all();
	// usize count = 0;
	// auto peek_context = parser.new_peek_context();
	// while(true)
	// {
	// 	if(!parser.token_peek(stream, tkn, peek_context)) break;
	// 	if(!parser.token_peek(stream, tkn, peek_context)) break;
	// 	if(!parser.token_peek(stream, tkn, peek_context)) break;
	// 	if(!parser.token_peek(stream, tkn, peek_context)) break;
	// 	if(!parser.token_peek(stream, tkn, peek_context)) break;
	// 	if(!parser.token_peek(stream, tkn, peek_context)) break;
	// 	if(!parser.token_peek(stream, tkn, peek_context)) break;
	// 	parser.token_flush(peek_context);
	// 	++count;
	// }
	// watch.stop();
	// cppr::printf("token count = {}\n", count);
	// cppr::printf("lexing = {}ms\n", watch.milliseconds());

}

void
benchmark()
{
	debug();
	return;
	srand(time(0));

	rand_data();
	corpse_index = 16;
	println(corpse_index);
	println(corpse[corpse_index]);
	
	compare_benchmarks(
		summary("std linear match"_rng, bm_std_linear_match),
		summary("linear match"_rng, bm_linear_match),
		summary("compiled linear match"_rng, bm_compiled_linear_match)
	);

	println();
	compare_benchmarks(
		summary("std or"_rng, bm_std_or),
		summary("or"_rng, bm_or),
		summary("compiled or"_rng, bm_compiled_or)
	);

	println();
	compare_benchmarks(
		summary("std star"_rng, bm_std_star),
		summary("star"_rng, bm_star),
		summary("compiled star"_rng, bm_compiled_star)
	);

	println();
	compare_benchmarks(
		summary("std plus"_rng, bm_std_plus),
		summary("plus"_rng, bm_plus),
		summary("compiled plus"_rng, bm_compiled_plus)
	);

	println();
	compare_benchmarks(
		summary("std email"_rng, bm_std_email),
		summary("email"_rng, bm_email),
		summary("compiled email"_rng, bm_compiled_email)
	);

	println();
	compare_benchmarks(
		summary("std cid"_rng, bm_std_cid),
		summary("cid"_rng, bm_cid),
		summary("compiled cid"_rng, bm_compiled_cid)
	);

	println();
	compare_benchmarks(
		summary("std backtrack"_rng, bm_std_backtrack),
		summary("backtrack"_rng, bm_backtrack)
	);

	println();
	compare_benchmarks(
		summary("std string"_rng, bm_std_string),
		summary("string"_rng, bm_string)
	);
}