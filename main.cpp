#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <stack>
#include "Lex.hpp"

using namespace std;
using namespace Pawka;

class PolizOp {
protected:
	virtual void execute() = 0;
};

class SyntaxAnalizer {
public:
	SyntaxAnalizer(istream& stream) :
			la(stream),
			neadRead(true) {	}

	void Start();
private:
	//Program states
	void Descriptions();
	void ReadId(const LexType);
	void ReadValue(const LexType);
	void Operators();
	void Operator();
	void IfBlock();
	void ReadFunc();
	void WriteFunc();

	//Expression states
	void Expression();
	void Pr();
	void SumOp();
	void PrOp();
	void AssignOp();
	void AndOp();
	void OrOp();
	void NotOp();
	void CmpOp();
	void Atom();

	bool getLex(LexType type = LEX_NULL);
	void ungetLex();

	bool neadRead;
	Lex cur;
	stack<Lex> expr;
	LexAnalizer la;
};

void SyntaxAnalizer::Expression(){
	bool running = true;
	Pr();

	getLex();
	if (cur.type != LEX_PLUS && cur.type != LEX_MINUS)
		running = false;
	ungetLex();

	while (running) {
		SumOp();
		Pr();
	}
}

void SyntaxAnalizer::Pr() {
	bool running = true;
	Atom();

	getLex();
	if (cur.type != LEX_MUL && cur.type != LEX_DIV)
		running = false;
	ungetLex();

	while (running) {
		PrOp();
		Atom();
	}
}

void SyntaxAnalizer::SumOp() {
	getLex();
}

void SyntaxAnalizer::PrOp() {
	getLex();
}

void SyntaxAnalizer::Atom() {
	getLex();
	if (cur.type == LEX_OP_ROUND) {
		Expression();
		getLex(LEX_OP_ROUND);
	}
}
int main()
{
	/*
	LexAnalizer la(cin);
	try{
		while (la.moveNext()) {
			cout << la.current() << endl;
		}
	}
	catch(const char *str) {
		cout << str << endl;
	}*/
	SyntaxAnalizer sa(cin);

	try {
		sa.Start();
	}
	catch (Lex l) {
		cout << l << endl;
	}
	return 0;
}


void SyntaxAnalizer::Start() {
	getLex(LEX_PROGRAM);
	getLex(LEX_OP_BRACE);

	Descriptions();
	Operators();

	getLex(LEX_CL_BRACE);
}

bool SyntaxAnalizer::getLex(LexType type) {
	static bool exist = true;
	if (neadRead)
		exist = la.moveNext();
	else
		neadRead = true;

	if (type == LEX_NULL) {
		if (!exist) {/*
			cur = Lex();
			return false;*/
			throw Lex();
		}
		cur = la.current();
	}
	else {
		if (!exist)
			throw "Unexpected end of file";
		cur = la.current();
		if (cur.type != type)
			throw cur;
	}
	return true;
}

void SyntaxAnalizer::ungetLex() {
	neadRead = false;
}


void SyntaxAnalizer::Descriptions() {
	bool running = true;

	while (running)	{
		getLex();
		switch (cur.type) {
			case LEX_NULL:
				throw "Unexpected end of file";
				break;

			case LEX_INT: case LEX_STR:
				LexType t;
				if (cur.type == LEX_INT)
					t = LEX_NUM;
				else
					t = LEX_STRING;

				ReadId(t);
				break;
			default:
				ungetLex();
				running = false;
				break;
		}
	}
}

void SyntaxAnalizer::ReadId(const LexType type) {
	getLex(LEX_ID);
	getLex();
	switch(cur.type) {
		case LEX_COMMA:
			ReadId(type);
			break;

		case LEX_ASSIGN:
			ReadValue(type);
			break;

		case LEX_SEMICOLON:
			Descriptions();
			break;

		default:
			throw cur;
	}
}

void SyntaxAnalizer::ReadValue(const LexType type) {
	getLex(type);
	getLex();

	switch (cur.type) {
		case LEX_COMMA:
			ReadId(type);
			break;

		case LEX_SEMICOLON:
			break;

		default:
			throw cur;
	}
}

void SyntaxAnalizer::Operators() {
	bool running = true;
	while (running) {
		getLex();
		switch(cur.type) {
			case LEX_OP_BRACE:
				Operators();
				getLex(LEX_CL_BRACE);
				break;

			case LEX_CL_BRACE:
				ungetLex();
				running = false;
				break;

			default:
				ungetLex();
				Operator();
				break;
		}
	}
}

void SyntaxAnalizer::Operator() {
	getLex();
	switch(cur.type) {
		case LEX_READ:
			ReadFunc();
			break;
		case LEX_WRITE:
			WriteFunc();
			break;
		case LEX_ID:
			break;
		default:
			throw cur;
			break;
	}
}

void SyntaxAnalizer::ReadFunc() {
	getLex(LEX_OP_ROUND);
	getLex(LEX_ID);
	getLex(LEX_CL_ROUND);
	getLex(LEX_SEMICOLON);
}

void SyntaxAnalizer::WriteFunc() {
	getLex(LEX_OP_ROUND);
	do {
		Expression();
		getLex();
	} while (cur.type == LEX_COMMA);
	ungetLex();
	getLex(LEX_CL_ROUND);
	getLex(LEX_SEMICOLON);
}

void SyntaxAnalizer::IfBlock() {
	getLex(LEX_OP_ROUND);
	Expression();
	getLex(LEX_CL_ROUND);

	getLex();
	if (cur.type == LEX_OP_BRACE) {
		Operators();
		getLex(LEX_CL_BRACE);
	}
	else {
		ungetLex();
		Operator();
	}

	getLex();
	if(cur.type == LEX_ELSE) {
		getLex();
		if (cur.type == LEX_OP_BRACE) {
			Operators();
			getLex(LEX_CL_BRACE);
		}
		else {
			ungetLex();
			Operator();
		}
	}
	else {
		ungetLex();
	}
}
