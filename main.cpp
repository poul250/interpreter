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

struct da{ };

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
	Lex lex;
	stack<Lex> expr;
	LexAnalizer la;
};

void SyntaxAnalizer::Expression(){
	Pr();

	getLex();
	bool running = lex.type == LEX_PLUS || lex.type == LEX_MINUS;
	ungetLex();

	while (running) {
		SumOp();
		Pr();
		getLex();
		running = lex.type == LEX_PLUS || lex.type == LEX_MINUS;
		ungetLex();
	}
}

void SyntaxAnalizer::Pr() {
	Atom();

	getLex();
	bool running = lex.type == LEX_MUL || lex.type == LEX_DIV;
	ungetLex();

	while (running) {
		PrOp();
		Atom();
		getLex();
		running = lex.type == LEX_MUL || lex.type == LEX_DIV;
		ungetLex();
	}
}

void SyntaxAnalizer::SumOp() {
	getLex();
	switch(lex.type) {
		case LEX_PLUS:
			break;
		case LEX_MINUS:
			break;
		default:
			ungetLex();
			break;
	}
}

void SyntaxAnalizer::PrOp() {
	getLex();
	switch(lex.type) {
		case LEX_MUL:
			break;
		case LEX_DIV:
			break;
		default:
			ungetLex();
			break;
	}
}

void SyntaxAnalizer::Atom() {
	getLex();
	switch(lex.type) {
		case LEX_OP_ROUND:
			Expression();
			getLex(LEX_OP_ROUND);
			break;

		case LEX_ID:
		case LEX_NUM:
		case LEX_STRING:
			break;

		default:
			throw lex;
			break;
	}
}
int main()
{
	
	LexAnalizer la(cin);
	try{
		while (la.moveNext()) {
			cout << la.current() << endl;
		}
	}
	catch(const char *str) {
		cout << str << endl;
	}
	/*
	SyntaxAnalizer sa(cin);

	try {
		sa.Start();
	}
	catch (Lex l) {
		cout << l << endl;
	}*/
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
			lex = Lex();
			return false;*/
			throw Lex();
		}
		lex = la.current();
	}
	else {
		if (!exist)
			throw "Unexpected end of file";
		lex = la.current();
		if (lex.type != type)
			throw lex;
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
		switch (lex.type) {
			case LEX_NULL:
				throw "Unexpected end of file";
				break;

			case LEX_INT: case LEX_STR:
				LexType t;
				if (lex.type == LEX_INT)
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
	switch(lex.type) {
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
			throw lex;
	}
}

void SyntaxAnalizer::ReadValue(const LexType type) {
	getLex(type);
	getLex();

	switch (lex.type) {
		case LEX_COMMA:
			ReadId(type);
			break;

		case LEX_SEMICOLON:
			break;

		default:
			throw lex;
	}
}

void SyntaxAnalizer::Operators() {
	bool running = true;
	while (running) {
		getLex();
		switch(lex.type) {
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
	switch(lex.type) {
		case LEX_READ:
			ReadFunc();
			break;
		case LEX_WRITE:
			WriteFunc();
			break;
		case LEX_ID:
			break;
		default:
			throw lex;
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
	} while (lex.type == LEX_COMMA);
	ungetLex();
	getLex(LEX_CL_ROUND);
	getLex(LEX_SEMICOLON);
}

void SyntaxAnalizer::IfBlock() {
	getLex(LEX_OP_ROUND);
	Expression();
	getLex(LEX_CL_ROUND);

	getLex();
	if (lex.type == LEX_OP_BRACE) {
		Operators();
		getLex(LEX_CL_BRACE);
	}
	else {
		ungetLex();
		Operator();
	}

	getLex();
	if(lex.type == LEX_ELSE) {
		getLex();
		if (lex.type == LEX_OP_BRACE) {
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
