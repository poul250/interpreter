#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <stack>
#include "Lex.hpp"
#include "Poliz.hpp"
#include "types.hpp"

namespace Pawka {

class Interpretator {
public:
	Interpretator(istream& stream);
	~Interpretator();

	void Inerpretate();
private:

	//Program states
	void Descriptions();
	void ReadId(LexType);
	void ReadValue(const string&, LexType);
	void Operators();
	void Operator();
	void IfBlock(); //+
	void DoWhileBlock(); //+
	void WhileBlock(); //+
	void ReadFunc(); //+
	void WriteFunc(); //+
	void BreakOp(); //+
	void OpExpression(); //+

	//Expression states
	void Expression();
	void Assign();
	void AssignOp(int);
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
	void inline checkLex(LexType);
	LexType inline getType(const Lex&);
	LexType getLex(LexType type = LEX_NULL);

	void execute();

	LexAnalizer la;
	Lex lex;

	map<string, Data> data;
	stack<Lex> st;
	stack<vector<PolizJump*>*> whileExits;
	vector<PolizOp*> ops;
};

} //namespace
