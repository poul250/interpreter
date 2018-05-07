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

class Interpretator {
public:
	Interpretator(istream& stream)
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
	void Assign();
	void AssignOp();
	void Or();
	void OrOp();
	void And();
	void AndOp();
	void Relation();
	void RelationOp();
	void Sum();
	void SumOp();
	void Pr();
	void PrOp();
	void NotOp();
	void Atom();

	bool getLex(LexType type = LEX_NULL);
	void inline checkLex(LexType);
	void ungetLex() { neadRead = false; }

	bool neadRead;
	Lex lex;
	LexAnalizer la;
};

void Interpretator::Expression() {
	Or();
	while (lex.type == LEX_ASSIGN) {
		AssignOp();
		Or();
	}
}

void Interpretator::AssignOp() {
	switch(lex.type) {
		case LEX_ASSIGN: getLex(); break;
	}
}

void Interpretator::Or() {
	And();
	while (lex.type == LEX_OR) {
		OrOp();
		And();
	}
}

void Interpretator::OrOp() {
	getLex();
}

void Interpretator::And() {
	Relation();
	while (lex.type == LEX_AND) {
		AndOp();
		Relation();
	}
}

void Interpretator::AndOp() {
	getLex();
}

void Interpretator::Relation() {
	Sum();
	while (lex.type == LEX_LESS || lex.type == LEX_GREATER || lex.type == LEX_LE ||
		lex.type == LEX_GE || lex.type == LEX_EQ || lex.type == LEX_NE)
	{
		RelationOp();
		Sum();
	}
}

void Interpretator::RelationOp() {
	switch (lex.type) {
		case LEX_LESS:
		case LEX_GREATER:
		case LEX_EQ:
		case LEX_NE:
		case LEX_LE:
		case LEX_GE:
			getLex();
			break;
	}
}

void Interpretator::Sum() {
	Pr();
	while (lex.type == LEX_PLUS || lex.type == LEX_MINUS) {
		SumOp();
		Pr();
	}
}

void Interpretator::SumOp() {
	switch(lex.type) {
		case LEX_PLUS:  getLex(); break;
		case LEX_MINUS: getLex(); break;
	}
}

void Interpretator::Pr() {
	Atom();
	while (lex.type == LEX_MUL || lex.type == LEX_DIV) {
		PrOp();
		Atom();
	}
}

void Interpretator::PrOp() {
	getLex();
}

void Interpretator::Atom() {
	switch(lex.type) {
		case LEX_OP_ROUND:
			Expression();
			getLex(LEX_OP_ROUND);
			break;
		case LEX_ID:
		case LEX_NUM:
		case LEX_STRING:
			getLex();
			break;
		default:
			throw lex;
			break;
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
		cout << "Line " << la.getLine() << ": " << str << endl;
	}
	*/
	Interpretator sa(cin);

	try {
		sa.Inerpretate();
	}
	catch (Lex l) {
		cout << l << endl;
	}
	catch (const char*) {}

	return 0;
}

bool Interpretator::getLex(LexType type) {
	static bool exist = true;
	if (neadRead)
		exist = la.moveNext();
	else
		neadRead = true;

	if (type == LEX_NULL) {
		if (!exist)
			throw Lex();
		lex = la.current();
	} else {
		if (!exist)
			throw "Unexpected end of file";
		lex = la.current();
		if (lex.type != type)
			throw lex;
	}
	return true;
}

void inline Interpretator::checkLex(LexType type) {
	if (lex.type != type)
		throw lex;
}

void Interpretator::Inerpretate() {
	getLex(LEX_PROGRAM);
	getLex(LEX_OP_BRACE);
	getLex();

	Descriptions();
	Operators();

	getLex(LEX_CL_BRACE);
}

void Interpretator::Descriptions() {
	while (lex.type == LEX_INT || lex.type == LEX_STR)	{
		switch (lex.type) {
			case LEX_INT: ReadId(LEX_NUM);    break;
			case LEX_STR: ReadId(LEX_STRING); break;
		}
		getLex();
	}
}

void Interpretator::ReadId(const LexType type) {
	getLex(LEX_ID);
	getLex();
	switch(lex.type) {
		case LEX_COMMA:  ReadId(type);    break;
		case LEX_ASSIGN: ReadValue(type); break;
		case LEX_SEMICOLON:               break;
		default:         throw lex;       break;
	}
}

void Interpretator::ReadValue(const LexType type) {
	if (type == LEX_NUM) {
		getLex();
		if (lex.type != LEX_PLUS && lex.type != LEX_MINUS)
			ungetLex();
	}
	getLex(type);
	getLex();

	switch (lex.type) {
		case LEX_COMMA: ReadId(type); break;
		case LEX_SEMICOLON:           break;
		default:        throw lex;    break;
	}
}

void Interpretator::Operators() {
	while (lex.type != LEX_CL_BRACE) {
		switch(lex.type) {
			case LEX_OP_BRACE:
				Operators();
				checkLex(LEX_CL_BRACE);
				getLex();
				break;
			default:
				Operator();
				break;
		}
	}
}

void Interpretator::Operator() {
	switch(lex.type) {
		case LEX_READ:  ReadFunc();   break;
		case LEX_WRITE: WriteFunc();  break;
		case LEX_IF:    IfBlock();    break;
		case LEX_ID:    Expression(); break;
		default:        throw lex;    break;
	}
}

void Interpretator::ReadFunc() {
	getLex(LEX_OP_ROUND);
	getLex(LEX_ID);
	getLex(LEX_CL_ROUND);
	getLex(LEX_SEMICOLON);
	getLex();
}

void Interpretator::WriteFunc() {
	getLex(LEX_OP_ROUND);
	getLex();
	do {
		Expression();
	} while (lex.type == LEX_COMMA);
	ungetLex();
	getLex(LEX_CL_ROUND);
	getLex(LEX_SEMICOLON);
	getLex();
}

void Interpretator::IfBlock() {
	getLex(LEX_OP_ROUND);
	Expression();
	checkLex(LEX_CL_ROUND);

	getLex();
	if (lex.type == LEX_OP_BRACE) {
		getLex();
		Operators();
		checkLex(LEX_CL_BRACE);
	} else {
		Operator();
	}

	if(lex.type == LEX_ELSE) {
		getLex();
		if (lex.type == LEX_OP_BRACE) {
			getLex();
			Operators();
			checkLex(LEX_CL_BRACE);
			getLex();
		} else {
			Operator();
		}
	}
}
