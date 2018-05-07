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

class Inerpretator {
public:
	Inerpretator(istream& stream)
		: la(stream)
		, neadRead(true) {	}

	void Inerpretate();
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
	LexAnalizer la;
};

void Inerpretator::Expression(){
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

void Inerpretator::Pr() {
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

void Inerpretator::SumOp() {
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

void Inerpretator::PrOp() {
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

void Inerpretator::Atom() {
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
		cout << "Line " << la.getLine() << ": " << str << endl;
	}
	/*
	Inerpretator sa(cin);

	try {
		sa.Inerpretate();
	}
	catch (Lex l) {
		cout << l << endl;
	}
	catch (const char*) {}
	*/
	return 0;
}

bool Inerpretator::getLex(LexType type) {
	static bool exist = true;
	if (neadRead)
		exist = la.moveNext();
	else
		neadRead = true;

	if (type == LEX_NULL) {
		if (!exist)
			throw Lex();
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

void Inerpretator::ungetLex() {
	neadRead = false;
}

void Inerpretator::Inerpretate() {
	getLex(LEX_PROGRAM);
	getLex(LEX_OP_BRACE);

	Descriptions();
	Operators();

	getLex(LEX_CL_BRACE);
}

void Inerpretator::Descriptions() {
	bool running = true;

	while (running)	{
		getLex();
		switch (lex.type) {
			case LEX_INT: case LEX_STR:
				if (lex.type == LEX_INT)
					ReadId(LEX_NUM);
				else
					ReadId(LEX_STRING);
				break;

			default:
				ungetLex();
				running = false;
				break;
		}
	}
}

void Inerpretator::ReadId(const LexType type) {
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
			break;

		default:
			throw lex;
	}
}

void Inerpretator::ReadValue(const LexType type) {
	if (type == LEX_NUM) {
		getLex();
		if (lex.type != LEX_PLUS && lex.type != LEX_MINUS)
			ungetLex();
	}
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

void Inerpretator::Operators() {
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

void Inerpretator::Operator() {
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

void Inerpretator::ReadFunc() {
	getLex(LEX_OP_ROUND);
	getLex(LEX_ID);
	getLex(LEX_CL_ROUND);
	getLex(LEX_SEMICOLON);
}

void Inerpretator::WriteFunc() {
	getLex(LEX_OP_ROUND);
	do {
		Expression();
		getLex();
	} while (lex.type == LEX_COMMA);
	ungetLex();
	getLex(LEX_CL_ROUND);
	getLex(LEX_SEMICOLON);
}

void Inerpretator::IfBlock() {
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
