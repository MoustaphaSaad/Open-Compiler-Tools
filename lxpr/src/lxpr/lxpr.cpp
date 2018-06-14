#include "lxpr/lxpr.h"
#include <cpprelude/File.h>
#include <cpprelude/Buffered_Stream.h>
#include <cpprelude/Panic.h>
#include <ctype.h>

using namespace cppr;
using namespace rgx;
using namespace oct;

namespace lxpr
{
	//lexer part
	bool
	meta_lexer_create(Proto_Lexer& lexer)
	{
		if(!lexer.push_token("[ \t\n\r]+"_rng, Token_Action
		{
			nullptr,
			META_TOKENS::WHITESPACE,
			true
		}))
			return false;

		if(!lexer.push_token("//[^\n]*"_rng, Token_Action
		{
			nullptr,
			META_TOKENS::COMMENT,
			true
		}))
			return false;

		if(!lexer.push_token("="_rng, Token_Action
		{
			nullptr,
			META_TOKENS::ASSIGN,
			false
		}))
			return false;

		if(!lexer.push_token(";"_rng, Token_Action
		{
			nullptr,
			META_TOKENS::SEMICOLON,
			false
		}))
			return false;

		if(!lexer.push_token("::"_rng, Token_Action
		{
			nullptr,
			META_TOKENS::SCOPE,
			false
		}))
			return false;

		if(!lexer.push_token(":"_rng, Token_Action
		{
			nullptr,
			META_TOKENS::COLON,
			false
		}))
			return false;

		if(!lexer.push_token(">"_rng, Token_Action
		{
			nullptr,
			META_TOKENS::ARROW,
			false
		}))
			return false;

		if(!lexer.push_token("\\|"_rng, Token_Action
		{
			nullptr,
			META_TOKENS::OR,
			false
		}))
			return false;

		if(!lexer.push_token("\\*"_rng, Token_Action
		{
			nullptr,
			META_TOKENS::STAR,
			false
		}))
			return false;

		if(!lexer.push_token("\\+"_rng, Token_Action
		{
			nullptr,
			META_TOKENS::PLUS,
			false
		}))
			return false;

		if(!lexer.push_token("\\?"_rng, Token_Action
		{
			nullptr,
			META_TOKENS::OPTIONAL,
			false
		}))
			return false;

		if(!lexer.push_token("token"_rng, Token_Action
		{
			nullptr,
			META_TOKENS::KEYWORD_TOKEN,
			false
		}))
			return false;

		if(!lexer.push_token("ignore"_rng, Token_Action
		{
			nullptr,
			META_TOKENS::KEYWORD_IGNORE,
			false
		}))
			return false;

		if(!lexer.push_token("rule"_rng, Token_Action
		{
			nullptr,
			META_TOKENS::KEYWORD_RULE,
			false
		}))
			return false;

		if(!lexer.push_token("start"_rng, Token_Action
		{
			nullptr,
			META_TOKENS::KEYWORD_START,
			false
		}))
			return false;

		if(!lexer.push_token("lexer_name"_rng, Token_Action
		{
			nullptr,
			META_TOKENS::KEYWORD_LEXER_NAME,
			false
		}))
			return false;

		if(!lexer.push_token("[a-zA-Z_][a-zA-Z0-9_]*"_rng, Token_Action
		{
			nullptr,
			META_TOKENS::ID,
			false
		}))
			return false;

		if(!lexer.push_token("\"(\\\\.|[^\"])*\""_rng, Token_Action
		{
			[](const String_Range& match, String_Range& stream, Token& tkn)
			{
				tkn.str.bytes = tkn.str.bytes.range(1, tkn.str.bytes.size - 1);
				stream.bytes.ptr += match.bytes.size;
				return true;
			},
			META_TOKENS::STRING,
			false
		}))
			return false;

		if(!lexer.push_token("{{.*}}"_rng, Token_Action
		{
			nullptr,
			META_TOKENS::EMBEDDED_CODE,
			false
		}))
			return false;

		// if(!lexer.push_token("\""_rng, Token_Action
		// {
		// 	[](const String_Range& match, String_Range& stream, Token& tkn)
		// 	{
		// 		bool ignore = false;
		// 		auto str = stream;
		// 		for(str.pop_front(); !str.empty(); str.pop_front())
		// 		{
		// 			Rune c = str.front();
		// 			if(c == '\\')
		// 			{
		// 				ignore = true;
		// 			}
		// 			else if(c == '"' && !ignore)
		// 			{
		// 				tkn.str = stream.range(match.end(), str.begin());
		// 				//pop the last '"'
		// 				str.pop_front();
		// 				stream = str;
		// 				return true;
		// 			}
		// 			else
		// 			{
		// 				ignore = false;
		// 			}
		// 		}
		// 		return false;
		// 	},
		// 	META_TOKENS::STRING,
		// 	false
		// }))
		// 	return false;

		return true;
	}

	//parser part
	String_Range
	Lxpr_Program::str_intern(const cppr::String_Range& str)
	{
		return str_intern_table.insert(str)->all();
	}

	Token
	Lxpr_Program::token_intern(const Token& token)
	{
		Token result = token;
		result.str = str_intern(token.str);
		return result;
	}

	Lxpr_Stmt&
	Lxpr_Program::stmt_create(Lxpr_Stmt::TYPE type)
	{
		stmts.insert_back(Lxpr_Stmt::make(type));
		auto& def = stmts.back();
		def.index = stmts.count() - 1;
		return def;
	}

	cppr::String_Range
	Lxpr_Program::token_rgx(cppr::usize index)
	{
		auto it = token_rgx_cache.lookup(index);
		if(it != token_rgx_cache.end())
			return it->value;

		cppr::String str = "(";
		usize first = stmts[index].token_decl.first_node;
		for(usize i = first;
			i != -1;
			i = stmts[i].token_node.next)
		{
			auto& node = stmts[i].token_node;

			switch(node.token.type)
			{
			case META_TOKENS::STRING:
			case META_TOKENS::PLUS:
			case META_TOKENS::STAR:
			case META_TOKENS::OR:
				if(node.token.type == META_TOKENS::OR)
					str.concat(")|(");
				else
					str.concat(node.token.str);
				break;

			case META_TOKENS::ID:
				str.concat(token_rgx(symbol_table[node.token.str]));
				break;
			default:
				break;
			}
		}
		str.concat(")");
		auto result = str_intern_table.insert(str)->all();
		token_rgx_cache.insert(index, result);
		return result;
	}

	#define PARSER_ERROR(MSG, ...) {\
		printf_err(MSG, ## __VA_ARGS__);\
		return false;\
	}

	inline static bool
	_parse_lexer_name(Proto_Parser& parser, String_Range& stream, Lxpr_Program& out_program)
	{
		//skip the lexer_name keyword and the scope token
		if(parser.token_skip(stream, 2) != 2) return false;

		Token token;
		if(!parser.token_expect(stream, META_TOKENS::ID, token)) return false;
		out_program.lexer_name = out_program.str_intern(token.str);
		return true;
	}

	inline static bool
	_parse_proc_code(Proto_Parser& parser, String_Range& stream, Lxpr_Program& out_program)
	{
		Lxpr_Stmt& stmt = out_program.stmt_create(Lxpr_Stmt::PROC);
		
		//parse the ID of the token
		Token token;
		if(!parser.token_expect(stream, META_TOKENS::ID, token)) return false;
		stmt.proc_decl.id = out_program.token_intern(token);

		if(!parser.token_expect(stream, META_TOKENS::SCOPE)) return false;

		if(!parser.token_expect(stream, META_TOKENS::EMBEDDED_CODE, token)) return false;

		stmt.proc_decl.code = out_program.token_intern(token);
		return true;
	}

	inline static bool
	_parse_token_def(Proto_Parser& parser, String_Range& stream, Lxpr_Program& out_program)
	{
		Lxpr_Stmt& stmt = out_program.stmt_create(Lxpr_Stmt::TOKEN_DECL);
		usize stmt_index = stmt.index;
		stmt.token_decl.first_node = -1;

		//parse the ID of the token
		Token token;
		if(!parser.token_expect(stream, META_TOKENS::ID, token)) return false;
		stmt.token_decl.id = out_program.token_intern(token);

		//skip the :
		if(!parser.token_expect(stream, META_TOKENS::COLON)) return false;

		//peek the next token
		if(!parser.token_peek(stream, token)) return false;

		//handle the ignore keyword
		stmt.token_decl.ignore = parser.token_expect(stream, META_TOKENS::KEYWORD_IGNORE);

		//expect the token keyword
		if(!parser.token_expect(stream, META_TOKENS::KEYWORD_TOKEN)) return false;
		
		//expect assignment
		if(!parser.token_expect(stream, META_TOKENS::ASSIGN)) return false;

		//parse the token definition body
		if(!parser.token_peek(stream, token)) return false;

		//parse the token definition body
		usize last_token = -1;
		while(token.type != META_TOKENS::SEMICOLON &&
			  token.type != META_TOKENS::ARROW)
		{
			//link tokens as if they were in a linked list style
			if(last_token == -1)
				out_program.stmts[stmt_index].token_decl.first_node = out_program.stmts.count();
			else
				out_program.stmts[last_token].token_node.next = out_program.stmts.count();

			//create a token node
			Lxpr_Stmt& node = out_program.stmt_create(Lxpr_Stmt::TOKEN_NODE);
			node.token_node.next = node.token_node.prev = -1;
			
			//link the node to the prev node
			node.token_node.token = out_program.token_intern(token);
			node.token_node.prev = last_token;

			last_token = out_program.stmts.count() - 1;

			//skip the current token
			if(parser.token_skip(stream, 1) != 1) return false;

			//peek into the next token
			if(!parser.token_peek(stream, token)) return false;
		}

		//check if there's an action
		if(token.type == META_TOKENS::ARROW)
		{
			if(parser.token_skip(stream, 1) != 1) return false;

			if(!parser.token_expect(stream, token)) return false;

			out_program.stmts[stmt_index].token_decl.action = out_program.token_intern(token);
		}

		out_program.symbol_table.insert(out_program.stmts[stmt_index].token_decl.id.str, stmt_index);
		return true;
	}

	inline static bool
	_parse_rule_def(Proto_Parser& parser, String_Range& stream, Lxpr_Program& out_program)
	{
		return false;
	}

	inline static bool
	_parse_stmt(Proto_Parser& parser, String_Range& stream, Lxpr_Program& out_program)
	{
		Token token;
		auto context = parser.new_peek_context();

		if(!parser.token_peek(stream, token, context)) return false;

		//if it's a lexer name then parse a lexer name
		if(token.type == META_TOKENS::KEYWORD_LEXER_NAME)
			return _parse_lexer_name(parser, stream, out_program);
		
		//then it must be an id
		if(token.type != META_TOKENS::ID) return false;

		if(!parser.token_peek(stream, token, context)) return false;

		//then it's an ambedded code
		if(token.type == META_TOKENS::SCOPE)
			return _parse_proc_code(parser, stream, out_program);

		//it must be a colon
		if(token.type != META_TOKENS::COLON) return false;

		if(!parser.token_peek(stream, token, context)) return false;

		//it's a token definition
		if(token.type == META_TOKENS::KEYWORD_IGNORE ||
		   token.type == META_TOKENS::KEYWORD_TOKEN)
		   return _parse_token_def(parser, stream, out_program);

		//it's a rule definition
		if(token.type == META_TOKENS::KEYWORD_RULE)
			return _parse_rule_def(parser, stream, out_program);

		//it's unrecognized stmt
		return false;
	}

	bool
	meta_parse(Proto_Parser& parser,
			   String_Range& stream,
			   Lxpr_Program& out_program)
	{
		out_program.lexer_name = out_program.str_intern("default_lexer"_rng);
		while(_parse_stmt(parser, stream, out_program))
			if(!parser.token_expect(stream, META_TOKENS::SEMICOLON))
				return false;
		return true;
	}

	//code generation part
	#define LINE(level, ...) for(usize i = 0; i < level; ++i)\
							 vprints(io, "\t");\
							 vprints(io, ##__VA_ARGS__, "\n")

	#define LINEF(level, format, ...) for(usize i = 0; i < level; ++i)\
							 vprints(io, "\t");\
							 vprintf(io, format"\n", ##__VA_ARGS__)

	String
	_cap_token(const String_Range& name)
	{
		String result = name;
		for(auto& c: result._bytes.all())
			c = toupper(c);
		return result;
	}

	void
	_meta_header_gen(IO_Trait* io, Lxpr_Program& program)
	{
		LINE(0, "#pragma once");
		LINE(0, "#include <cpprelude/defines.h>");
		LINE(0, "#include <oct/Proto_Lexer.h>");
		LINE(0, "#include <oct/Proto_Parser.h>");

		LINE(0, "namespace ", program.lexer_name);
		LINE(0, "{");
		LINE(1, "enum TOKENS: cppr::u32");
		LINE(1, "{");
		LINE(2, "TOKEN_NONE,");
		for(const auto& def: program.stmts)
		{
			if(def.type == Lxpr_Stmt::TOKEN_DECL)
			{
				LINE(2, _cap_token(def.token_decl.id.str), ",");
			}
		}
		LINE(1, "};");
		LINE(1);

		LINE(1, "bool lexer_create(oct::Proto_Lexer& lexer);");
		LINE(0, "}");
	}

	void
	_meta_source_gen(IO_Trait* io, Lxpr_Program& program)
	{
		LINEF(0, "#include \"{}.h\"", program.lexer_name);
		LINE(0, "using namespace cppr;");
		LINE(0, "using namespace rgx;");
		LINE(0, "using namespace oct;");

		LINE(0, "namespace ", program.lexer_name);
		LINE(0, "{");
		LINE(1, "bool lexer_create(Proto_Lexer& lexer)");
		LINE(1, "{");
		for(const auto& def: program.stmts)
		{
			if(def.type == Lxpr_Stmt::TOKEN_DECL)
			{
				LINEF(2, "if(!lexer.push_token(\"{}\"_rng, Token_Action{{ {}, TOKENS::{}, {} })) return false;",
					  program.token_rgx(def.index), "nullptr"_rng, _cap_token(def.token_decl.id.str), def.token_decl.ignore ? "true": "false");
			}
		}
		LINE(2, "return true;");
		LINE(1, "}");
		LINE(0, "}");
	}

	inline static String
	gen_header_name(const cppr::String_Range& out_folder, Lxpr_Program& program)
	{
		String header_name = out_folder;
		header_name.concat(program.lexer_name);
		header_name.concat(".h");
		return header_name;
	}

	inline static String
	gen_source_name(const cppr::String_Range& out_folder, Lxpr_Program& program)
	{
		String source_name = out_folder;
		source_name.concat(program.lexer_name);
		source_name.concat(".cpp");
		return source_name;
	}

	void
	meta_gen(const cppr::String_Range& out_folder, Lxpr_Program& program)
	{
		File header_file = unwrap(File::open(gen_header_name(out_folder, program).all()), OS_ERROR::OK, "unable to open the header file"_rng);
		File source_file = unwrap(File::open(gen_source_name(out_folder, program).all()), OS_ERROR::OK, "unable to open the source file"_rng);

		Buf_Writer writer(header_file);
		//generate header file
		_meta_header_gen(writer, program);
		writer.flush();
		writer._io = source_file;
		_meta_source_gen(writer, program);
	}
}