#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <map>

using namespace std;

namespace Pawka {
enum LexType {
	LEX_NULL, LEX_ID, LEX_NUM, LEX_STRING,
	LEX_OP_BRACE /* {  */, LEX_CL_BRACE  /* }  */, LEX_ASSIGN/* = */,
	LEX_OP_ROUND /* (  */, LEX_CL_ROUND  /* )  */,
	LEX_COMMA    /* ,  */, LEX_SEMICOLON /* ;  */,
	LEX_PLUS     /* +  */, LEX_MINUS     /* -  */,
	LEX_MUL      /* *  */, LEX_DIV       /* /  */,
	LEX_LESS     /* <  */, LEX_GREATER   /* >  */,
	LEX_EQ       /* == */, LEX_NE        /* != */,
	LEX_LE       /* <= */, LEX_GE        /* >= */,
	LEX_PROGRAM, LEX_READ, LEX_WRITE, LEX_IF, LEX_ELSE,
	LEX_NOT, LEX_AND, LEX_OR, LEX_INT, LEX_STR
};

struct Lex {
	LexType type;
	string str;

	Lex(const LexType srcType = LEX_NULL, string srcStr = string());

	friend ostream& operator<<(ostream& stream, Lex lex);
};

class LexAnalizer {
public:
	LexAnalizer(istream& stream);
	~LexAnalizer();

	bool moveNext();
	const Lex& current() const { return lex; }
	int getLine() { return line; }

private:
	typedef bool (LexAnalizer::*State) (int c);

	bool FirstSym (int c);
	bool ComplexOperation(int c);
	bool ReadId (int c);
	bool ReadNum (int c);
	bool ReadString (int c);
	bool Shielding(int c);
	bool Comment(int c);
	bool BigComment(int c);

	void makeLex(LexType type) { lex = Lex(type, buf); };

	string buf;
	istream& stream;
	State state;
	Lex lex;
	map<string, LexType> TW;
	int line;
};
}//namespace
