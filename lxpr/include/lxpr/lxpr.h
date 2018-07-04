#pragma once

#include <cpprelude/Ranges.h>
#include <cpprelude/Dynamic_Array.h>
#include <cpprelude/Hash_Map.h>
#include <cpprelude/String.h>
#include <cpprelude/IO_Trait.h>
#include <oct/Proto_Lexer.h>
#include <oct/Proto_Parser.h>

namespace lxpr
{
	//Lexer part
	struct META_TOKENS
	{
		enum
		{
			NONE,					//invalid token type
			WHITESPACE,				//any whitespace
			COMMENT,				//single line comment
			ASSIGN,					//=
			SEMICOLON,				//;
			SCOPE,					//::
			COLON,					//:
			ARROW,					//>
			OR,						//|
			STAR,					//*
			PLUS,					//+
			OPTIONAL,				//?
			KEYWORD_TOKEN,			//token
			KEYWORD_IGNORE,			//ignore
			KEYWORD_RULE,			//rule
			KEYWORD_START,			//start
			KEYWORD_LEXER_NAME,		//lexer_name
			ID,						//any identifier
			STRING,					//anything between double quotes
			EMBEDDED_CODE			//anything between {{}}
		};
	};

	bool
	meta_lexer_create(oct::Proto_Lexer& lexer);
	
	//parser part
	struct Lxpr_Token_Node
	{
		oct::Token token;
		usize next, prev;
	};

	struct Lxpr_Token_Decl
	{
		oct::Token id, action;
		usize first_node;
		bool ignore;
	};

	struct Lxpr_Rule
	{
		usize first;
	};

	struct Lxpr_Proc
	{
		oct::Token id, code;
	};

	struct Lxpr_Stmt
	{
		enum TYPE { NONE, TOKEN_DECL, TOKEN_NODE, RULE, PROC };

		TYPE type;
		usize index;

		union
		{
			Lxpr_Token_Decl token_decl;
			Lxpr_Token_Node token_node;
			Lxpr_Rule rule_def;
			Lxpr_Proc proc_decl;
		};

		inline static Lxpr_Stmt
		make(TYPE provided_type)
		{
			Lxpr_Stmt result;
			memset(&result, 0, sizeof(Lxpr_Stmt));
			result.type = provided_type;
			return result;
		}
	};

	struct Lxpr_Program
	{
		cppr::Hash_Set<cppr::String> str_intern_table;
		cppr::Dynamic_Array<Lxpr_Stmt> stmts;
		cppr::Hash_Map<cppr::String_Range, usize> symbol_table;
		cppr::Hash_Map<usize, cppr::String_Range> token_rgx_cache;
		cppr::String_Range lexer_name;

		cppr::String_Range
		str_intern(const cppr::String_Range& str);

		oct::Token
		token_intern(const oct::Token& token);

		Lxpr_Stmt&
		stmt_create(Lxpr_Stmt::TYPE type);

		cppr::String_Range
		token_rgx(usize index);
	};

	bool
	meta_parse(oct::Proto_Parser& parser,
			   cppr::String_Range& stream,
			   Lxpr_Program& out_program);

	//code generation part
	void
	meta_gen(const cppr::String_Range& out_folder, Lxpr_Program& program);
}