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

template <class T> struct Ident {
	Ident();

	LexType type;
	bool assign;
	T value;
};

template <class T> Ident<T>::Ident()
	: assign(false)
{

}

class Interpretator {
public:
	Interpretator(istream& stream);

	void Inerpretate();
private:
	//Program states
	void Descriptions();
	void ReadId(const LexType);
	void ReadValue(const LexType);
	void Operators();
	void Operator();
	void IfBlock();
	void ForBlock();
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

	LexType getLex(LexType type = LEX_NULL);
	void inline checkLex(LexType);

	LexAnalizer la;
	bool neadRead;
	Lex lex;

	map <string, Ident<int>> integers;
	map <string, Ident<string>> strings;
};

Interpretator::Interpretator(istream& stream)
	: la(stream)
	, neadRead(true)
{	}

LexType Interpretator::getLex(LexType type) {
	if (neadRead) {
		la.moveNext();
		lex = la.current();
		if (type != LEX_NULL && type != lex.type)
			throw lex;
	} else {
		neadRead = true;
	}
	cout << lex << endl;
	return lex.type;
}

void inline Interpretator::checkLex(LexType type) {
	if (lex.type != type)
		throw lex;
}

void Interpretator::Inerpretate() {
	try {
		getLex(LEX_PROGRAM);
		getLex(LEX_OP_BRACE);
		getLex();

		Descriptions();
		Operators();
		checkLex(LEX_NULL);
		cout << "Success" << endl;
	} catch (Lex lex) {
		if (lex.type == LEX_NULL)
			cout << "Error: Unexpected end of file" << endl;
		else
			cout << "Error: line " << la.getLine() <<
				", Unexpected lexeme: " << endl << lex << endl;
	}
}

void Interpretator::Descriptions() {
	while (lex.type == LEX_INT || lex.type == LEX_STR)	{
		switch (lex.type) {
			case LEX_INT: ReadId(LEX_NUM);    break;
			case LEX_STR: ReadId(LEX_STRING); break;
		}
	}
}

void Interpretator::ReadId(const LexType type) {
	getLex(LEX_ID);
	switch(getLex()) {
		case LEX_COMMA:     ReadId(type);    break;
		case LEX_ASSIGN:    ReadValue(type); break;
		case LEX_SEMICOLON: getLex();        break;
		default:            throw lex;       break;
	}
}

void Interpretator::ReadValue(const LexType type) {
	getLex();
	if (type == LEX_NUM) {
		if (lex.type == LEX_PLUS || lex.type == LEX_MINUS)
			getLex();
	}
	checkLex(type);
	switch (getLex()) {
		case LEX_COMMA:     ReadId(type); break;
		case LEX_SEMICOLON: getLex();     break;
		default:            throw lex;    break;
	}
}

void Interpretator::Operators() {
	while (lex.type != LEX_CL_BRACE) {
		switch(lex.type) {
			case LEX_OP_BRACE:
				getLex();
				Operators();
				break;
			default:
				Operator();
				break;
		}
	}
	getLex();
}

void Interpretator::Operator() {
	switch(lex.type) {
		case LEX_READ:  ReadFunc();   break;
		case LEX_WRITE: WriteFunc();  break;
		case LEX_IF:    IfBlock();    break;
		case LEX_FOR:   ForBlock();   break;
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
	do {
		getLex();
		Expression();
	} while (lex.type == LEX_COMMA);
	checkLex(LEX_CL_ROUND);
	getLex(LEX_SEMICOLON);
	getLex();
}

void Interpretator::IfBlock() {
	getLex(LEX_OP_ROUND);
	getLex();
	Expression();
	checkLex(LEX_CL_ROUND);

	if (getLex() == LEX_OP_BRACE) {
		getLex();
		Operators();
	} else {
		Operator();
	}

	if(lex.type == LEX_ELSE) {
		getLex();
		if (lex.type == LEX_OP_BRACE) {
			getLex();
			Operators();
		} else {
			Operator();
		}
	}
}

void Interpretator::ForBlock() {
	getLex(LEX_OP_ROUND);

	getLex();
	Expression();
	checkLex(LEX_SEMICOLON);

	getLex();
	Expression();
	checkLex(LEX_SEMICOLON);

	getLex();
	Expression();
	checkLex(LEX_CL_ROUND);

	if (getLex() == LEX_OP_BRACE) {
		getLex();
		Operators();
	} else {
		Operator();
	}
}

//Expressions

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
	getLex();
}

void Interpretator::Sum() {
	Pr();
	while (lex.type == LEX_PLUS || lex.type == LEX_MINUS) {
		SumOp();
		Pr();
	}
}

void Interpretator::SumOp() {
	getLex();
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
			getLex();
			Expression();
			checkLex(LEX_CL_ROUND);
			getLex();
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
