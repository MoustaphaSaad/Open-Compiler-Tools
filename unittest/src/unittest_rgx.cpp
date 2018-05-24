#include "catch.hpp"
#include <cpprelude/String.h>
#include <rgx/Compiler.h>
#include <rgx/VM.h>
using namespace cppr;
using namespace rgx;

TEST_CASE("concat test", "[concat]")
{
	SECTION("Case 01")
	{
		Tape program;
		VM_State vm;
		REQUIRE(compile("abc", program) == RGX_ERROR::OK);

		CHECK(run(vm, "abc", program) == true);
		CHECK(run(vm, "abd", program) == false);
		CHECK(run(vm, "abccc", program) == true);
	}

	SECTION("Case 02")
	{
		Tape program;
		VM_State vm;
		REQUIRE(compile("مصطفى", program) == RGX_ERROR::OK);

		CHECK(run(vm, "مصطفى", program) == true);
		CHECK(run(vm, "مصفى", program) == false);
		CHECK(run(vm, "مصطفى سعد", program) == true);
	}

	SECTION("Case 03")
	{
		Tape program;
		VM_State vm;
		REQUIRE(compile("a(b)c", program) == RGX_ERROR::OK);

		CHECK(run(vm, "abc", program) == true);
		CHECK(run(vm, "abd", program) == false);
		CHECK(run(vm, "abccc", program) == true);
	}

	SECTION("Case 04")
	{
		Tape program;
		VM_State vm;
		REQUIRE(compile("((a)((b)(c)))", program) == RGX_ERROR::OK);

		CHECK(run(vm, "abc", program) == true);
		CHECK(run(vm, "abd", program) == false);
		CHECK(run(vm, "abccc", program) == true);
	}

	SECTION("Case 05")
	{
		Tape program;
		VM_State vm;
		REQUIRE(compile("(\\(a\\)((b)(c)*))", program) == RGX_ERROR::OK);

		CHECK(run(vm, "abc", program) == false);
		CHECK(run(vm, "abd", program) == false);
		CHECK(run(vm, "abccc", program) == false);
		CHECK(run(vm, "(a)bccc", program) == true);
		CHECK(run(vm, "(abccc", program) == false);
		CHECK(run(vm, "(a))bccc", program) == false);
		CHECK(run(vm, "(a)bc", program) == true);
		CHECK(run(vm, "(a)b", program) == true);
		CHECK(run(vm, "(a)ccc", program) == false);
	}
}

TEST_CASE("or test", "[or]")
{
	SECTION("Case 01")
	{
		Tape program;
		VM_State vm;
		REQUIRE(compile("abc|def", program) == RGX_ERROR::OK);

		CHECK(run(vm, "abc", program) == true);
		CHECK(run(vm, "abd", program) == false);
		CHECK(run(vm, "abccc", program) == true);
		CHECK(run(vm, "def", program) == true);
		CHECK(run(vm, "dea", program) == false);
		CHECK(run(vm, "deffff", program) == true);
	}

	SECTION("Case 02")
	{
		Tape program;
		VM_State vm;
		REQUIRE(compile("مصطفى|سعد", program) == RGX_ERROR::OK);

		CHECK(run(vm, "مصطفى", program) == true);
		CHECK(run(vm, "مصفى", program) == false);
		CHECK(run(vm, "مصطفى سعد", program) == true);
		CHECK(run(vm, "سعد", program) == true);
		CHECK(run(vm, "سد", program) == false);
		CHECK(run(vm, "سعد عبد الحميد", program) == true);
	}

	SECTION("Case 03")
	{
		Tape program;
		VM_State vm;
		REQUIRE(compile("a(b|d)c", program) == RGX_ERROR::OK);

		CHECK(run(vm, "abc", program) == true);
		CHECK(run(vm, "abd", program) == false);
		CHECK(run(vm, "abccc", program) == true);
		CHECK(run(vm, "adccc", program) == true);
		CHECK(run(vm, "abdccc", program) == false);
		CHECK(run(vm, "adbccc", program) == false);
	}

	SECTION("Case 04")
	{
		Tape program;
		REQUIRE(compile("((a)|((b)(c)))", program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(run(vm, "abc", program) == true);
		CHECK(run(vm, "bc", program) == true);
		CHECK(run(vm, "abccc", program) == true);
		CHECK(run(vm, "dbccc", program) == false);
		CHECK(run(vm, "ccc", program) == false);
	}

	SECTION("Case 05")
	{
		Tape program;
		REQUIRE(compile("abc\\|def", program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(run(vm, "abc", program) == false);
		CHECK(run(vm, "def", program) == false);
		CHECK(run(vm, "abc|def", program) == true);
	}
}

TEST_CASE("star test", "[star]")
{
	SECTION("Case 01")
	{
		Tape program;
		REQUIRE(compile("abc*", program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(run(vm, "ab", program) == true);
		CHECK(run(vm, "abc", program) == true);
		CHECK(run(vm, "abccccc", program) == true);
		CHECK(run(vm, "ac", program) == false);
	}

	SECTION("Case 02")
	{
		Tape program;
		REQUIRE(compile("أبت*", program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(run(vm, "أب", program) == true);
		CHECK(run(vm, "أبت", program) == true);
		CHECK(run(vm, "أبتتتتت", program) == true);
		CHECK(run(vm, "أت", program) == false);
	}

	SECTION("Case 03")
	{
		Tape program;
		REQUIRE(compile("(abc|def)*", program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(run(vm, "", program) == true);
		CHECK(run(vm, "abc", program) == true);
		CHECK(run(vm, "def", program) == true);
		CHECK(run(vm, "abcdefabc", program) == true);
		CHECK(run(vm, "abcdefab", program) == true);
		CHECK(run(vm, "abdcefab", program) == true);
	}

	SECTION("Case 04")
	{
		Tape program;
		REQUIRE(compile("abc\\*", program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(run(vm, "abc", program) == false);
		CHECK(run(vm, "", program) == false);
		CHECK(run(vm, "abc*", program) == true);
	}

	SECTION("Case 05")
	{
		Tape program;
		REQUIRE(compile("\\**", program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(run(vm, "", program) == true);
		CHECK(run(vm, "*******", program) == true);
	}

	SECTION("Case 06")
	{
		Tape program;
		REQUIRE(compile("abc*?", program) == RGX_ERROR::OK);
		VM_State vm;

		Match_Result res;

		CHECK(run(vm, "ab", program) == true);
		CHECK(run(vm, "abc", program, res) == true);
		CHECK(strcmp(res.str, "ab"_const_str) == 0);
		res = Match_Result{};
		CHECK(run(vm, "abccccc", program, res) == true);
		CHECK(strcmp(res.str, "ab"_const_str) == 0);

		CHECK(run(vm, "ac", program) == false);
	}
}

TEST_CASE("plus test", "[plus]")
{
	SECTION("Case 01")
	{
		Tape program;
		REQUIRE(compile("abc+", program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(run(vm, "ab", program) == false);
		CHECK(run(vm, "abc", program) == true);
		CHECK(run(vm, "abccccc", program) == true);
		CHECK(run(vm, "ac", program) == false);
	}

	SECTION("Case 02")
	{
		Tape program;
		REQUIRE(compile("ÃÈÊ+", program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(run(vm, "ÃÈ", program) == false);
		CHECK(run(vm, "ÃÈÊ", program) == true);
		CHECK(run(vm, "ÃÈÊÊÊÊ", program) == true);
		CHECK(run(vm, "ÃÊ", program) == false);
	}

	SECTION("Case 03")
	{
		Tape program;
		REQUIRE(compile("(abc|def)+", program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(run(vm, "", program) == false);
		CHECK(run(vm, "abc", program) == true);
		CHECK(run(vm, "def", program) == true);
		CHECK(run(vm, "abcdefabc", program) == true);
		CHECK(run(vm, "abcdefab", program) == true);
		CHECK(run(vm, "abdcefab", program) == false);
	}

	SECTION("Case 04")
	{
		Tape program;
		REQUIRE(compile("abc\\+", program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(run(vm, "abc", program) == false);
		CHECK(run(vm, "", program) == false);
		CHECK(run(vm, "abc+", program) == true);
	}

	SECTION("Case 05")
	{
		Tape program;
		REQUIRE(compile("\\++", program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(run(vm, "", program) == false);
		CHECK(run(vm, "+", program) == true);
		CHECK(run(vm, "+++++++", program) == true);
	}

	SECTION("Case 06")
	{
		Tape program;
		REQUIRE(compile("abc+?", program) == RGX_ERROR::OK);
		VM_State vm;
		
		Match_Result res;

		CHECK(run(vm, "ab", program, res) == false);
		CHECK(run(vm, "abc", program, res) == true);
		CHECK(run(vm, "abccccc", program, res) == true);
		CHECK(strcmp(res.str, "abc"_const_str) == 0);
	}
}

TEST_CASE("optional test", "[optional]")
{
	SECTION("Case 01")
	{
		Tape program;
		REQUIRE(compile("abc?", program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(run(vm, "ab", program) == true);
		CHECK(run(vm, "abc", program) == true);
		CHECK(run(vm, "abccccc", program) == true);
		CHECK(run(vm, "ac", program) == false);
	}

	SECTION("Case 02")
	{
		Tape program;
		REQUIRE(compile("ÃÈÊ?", program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(run(vm, "ÃÈ", program) == true);
		CHECK(run(vm, "ÃÈÊ", program) == true);
		CHECK(run(vm, "ÃÈÊÊÊÊ", program) == true);
		CHECK(run(vm, "ÃÊ", program) == false);
	}

	SECTION("Case 03")
	{
		Tape program;
		REQUIRE(compile("(abc|def)?", program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(run(vm, "", program) == true);
		CHECK(run(vm, "abc", program) == true);
		CHECK(run(vm, "def", program) == true);
		CHECK(run(vm, "abcdefabc", program) == true);
		CHECK(run(vm, "abcdefab", program) == true);
		CHECK(run(vm, "abdcefab", program) == true);
	}

	SECTION("Case 04")
	{
		Tape program;
		REQUIRE(compile("abc\\?", program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(run(vm, "abc", program) == false);
		CHECK(run(vm, "", program) == false);
		CHECK(run(vm, "abc?", program) == true);
	}

	SECTION("Case 05")
	{
		Tape program;
		REQUIRE(compile("\\??", program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(run(vm, "", program) == true);
		CHECK(run(vm, "?", program) == true);
		CHECK(run(vm, "????", program) == true);
		CHECK(run(vm, "+++++++", program) == true);
	}

	SECTION("Case 06")
	{
		Tape program;
		REQUIRE(compile("abc??", program) == RGX_ERROR::OK);
		VM_State vm;

		Match_Result res;

		CHECK(run(vm, "ab", program, res) == true);
		CHECK(run(vm, "abc", program, res) == true);
		CHECK(strcmp(res.str, "ab"_const_str) == 0);
	}
}

TEST_CASE("set test", "[set]")
{
	SECTION("Case 01")
	{
		Tape program;
		REQUIRE(compile("[abc]f", program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(run(vm, "f", program) == false);
		CHECK(run(vm, "af", program) == true);
		CHECK(run(vm, "bf", program) == true);
		CHECK(run(vm, "cf", program) == true);
		CHECK(run(vm, "zf", program) == false);
		CHECK(run(vm, "[f", program) == false);
	}

	SECTION("Case 02")
	{
		Tape program;
		REQUIRE(compile("[^abc]f", program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(run(vm, "f", program) == false);
		CHECK(run(vm, "af", program) == false);
		CHECK(run(vm, "bf", program) == false);
		CHECK(run(vm, "cf", program) == false);
		CHECK(run(vm, "zf", program) == true);
		CHECK(run(vm, "Ãf", program) == true);
	}

	SECTION("Case 03")
	{
		Tape program;
		REQUIRE(compile("[a-c]f", program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(run(vm, "f", program) == false);
		CHECK(run(vm, "af", program) == true);
		CHECK(run(vm, "bf", program) == true);
		CHECK(run(vm, "cf", program) == true);
		CHECK(run(vm, "zf", program) == false);
		CHECK(run(vm, "[f", program) == false);
	}

	SECTION("Case 04")
	{
		Tape program;
		REQUIRE(compile("[a-a]f", program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(run(vm, "f", program) == false);
		CHECK(run(vm, "af", program) == true);
		CHECK(run(vm, "bf", program) == false);
		CHECK(run(vm, "cf", program) == false);
		CHECK(run(vm, "zf", program) == false);
		CHECK(run(vm, "[f", program) == false);
	}

	SECTION("Case 05")
	{
		Tape program;
		REQUIRE(compile("[^a-c]f", program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(run(vm, "f", program) == false);
		CHECK(run(vm, "af", program) == false);
		CHECK(run(vm, "bf", program) == false);
		CHECK(run(vm, "cf", program) == false);
		CHECK(run(vm, "zf", program) == true);
		CHECK(run(vm, "Ãf", program) == true);
	}

	SECTION("Case 06")
	{
		Tape program;
		REQUIRE(compile("[^a-a]f", program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(run(vm, "f", program) == false);
		CHECK(run(vm, "af", program) == false);
		CHECK(run(vm, "bf", program) == true);
		CHECK(run(vm, "cf", program) == true);
		CHECK(run(vm, "zf", program) == true);
		CHECK(run(vm, "Ãf", program) == true);
	}

	SECTION("Case 07")
	{
		Tape program;
		REQUIRE(compile("[a\\-a]f", program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(run(vm, "f", program) == false);
		CHECK(run(vm, "af", program) == true);
		CHECK(run(vm, "bf", program) == false);
		CHECK(run(vm, "cf", program) == false);
		CHECK(run(vm, "-f", program) == true);
		CHECK(run(vm, "Ãf", program) == false);
	}

	SECTION("Case 08")
	{
		Tape program;
		REQUIRE(compile("[\\\\]f", program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(run(vm, "\\f", program) == true);
		CHECK(run(vm, "af", program) == false);
	}

	SECTION("Case 09")
	{
		Tape program;
		REQUIRE(compile("[a]f", program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(run(vm, "\\f", program) == false);
		CHECK(run(vm, "af", program) == true);
	}

	SECTION("Case 10")
	{
		Tape program;
		REQUIRE(compile("[a\\]]f", program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(run(vm, "\\f", program) == false);
		CHECK(run(vm, "af", program) == true);
		CHECK(run(vm, "]f", program) == true);
	}

	SECTION("Case 11")
	{
		Tape program;
		REQUIRE(compile("[a\\]]*f", program) == RGX_ERROR::OK);
		VM_State vm;
		
		CHECK(run(vm, "\\f", program) == false);
		CHECK(run(vm, "af", program) == true);
		CHECK(run(vm, "f", program) == true);
		CHECK(run(vm, "]f", program) == true);
		CHECK(run(vm, "a]]]]af", program) == true);	
	}
}

TEST_CASE("any test", "[any]")
{
	SECTION("Case 01")
	{
		Tape program;
		REQUIRE(compile("ab.", program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(run(vm, "abc", program) == true);
		CHECK(run(vm, "acd", program) == false);
		CHECK(run(vm, "abccc", program) == true);
	}

	SECTION("Case 02")
	{
		Tape program;
		REQUIRE(compile("ãÕØÝ.", program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(run(vm, "ãÕØÝì", program) == true);
		CHECK(run(vm, "ãÕÝì", program) == false);
		CHECK(run(vm, "ãÕØÝì ÓÚÏ", program) == true);
	}

	SECTION("Case 03")
	{
		Tape program;
		REQUIRE(compile("a(.)c", program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(run(vm, "abc", program) == true);
		CHECK(run(vm, "abd", program) == false);
		CHECK(run(vm, "abccc", program) == true);
	}

	SECTION("Case 04")
	{
		Tape program;
		REQUIRE(compile("((a)((.)(c)))", program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(run(vm, "abc", program) == true);
		CHECK(run(vm, "abd", program) == false);
		CHECK(run(vm, "abccc", program) == true);
	}

	SECTION("Case 05")
	{
		Tape program;
		REQUIRE(compile("(\\(a\\)((b)(.)*))", program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(run(vm, "abc", program) == false);
		CHECK(run(vm, "abd", program) == false);
		CHECK(run(vm, "abccc", program) == false);
		CHECK(run(vm, "(a)bccc", program) == true);
		CHECK(run(vm, "(abccc", program) == false);
		CHECK(run(vm, "(a))bccc", program) == false);
		CHECK(run(vm, "(a)bc", program) == true);
		CHECK(run(vm, "(a)b", program) == true);
		CHECK(run(vm, "(a)ccc", program) == false);
	}
}

// TEST_CASE("regex search", "[search]")
// {
// 	arena_t rgx_memory(MEGABYTES(25));

// 	SECTION("Case 01")
// 	{
// 		Tape program;
// 		REQUIRE(compile("fn[ \t\r\n]*[a-zA-Z_][a-zA-Z0-9_]*\\(.*?\\)", program) == RGX_ERROR::OK);
// 		VM_State vm;

// 		string str = "struct class int float fn foo(); fn koko();";
// 		Match_Result res;

// 		CHECK(search(str, program, res) == true);
// 		CHECK(search(str.view(res.str_view.end(), str.end()), program, res) == true);
// 	}
// }