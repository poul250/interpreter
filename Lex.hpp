#pragma once
#include <iostream>
#include <vector>
#include <string>

using namespace std;

namespace Pawka {
enum LexType {
	LEX_NULL, LEX_ID, LEX_NUM, LEX_STRING,
	LEX_OP_BRACE = '{', LEX_CL_BRACE  = '}', LEX_ASSIGN = '=',
	LEX_OP_ROUND = '(', LEX_CL_ROUND  = ')',
	LEX_COMMA    = ',', LEX_SEMICOLON = ';',
	LEX_PLUS     = '+', LEX_MINUS     = '-',
	LEX_MUL      = '*', LEX_DIV       = '/',
	LEX_LESS     = '<', LEX_GREATER   = '>',
	LEX_EQ       = '=' + 300, LEX_NE  = '!' + 300,
	LEX_LE       = '<' + 300, LEX_GE  = '>' + 300,
	LEX_PROGRAM = 600, LEX_READ, LEX_WRITE, LEX_IF, LEX_ELSE, LEX_NOT, LEX_AND, LEX_OR,
	LEX_INT, LEX_STR
};

struct Lex {
	LexType type;
	string str;

	Lex(const LexType, const char*);
	Lex(const LexType, string);
	Lex();

	friend ostream& operator<<(ostream& stream, Lex lex);
};

class LexAnalizer {
public:
	LexAnalizer(istream& stream);
	~LexAnalizer();

	bool moveNext();
	const Lex& current() const { 
		return lex; 
	}

private:
	typedef bool (LexAnalizer::*State) (int c);

	bool stateA (int c);
	bool stateAA(int c);
	bool stateB (int c);
	bool stateC (int c);
	bool stateD (int c);
	bool stateDD(int c);


	void makeLex(LexType);

	string buf;
	istream& stream;
	State state;
	Lex lex;
	vector<string> TW;
};
}//namespace
