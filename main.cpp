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
	Interpretator(istream& stream);

	void Inerpretate();
private:
	struct Ident {
		Ident(const string& src)
			: assign(false)
			, type(LEX_NULL)
			, name(src)
		{	}
		Ident(const Ident& src)
			: assign(src.assign)
			, type(src.type)
			, name(src.name)
		{	}

		string name;
		LexType type;
		bool assign;
	};
	//Program states
	void Descriptions();
	void ReadId(LexType);
	void ReadValue(const string&, LexType);
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
	void RelationOp(LexType);
	void Sum();
	void SumOp(LexType);
	void Pr();
	void PrOp(LexType);
	void Not();
	void NotOp(LexType);
	void Atom();

	bool inline declared(const string&);
	LexType getLex(LexType type = LEX_NULL);
	void inline checkLex(LexType);
	int strToInt(const string&);

	LexAnalizer la;
	bool neadRead;
	Lex lex;

	map<string, Ident*> vars;
	map<string, int> integers;
	map<string, string> strings;
	stack<LexType> st;
};

Interpretator::Interpretator(istream& stream)
	: la(stream)
	, neadRead(true)
{	}

bool inline Interpretator::declared(const string& name) {
	return vars.count(name) > 0;
}

LexType Interpretator::getLex(LexType type) {
	if (neadRead) {
		la.moveNext();
		lex = la.current();
		if (type != LEX_NULL && type != lex.type)
			throw lex;
	} else {
		neadRead = true;
	}
	return lex.type;
}

void inline Interpretator::checkLex(LexType type) {
	if (lex.type != type)
		throw lex;
}

int Interpretator::strToInt(const string& src) {
	int x = 0;
	int sign = 1;
	int start = 0;
	if (src[0] == '+' || src[0] == '-') {
		start = 1;
		sign = src[0] == '+' ? 1 : -1;
	}
	for (int i = start; i < src.size(); ++i) {
		x = x * 10 + src[i] - '0';
	}
	return x * sign;
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
	} catch (string s) {
		cout << "Error: Line " << la.getLine() << ", " << s << endl;
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

void Interpretator::ReadId(LexType type) {
	getLex(LEX_ID);

	if (declared(lex.str))
		throw "variable \"" + lex.str + "\" has already been declared";
	vars[lex.str] = new Ident(lex.str);
	if (type == LEX_NUM) {
		integers[lex.str] = int();
	} else if (type == LEX_STRING) {
		strings[lex.str] = string();
	}

	switch(getLex()) {
		case LEX_COMMA:     ReadId(type);             break;
		case LEX_ASSIGN:    ReadValue(lex.str, type); break;
		case LEX_SEMICOLON: getLex();                 break;
		default:            throw lex;                break;
	}
}

void Interpretator::ReadValue(const string& name, LexType type) {
	getLex();
	if (type == LEX_NUM) {
		if (lex.type == LEX_PLUS || lex.type == LEX_MINUS) {
			string s = lex.str;
			getLex();
			lex.str = s + lex.str;
		}
	}

	checkLex(type);
	vars[name]->assign = true;

	if (type == LEX_NUM) {
		integers[name] = strToInt(lex.str);
	} else if (type == LEX_STRING) {
		strings[name] = lex.str;
	}

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
		getLex();
		Or();
		AssignOp();
	}
}

void Interpretator::AssignOp() {
	LexType t2 = st.top();
	st.pop();
	LexType t1 = st.top();
	st.pop();
	if (t1 != t2) {
		throw "incompatible types of operands";
	}
	st.push(t1);
}

void Interpretator::Or() {
	And();
	while (lex.type == LEX_OR) {
		getLex();
		And();
		OrOp();
	}
}

void Interpretator::OrOp() {
	LexType t2 = st.top();
	st.pop();
	LexType t1 = st.top();
	st.pop();
	if (t1 != t2 || t1 != LEX_NUM) {
		throw "incompatible types of operands";
	}
	st.push(LEX_NUM);
}

void Interpretator::And() {
	Relation();
	while (lex.type == LEX_AND) {
		getLex();
		Relation();
		AndOp();
	}
}

void Interpretator::AndOp() {
	LexType t2 = st.top();
	st.pop();
	LexType t1 = st.top();
	st.pop();
	if (t1 != t2 || t1 != LEX_NUM) {
		throw "incompatible types of operands";
	}
	st.push(LEX_NUM);
}

void Interpretator::Relation() {
	Sum();
	while (lex.type == LEX_LESS || lex.type == LEX_GREATER || lex.type == LEX_LE ||
			lex.type == LEX_GE || lex.type == LEX_EQ || lex.type == LEX_NE) {
		LexType t = lex.type;
		getLex();
		Sum();
		RelationOp(t);
	}
}

void Interpretator::RelationOp(LexType type) {
	LexType t2 = st.top();
	st.pop();
	LexType t1 = st.top();
	st.pop();
	if (t1 != t2) {
		throw "incompatible types of operands";
	}
	st.push(t1);
}

void Interpretator::Sum() {
	Pr();
	while (lex.type == LEX_PLUS || lex.type == LEX_MINUS) {
		LexType t = lex.type;
		getLex();
		Pr();
		SumOp(t);
	}
}

void Interpretator::SumOp(LexType type) {
	LexType t2 = st.top();
	st.pop();
	LexType t1 = st.top();
	st.pop();

	if (t1 != t2) {
		throw "incompatible types of operands";
	}
	st.push(t1);
}

void Interpretator::Pr() {
	Not();
	while (lex.type == LEX_MUL || lex.type == LEX_DIV) {
		LexType t = lex.type;
		getLex();
		Not();
		PrOp(t);
	}
}

void Interpretator::PrOp(LexType type) {
	LexType t2 = st.top();
	st.pop();
	LexType t1 = st.top();
	st.pop();

	if (t1 != t2) {
		throw "incompatible types of operands";
	}
	st.push(LEX_NUM);
}

void Interpretator::Not() {
	if (lex.type == LEX_PLUS || lex.type == LEX_MINUS || lex.type == LEX_NOT) {
		LexType t = lex.type;
		getLex();
		Atom();
		NotOp(t);
	}
}

void Interpretator::NotOp(LexType type) {
	LexType t = st.top();
	st.pop();

	if (t != LEX_NUM) {
		throw "incompatible types of operands";
	}
	st.push(LEX_NUM);
}

void Interpretator::Atom() {
	switch(lex.type) {
		case LEX_OP_ROUND:
			getLex();
			Expression();
			checkLex(LEX_CL_ROUND);
			break;
		case LEX_ID:
			if (!declared(lex.str))
				throw "undeclared var \"" + lex.str + "\"";
		case LEX_NUM:
		case LEX_STRING:
			st.push(lex.type);
			break;
		default:
			throw lex;
			break;
	}
	getLex();
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
