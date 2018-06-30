#include "benchmark.h"

#include <rgx/Compiler.h>
#include <rgx/Optimizer.h>
#include <rgx/VM.h>
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

void
rand_data()
{
	corpse_index = rand() % 19;
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
		Tape exp(rgx_stack);
		VM_State vm(rgx_stack);
		compile("[a-zA-Z0-9_.+\\-]+@[a-zA-Z0-9\\-]+\\.[a-zA-Z0-9\\-.]+"_rng, exp, true, rgx_stack);
		result = run(vm, make_strrng(corpse[corpse_index]), exp);
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
debug()
{
	Tape exp(rgx_stack);
	VM_State vm(rgx_stack);
	compile("[a-zA-Z]|bc"_rng, exp, true, rgx_stack);
	pretty_print(exp);
}

void
benchmark()
{
	debug();
	return;
	srand(time(0));

	rand_data();
	corpse_index = 15;
	println(corpse_index);
	println(corpse[corpse_index]);
	
	compare_benchmarks(
		summary("std linear match"_rng, bm_std_linear_match),
		summary("linear match"_rng, bm_linear_match)
	);

	println();
	compare_benchmarks(
		summary("std or"_rng, bm_std_or),
		summary("or"_rng, bm_or)
	);

	println();
	compare_benchmarks(
		summary("std star"_rng, bm_std_star),
		summary("star"_rng, bm_star)
	);

	println();
	compare_benchmarks(
		summary("std plus"_rng, bm_std_plus),
		summary("plus"_rng, bm_plus)
	);

	println();
	compare_benchmarks(
		summary("std email"_rng, bm_std_email),
		summary("email"_rng, bm_email)
	);

	println();
	compare_benchmarks(
		summary("std cid"_rng, bm_std_cid),
		summary("cid"_rng, bm_cid)
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