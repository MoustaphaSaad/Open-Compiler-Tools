#include "benchmark.h"

#include <rgx/Compiler.h>
#include <rgx/VM.h>
#include <cpprelude/IO.h>
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
Stack_Allocator vm_stack(MEGABYTES(25));

void
rand_data()
{
	corpse_index = rand() % 16;
}


bool
bm_linear_match(Stopwatch& watch)
{
	watch.start();
	vm_stack.free_all();
	Tape exp(vm_stack);
	VM_State vm(vm_stack);
	compile("moustaphaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", exp, true, vm_stack);
	auto result = run(vm, corpse[corpse_index], exp);
	watch.stop();
	return result;
}

std::smatch
bm_std_linear_match(Stopwatch& watch)
{
	watch.start();
	std::regex exp("moustaphaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
	std::smatch result;
	std::string data(corpse[corpse_index]);
	std::regex_match(data, result, exp);
	watch.stop();
	return result;
}


bool
bm_or(Stopwatch& watch)
{
	watch.start();
	Tape exp;
	VM_State vm;
	compile("saad|hani|moustapha", exp);
	auto result = run(vm, corpse[corpse_index], exp);
	watch.stop();
	return result;
}

std::smatch
bm_std_or(Stopwatch& watch)
{
	watch.start();
	std::regex exp("saad|hani|moustapha");
	std::smatch result;
	std::string data(corpse[corpse_index]);
	std::regex_match(data, result, exp);
	watch.stop();
	return result;
}

bool
bm_star(Stopwatch& watch)
{
	watch.start();
	Tape exp;
	VM_State vm;
	compile("abc*|def", exp);
	auto result = run(vm, corpse[corpse_index], exp);
	watch.stop();
	return result;
}

std::smatch
bm_std_star(Stopwatch& watch)
{
	watch.start();
	std::regex exp("abc*|def");
	std::smatch result;
	std::string data(corpse[corpse_index]);
	std::regex_match(data, result, exp);
	watch.stop();
	return result;
}

bool
bm_plus(Stopwatch& watch)
{
	watch.start();
	Tape exp;
	VM_State vm;
	compile("abc+", exp);
	auto result = run(vm, corpse[corpse_index], exp);
	watch.stop();
	return result;
}

std::smatch
bm_std_plus(Stopwatch& watch)
{
	watch.start();
	std::regex exp("abc+");
	std::smatch result;
	std::string data(corpse[corpse_index]);
	std::regex_match(data, result, exp);
	watch.stop();
	return result;
}


bool
bm_email(Stopwatch& watch)
{
	watch.start();
	Tape exp;
	VM_State vm;
	compile("[a-zA-Z0-9_.+\\-]+@[a-zA-Z0-9\\-]+\\.[a-zA-Z0-9\\-.]+", exp);
	auto result = run(vm, corpse[corpse_index], exp);
	watch.stop();
	return result;
}

std::smatch
bm_std_email(Stopwatch& watch)
{
	watch.start();
	std::regex exp("[a-zA-Z0-9_.+-]+@[a-zA-Z0-9-]+\\.[a-zA-Z0-9-.]+");
	std::smatch result;
	std::string data(corpse[corpse_index]);
	std::regex_match(data, result, exp);
	watch.stop();
	return result;
}


bool
bm_cid(Stopwatch& watch)
{
	watch.start();
	Tape exp;
	VM_State vm;
	compile("[a-zA-Z_][a-zA-Z0-9_]*", exp);
	auto result = run(vm, corpse[corpse_index], exp);
	watch.stop();
	return result;
}

std::smatch
bm_std_cid(Stopwatch& watch)
{
	watch.start();
	std::regex exp("[a-zA-Z_][a-zA-Z0-9_]*");
	std::smatch result;
	std::string data(corpse[corpse_index]);
	std::regex_match(data, result, exp);
	watch.stop();
	return result;
}


bool
bm_backtrack(Stopwatch& watch)
{
	watch.start();
	Tape exp;
	VM_State vm;
	compile("a?a?a?a?a?a?a?a?a?a?a?a?aaaaaaaaa", exp);
	auto result = run(vm, corpse[corpse_index], exp);
	watch.stop();
	return result;
}

std::smatch
bm_std_backtrack(Stopwatch& watch)
{
	watch.start();
	std::regex exp("a?a?a?a?a?a?a?a?a?a?a?a?aaaaaaaaa");
	std::smatch result;
	std::string data(corpse[corpse_index]);
	std::regex_match(data, result, exp);
	watch.stop();
	return result;
}

void
benchmark()
{
	srand(time(0));
	rand_data();
	println(corpse_index);
	println(corpse[corpse_index]);
	corpse_index = 13;
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