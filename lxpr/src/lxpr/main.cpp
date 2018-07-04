#include <cpprelude/IO.h>
#include <cpprelude/File.h>
#include <cpprelude/String.h>
#include <cpprelude/Panic.h>
#include <cpprelude/Benchmark.h>
#include <cpprelude/Hash_Map.h>
#include <cpprelude/Memory_Stream.h>
#include <cpprelude/Buffered_Stream.h>

#include <oct/Proto_Lexer.h>
#include <oct/Proto_Parser.h>

#include "lxpr/lxpr.h"

#include "json.h"

using namespace cppr;
using namespace oct;
using namespace lxpr;

String
get_file_content(const String_Range& filename)
{
	File file = unwrap(File::open(filename, IO_MODE::READ, OPEN_MODE::OPEN_ONLY), 
					   OS_ERROR::OK, "unable to open the file"_rng);
	
	usize file_size = file.size();
	auto content_mem = os->alloc<byte>(file_size + 1);
	auto content_slice = content_mem.all();
	vreadb(file, content_slice);
	content_mem[file_size] = 0;
	return String(std::move(content_mem));
}

void
debug()
{
	String content = get_file_content("grammars/json.lxpr"_rng);
	while(true)
	{
		auto content_range = content.all();

		Proto_Lexer lexer;
		lxpr::meta_lexer_create(lexer);

		Stopwatch watch;
		usize count = 0;
		watch.start();
		Token tkn;
		while(lexer.token(content_range, tkn))
		{
			//printfmt("token id: {}, token str: <{}>\n", tkn.type, tkn.str);
			count++;
		}
		watch.stop();
		println("token count: ", count);
		println("lexing time: ", watch.milliseconds(), "ms");
	}
}

i32
main(i32 argc, byte **argv)
{
	debug();
	return 0;
	if(argc <= 1)
		panic("Please provide a file name"_rng);

	--argc;
	++argv;

	String_Range filename = make_strrng(*argv);

	--argc;
	++argv;

	String output_folder = "./";
	if(argc > 0)
	{
		output_folder = *argv;
		if(output_folder.all().bytes.back() != '/')
			output_folder.concat("/");

		--argc;
		++argv;
	}

	//create a lexer
	Proto_Lexer lexer;
	meta_lexer_create(lexer);

	//create a parser
	Proto_Parser parser(&lexer);

	//parser output
	Lxpr_Program parsed_program;

	//read the file content
	String content = get_file_content(filename);
	String_Range content_stream = content.all();

	bool result;
	Stopwatch watch;
	watch.start();
	{
		result = meta_parse(parser, content_stream, parsed_program);
	}
	watch.stop();

	printfmt("result: {}\n", result);
	printfmt("parsing took {}ms\n", watch.milliseconds());

	watch.start();
	{
		meta_gen(output_folder.all(), parsed_program);
	}
	watch.stop();
	printfmt("lxpr took {}ms\n", watch.milliseconds());
	//println(output.str_content());
	//println(make_strrng(output._buffer.range(0, output._cursor).convert<const byte>()));
	return 0;
}