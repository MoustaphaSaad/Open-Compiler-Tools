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
		REQUIRE(compile("abc"_rng, program) == RGX_ERROR::OK);

		CHECK(validate(program) == true);

		CHECK(run(vm, "abc"_rng, program) == true);
		CHECK(run(vm, "abd"_rng, program) == false);
		CHECK(run(vm, "abccc"_rng, program) == true);
	}

	SECTION("Case 02")
	{
		Tape program;
		VM_State vm;
		REQUIRE(compile("مصطفى"_rng, program) == RGX_ERROR::OK);
		
		CHECK(validate(program) == true);

		CHECK(run(vm, "مصطفى"_rng, program) == true);
		CHECK(run(vm, "مصفى"_rng, program) == false);
		CHECK(run(vm, "مصطفى سعد"_rng, program) == true);
	}

	SECTION("Case 03")
	{
		Tape program;
		VM_State vm;
		REQUIRE(compile("a(b)c"_rng, program) == RGX_ERROR::OK);

		CHECK(validate(program) == true);

		CHECK(run(vm, "abc"_rng, program) == true);
		CHECK(run(vm, "abd"_rng, program) == false);
		CHECK(run(vm, "abccc"_rng, program) == true);
	}

	SECTION("Case 04")
	{
		Tape program;
		VM_State vm;
		REQUIRE(compile("((a)((b)(c)))"_rng, program) == RGX_ERROR::OK);

		CHECK(validate(program) == true);

		CHECK(run(vm, "abc"_rng, program) == true);
		CHECK(run(vm, "abd"_rng, program) == false);
		CHECK(run(vm, "abccc"_rng, program) == true);
	}

	SECTION("Case 05")
	{
		Tape program;
		VM_State vm;
		REQUIRE(compile("(\\(a\\)((b)(c)*))"_rng, program) == RGX_ERROR::OK);

		CHECK(validate(program) == true);

		CHECK(run(vm, "abc"_rng, program) == false);
		CHECK(run(vm, "abd"_rng, program) == false);
		CHECK(run(vm, "abccc"_rng, program) == false);
		CHECK(run(vm, "(a)bccc"_rng, program) == true);
		CHECK(run(vm, "(abccc"_rng, program) == false);
		CHECK(run(vm, "(a))bccc"_rng, program) == false);
		CHECK(run(vm, "(a)bc"_rng, program) == true);
		CHECK(run(vm, "(a)b"_rng, program) == true);
		CHECK(run(vm, "(a)ccc"_rng, program) == false);
	}
}

TEST_CASE("or test", "[or]")
{
	SECTION("Case 01")
	{
		Tape program;
		VM_State vm;
		REQUIRE(compile("abc|def"_rng, program) == RGX_ERROR::OK);

		CHECK(validate(program) == true);

		CHECK(run(vm, "abc"_rng, program) == true);
		CHECK(run(vm, "abd"_rng, program) == false);
		CHECK(run(vm, "abccc"_rng, program) == true);
		CHECK(run(vm, "def"_rng, program) == true);
		CHECK(run(vm, "dea"_rng, program) == false);
		CHECK(run(vm, "deffff"_rng, program) == true);
	}

	SECTION("Case 02")
	{
		Tape program;
		VM_State vm;
		REQUIRE(compile("مصطفى|سعد"_rng, program) == RGX_ERROR::OK);

		CHECK(validate(program) == true);

		CHECK(run(vm, "مصطفى"_rng, program) == true);
		CHECK(run(vm, "مصفى"_rng, program) == false);
		CHECK(run(vm, "مصطفى سعد"_rng, program) == true);
		CHECK(run(vm, "سعد"_rng, program) == true);
		CHECK(run(vm, "سد"_rng, program) == false);
		CHECK(run(vm, "سعد عبد الحميد"_rng, program) == true);
	}

	SECTION("Case 03")
	{
		Tape program;
		VM_State vm;
		REQUIRE(compile("a(b|d)c"_rng, program) == RGX_ERROR::OK);

		CHECK(validate(program) == true);

		CHECK(run(vm, "abc"_rng, program) == true);
		CHECK(run(vm, "abd"_rng, program) == false);
		CHECK(run(vm, "abccc"_rng, program) == true);
		CHECK(run(vm, "adccc"_rng, program) == true);
		CHECK(run(vm, "abdccc"_rng, program) == false);
		CHECK(run(vm, "adbccc"_rng, program) == false);
	}

	SECTION("Case 04")
	{
		Tape program;
		REQUIRE(compile("((a)|((b)(c)))"_rng, program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(validate(program) == true);

		CHECK(run(vm, "abc"_rng, program) == true);
		CHECK(run(vm, "bc"_rng, program) == true);
		CHECK(run(vm, "abccc"_rng, program) == true);
		CHECK(run(vm, "dbccc"_rng, program) == false);
		CHECK(run(vm, "ccc"_rng, program) == false);
	}

	SECTION("Case 05")
	{
		Tape program;
		REQUIRE(compile("abc\\|def"_rng, program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(validate(program) == true);

		CHECK(run(vm, "abc"_rng, program) == false);
		CHECK(run(vm, "def"_rng, program) == false);
		CHECK(run(vm, "abc|def"_rng, program) == true);
	}
}

TEST_CASE("star test", "[star]")
{
	SECTION("Case 01")
	{
		Tape program;
		REQUIRE(compile("abc*"_rng, program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(validate(program) == true);

		CHECK(run(vm, "ab"_rng, program) == true);
		CHECK(run(vm, "abc"_rng, program) == true);
		CHECK(run(vm, "abccccc"_rng, program) == true);
		CHECK(run(vm, "ac"_rng, program) == false);
	}

	SECTION("Case 02")
	{
		Tape program;
		REQUIRE(compile("أبت*"_rng, program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(validate(program) == true);

		CHECK(run(vm, "أب"_rng, program) == true);
		CHECK(run(vm, "أبت"_rng, program) == true);
		CHECK(run(vm, "أبتتتتت"_rng, program) == true);
		CHECK(run(vm, "أت"_rng, program) == false);
	}

	SECTION("Case 03")
	{
		Tape program;
		REQUIRE(compile("(abc|def)*"_rng, program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(validate(program) == true);

		CHECK(run(vm, ""_rng, program) == true);
		CHECK(run(vm, "abc"_rng, program) == true);
		CHECK(run(vm, "def"_rng, program) == true);
		CHECK(run(vm, "abcdefabc"_rng, program) == true);
		CHECK(run(vm, "abcdefab"_rng, program) == true);
		CHECK(run(vm, "abdcefab"_rng, program) == true);
	}

	SECTION("Case 04")
	{
		Tape program;
		REQUIRE(compile("abc\\*"_rng, program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(validate(program) == true);

		CHECK(run(vm, "abc"_rng, program) == false);
		CHECK(run(vm, ""_rng, program) == false);
		CHECK(run(vm, "abc*"_rng, program) == true);
	}

	SECTION("Case 05")
	{
		Tape program;
		REQUIRE(compile("\\**"_rng, program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(validate(program) == true);

		CHECK(run(vm, ""_rng, program) == true);
		CHECK(run(vm, "*******"_rng, program) == true);
	}

	SECTION("Case 06")
	{
		Tape program;
		REQUIRE(compile("abc*?"_rng, program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(validate(program) == true);

		Match_Result res;

		CHECK(run(vm, "ab"_rng, program) == true);
		CHECK(run(vm, "abc"_rng, program, res) == true);
		CHECK(strcmp(res.str, "ab"_rng) == 0);
		res = Match_Result{};
		CHECK(run(vm, "abccccc"_rng, program, res) == true);
		CHECK(strcmp(res.str, "ab"_rng) == 0);

		CHECK(run(vm, "ac"_rng, program) == false);
	}
}

TEST_CASE("plus test", "[plus]")
{
	SECTION("Case 01")
	{
		Tape program;
		REQUIRE(compile("abc+"_rng, program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(validate(program) == true);

		CHECK(run(vm, "ab"_rng, program) == false);
		CHECK(run(vm, "abc"_rng, program) == true);
		CHECK(run(vm, "abccccc"_rng, program) == true);
		CHECK(run(vm, "ac"_rng, program) == false);
	}

	SECTION("Case 02")
	{
		Tape program;
		REQUIRE(compile("ÃÈÊ+"_rng, program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(validate(program) == true);

		CHECK(run(vm, "ÃÈ"_rng, program) == false);
		CHECK(run(vm, "ÃÈÊ"_rng, program) == true);
		CHECK(run(vm, "ÃÈÊÊÊÊ"_rng, program) == true);
		CHECK(run(vm, "ÃÊ"_rng, program) == false);
	}

	SECTION("Case 03")
	{
		Tape program;
		REQUIRE(compile("(abc|def)+"_rng, program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(validate(program) == true);

		CHECK(run(vm, ""_rng, program) == false);
		CHECK(run(vm, "abc"_rng, program) == true);
		CHECK(run(vm, "def"_rng, program) == true);
		CHECK(run(vm, "abcdefabc"_rng, program) == true);
		CHECK(run(vm, "abcdefab"_rng, program) == true);
		CHECK(run(vm, "abdcefab"_rng, program) == false);
	}

	SECTION("Case 04")
	{
		Tape program;
		REQUIRE(compile("abc\\+"_rng, program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(validate(program) == true);

		CHECK(run(vm, "abc"_rng, program) == false);
		CHECK(run(vm, ""_rng, program) == false);
		CHECK(run(vm, "abc+"_rng, program) == true);
	}

	SECTION("Case 05")
	{
		Tape program;
		REQUIRE(compile("\\++"_rng, program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(validate(program) == true);

		CHECK(run(vm, ""_rng, program) == false);
		CHECK(run(vm, "+"_rng, program) == true);
		CHECK(run(vm, "+++++++"_rng, program) == true);
	}

	SECTION("Case 06")
	{
		Tape program;
		REQUIRE(compile("abc+?"_rng, program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(validate(program) == true);

		Match_Result res;

		CHECK(run(vm, "ab"_rng, program, res) == false);
		CHECK(run(vm, "abc"_rng, program, res) == true);
		CHECK(run(vm, "abccccc"_rng, program, res) == true);
		CHECK(strcmp(res.str, "abc"_rng) == 0);
	}
}

TEST_CASE("optional test", "[optional]")
{
	SECTION("Case 01")
	{
		Tape program;
		REQUIRE(compile("abc?"_rng, program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(validate(program) == true);

		CHECK(run(vm, "ab"_rng, program) == true);
		CHECK(run(vm, "abc"_rng, program) == true);
		CHECK(run(vm, "abccccc"_rng, program) == true);
		CHECK(run(vm, "ac"_rng, program) == false);
	}

	SECTION("Case 02")
	{
		Tape program;
		REQUIRE(compile("ÃÈÊ?"_rng, program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(validate(program) == true);

		CHECK(run(vm, "ÃÈ"_rng, program) == true);
		CHECK(run(vm, "ÃÈÊ"_rng, program) == true);
		CHECK(run(vm, "ÃÈÊÊÊÊ"_rng, program) == true);
		CHECK(run(vm, "ÃÊ"_rng, program) == false);
	}

	SECTION("Case 03")
	{
		Tape program;
		REQUIRE(compile("(abc|def)?"_rng, program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(validate(program) == true);

		CHECK(run(vm, ""_rng, program) == true);
		CHECK(run(vm, "abc"_rng, program) == true);
		CHECK(run(vm, "def"_rng, program) == true);
		CHECK(run(vm, "abcdefabc"_rng, program) == true);
		CHECK(run(vm, "abcdefab"_rng, program) == true);
		CHECK(run(vm, "abdcefab"_rng, program) == true);
	}

	SECTION("Case 04")
	{
		Tape program;
		REQUIRE(compile("abc\\?"_rng, program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(validate(program) == true);

		CHECK(run(vm, "abc"_rng, program) == false);
		CHECK(run(vm, ""_rng, program) == false);
		CHECK(run(vm, "abc?"_rng, program) == true);
	}

	SECTION("Case 05")
	{
		Tape program;
		REQUIRE(compile("\\??"_rng, program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(validate(program) == true);

		CHECK(run(vm, ""_rng, program) == true);
		CHECK(run(vm, "?"_rng, program) == true);
		CHECK(run(vm, "????"_rng, program) == true);
		CHECK(run(vm, "+++++++"_rng, program) == true);
	}

	SECTION("Case 06")
	{
		Tape program;
		REQUIRE(compile("abc??"_rng, program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(validate(program) == true);

		Match_Result res;

		CHECK(run(vm, "ab"_rng, program, res) == true);
		CHECK(run(vm, "abc"_rng, program, res) == true);
		CHECK(strcmp(res.str, "ab"_rng) == 0);
	}
}

TEST_CASE("set test", "[set]")
{
	SECTION("Case 01")
	{
		Tape program;
		REQUIRE(compile("[abc]f"_rng, program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(validate(program) == true);

		CHECK(run(vm, "f"_rng, program) == false);
		CHECK(run(vm, "af"_rng, program) == true);
		CHECK(run(vm, "bf"_rng, program) == true);
		CHECK(run(vm, "cf"_rng, program) == true);
		CHECK(run(vm, "zf"_rng, program) == false);
		CHECK(run(vm, "[f"_rng, program) == false);
	}

	SECTION("Case 02")
	{
		Tape program;
		REQUIRE(compile("[^abc]f"_rng, program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(validate(program) == true);

		CHECK(run(vm, "f"_rng, program) == false);
		CHECK(run(vm, "af"_rng, program) == false);
		CHECK(run(vm, "bf"_rng, program) == false);
		CHECK(run(vm, "cf"_rng, program) == false);
		CHECK(run(vm, "zf"_rng, program) == true);
		CHECK(run(vm, "Ãf"_rng, program) == true);
	}

	SECTION("Case 03")
	{
		Tape program;
		REQUIRE(compile("[a-c]f"_rng, program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(validate(program) == true);

		CHECK(run(vm, "f"_rng, program) == false);
		CHECK(run(vm, "af"_rng, program) == true);
		CHECK(run(vm, "bf"_rng, program) == true);
		CHECK(run(vm, "cf"_rng, program) == true);
		CHECK(run(vm, "zf"_rng, program) == false);
		CHECK(run(vm, "[f"_rng, program) == false);
	}

	SECTION("Case 04")
	{
		Tape program;
		REQUIRE(compile("[a-a]f"_rng, program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(validate(program) == true);

		CHECK(run(vm, "f"_rng, program) == false);
		CHECK(run(vm, "af"_rng, program) == true);
		CHECK(run(vm, "bf"_rng, program) == false);
		CHECK(run(vm, "cf"_rng, program) == false);
		CHECK(run(vm, "zf"_rng, program) == false);
		CHECK(run(vm, "[f"_rng, program) == false);
	}

	SECTION("Case 05")
	{
		Tape program;
		REQUIRE(compile("[^a-c]f"_rng, program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(validate(program) == true);

		CHECK(run(vm, "f"_rng, program) == false);
		CHECK(run(vm, "af"_rng, program) == false);
		CHECK(run(vm, "bf"_rng, program) == false);
		CHECK(run(vm, "cf"_rng, program) == false);
		CHECK(run(vm, "zf"_rng, program) == true);
		CHECK(run(vm, "Ãf"_rng, program) == true);
	}

	SECTION("Case 06")
	{
		Tape program;
		REQUIRE(compile("[^a-a]f"_rng, program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(validate(program) == true);

		CHECK(run(vm, "f"_rng, program) == false);
		CHECK(run(vm, "af"_rng, program) == false);
		CHECK(run(vm, "bf"_rng, program) == true);
		CHECK(run(vm, "cf"_rng, program) == true);
		CHECK(run(vm, "zf"_rng, program) == true);
		CHECK(run(vm, "Ãf"_rng, program) == true);
	}

	SECTION("Case 07")
	{
		Tape program;
		REQUIRE(compile("[a\\-a]f"_rng, program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(validate(program) == true);

		CHECK(run(vm, "f"_rng, program) == false);
		CHECK(run(vm, "af"_rng, program) == true);
		CHECK(run(vm, "bf"_rng, program) == false);
		CHECK(run(vm, "cf"_rng, program) == false);
		CHECK(run(vm, "-f"_rng, program) == true);
		CHECK(run(vm, "Ãf"_rng, program) == false);
	}

	SECTION("Case 08")
	{
		Tape program;
		REQUIRE(compile("[\\\\]f"_rng, program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(validate(program) == true);

		CHECK(run(vm, "\\f"_rng, program) == true);
		CHECK(run(vm, "af"_rng, program) == false);
	}

	SECTION("Case 09")
	{
		Tape program;
		REQUIRE(compile("[a]f"_rng, program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(validate(program) == true);

		CHECK(run(vm, "\\f"_rng, program) == false);
		CHECK(run(vm, "af"_rng, program) == true);
	}

	SECTION("Case 10")
	{
		Tape program;
		REQUIRE(compile("[a\\]]f"_rng, program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(validate(program) == true);

		CHECK(run(vm, "\\f"_rng, program) == false);
		CHECK(run(vm, "af"_rng, program) == true);
		CHECK(run(vm, "]f"_rng, program) == true);
	}

	SECTION("Case 11")
	{
		Tape program;
		REQUIRE(compile("[a\\]]*f"_rng, program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(validate(program) == true);

		CHECK(run(vm, "\\f"_rng, program) == false);
		CHECK(run(vm, "af"_rng, program) == true);
		CHECK(run(vm, "f"_rng, program) == true);
		CHECK(run(vm, "]f"_rng, program) == true);
		CHECK(run(vm, "a]]]]af"_rng, program) == true);	
	}
}

TEST_CASE("any test", "[any]")
{
	SECTION("Case 01")
	{
		Tape program;
		REQUIRE(compile("ab."_rng, program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(validate(program) == true);

		CHECK(run(vm, "abc"_rng, program) == true);
		CHECK(run(vm, "acd"_rng, program) == false);
		CHECK(run(vm, "abccc"_rng, program) == true);
	}

	SECTION("Case 02")
	{
		Tape program;
		REQUIRE(compile("ãÕØÝ."_rng, program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(validate(program) == true);

		CHECK(run(vm, "ãÕØÝì"_rng, program) == true);
		CHECK(run(vm, "ãÕÝì"_rng, program) == false);
		CHECK(run(vm, "ãÕØÝì ÓÚÏ"_rng, program) == true);
	}

	SECTION("Case 03")
	{
		Tape program;
		REQUIRE(compile("a(.)c"_rng, program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(validate(program) == true);

		CHECK(run(vm, "abc"_rng, program) == true);
		CHECK(run(vm, "abd"_rng, program) == false);
		CHECK(run(vm, "abccc"_rng, program) == true);
	}

	SECTION("Case 04")
	{
		Tape program;
		REQUIRE(compile("((a)((.)(c)))"_rng, program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(validate(program) == true);

		CHECK(run(vm, "abc"_rng, program) == true);
		CHECK(run(vm, "abd"_rng, program) == false);
		CHECK(run(vm, "abccc"_rng, program) == true);
	}

	SECTION("Case 05")
	{
		Tape program;
		REQUIRE(compile("(\\(a\\)((b)(.)*))"_rng, program) == RGX_ERROR::OK);
		VM_State vm;

		CHECK(validate(program) == true);

		CHECK(run(vm, "abc"_rng, program) == false);
		CHECK(run(vm, "abd"_rng, program) == false);
		CHECK(run(vm, "abccc"_rng, program) == false);
		CHECK(run(vm, "(a)bccc"_rng, program) == true);
		CHECK(run(vm, "(abccc"_rng, program) == false);
		CHECK(run(vm, "(a))bccc"_rng, program) == false);
		CHECK(run(vm, "(a)bc"_rng, program) == true);
		CHECK(run(vm, "(a)b"_rng, program) == true);
		CHECK(run(vm, "(a)ccc"_rng, program) == false);
	}
}

// TEST_CASE("regex search"_rng, "[search]")
// {
// 	arena_t rgx_memory(MEGABYTES(25));

// 	SECTION("Case 01")
// 	{
// 		Tape program;
// 		REQUIRE(compile("fn[ \t\r\n]*[a-zA-Z_][a-zA-Z0-9_]*\\(.*?\\)"_rng, program) == RGX_ERROR::OK);
// 		VM_State vm;

// 		string str = "struct class int float fn foo(); fn koko();";
// 		Match_Result res;

// 		CHECK(search(str, program, res) == true);
// 		CHECK(search(str.view(res.str_view.end(), str.end()), program, res) == true);
// 	}
// }