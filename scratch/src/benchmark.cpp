#include "benchmark.h"

#include <rgx/Compiler.h>
#include <rgx/VM.h>
#include <cpprelude/IO.h>
#include <cmath>
#include <cpprelude/Benchmark.h>
#include <cpprelude/Allocators.h>

#include <string>
#include <regex>

using namespace cppr;
using namespace rgx;

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
	"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
};
usize corpse_index = 16;
Stack_Allocator rgx_stack(MEGABYTES(25));
//auto rgx_stack = os->global_memory;

void
rand_data()
{
	corpse_index = rand() % 16;
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
		compile("moustaphaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", exp, true, rgx_stack);
		result = run(vm, corpse[corpse_index], exp);
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
		compile("saad|hani|moustapha", exp, true, rgx_stack);
		result = run(vm, corpse[corpse_index], exp);
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
		compile("abc*|def", exp, true, rgx_stack);
		result = run(vm, corpse[corpse_index], exp);
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
		compile("abc+", exp, true, rgx_stack);
		result = run(vm, corpse[corpse_index], exp);
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
		compile("[a-zA-Z0-9_.+\\-]+@[a-zA-Z0-9\\-]+\\.[a-zA-Z0-9\\-.]+", exp, true, rgx_stack);
		result = run(vm, corpse[corpse_index], exp);
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
		compile("[a-zA-Z_][a-zA-Z0-9_]*", exp, true, rgx_stack);
		result = run(vm, corpse[corpse_index], exp);
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
		compile("a?a?a?a?a?a?a?a?a?a?a?a?aaaaaaaaa", exp, true, rgx_stack);
		result = run(vm, corpse[corpse_index], exp);
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

void
debug()
{
	Tape program;
	compile("abc*|def"_const_str, program);
	println(program);
	cpp_gen(os->unbuf_stdout, program);
	int x = 234;
}

void
benchmark()
{
	srand(time(0));

	rand_data();
	println(corpse_index);
	println(corpse[corpse_index]);
	
	compare_benchmarks(
		summary("std linear match", bm_std_linear_match),
		summary("linear match", bm_linear_match)
	);

	println();
	compare_benchmarks(
		summary("std or", bm_std_or),
		summary("or", bm_or)
	);

	println();
	compare_benchmarks(
		summary("std star", bm_std_star),
		summary("star", bm_star)
	);

	println();
	compare_benchmarks(
		summary("std plus", bm_std_plus),
		summary("plus", bm_plus)
	);

	println();
	compare_benchmarks(
		summary("std email", bm_std_email),
		summary("email", bm_email)
	);

	println();
	compare_benchmarks(
		summary("std cid", bm_std_cid),
		summary("cid", bm_cid)
	);

	println();
	compare_benchmarks(
		summary("std backtrack", bm_std_backtrack),
		summary("backtrack", bm_backtrack)
	);
}