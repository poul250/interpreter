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

	void Interpretate();
private:

	//Program states
	void Descriptions();
	void ReadId(LexType);
	void ReadValue(const string&, LexType);
	void Operators();
	void OneOperator();
	void Operator();
	void IfBlock();
	void DoWhileBlock();
	void WhileBlock();
	void ReadFunc();
	void WriteFunc();
	void BreakOp();
	void OpExpression();

	//Expression states
	void Expression();
	void Assign();
	void Or();
	void And();
	void Relation();
	void Sum();
	void Pr();
	void Not();
	void Atom();
	void AssignOp(int);
	void NotOp(LexType);
	void ExprOp(LexType);

	bool inline declared(const string&);
	void inline checkLex(LexType);
	LexType inline getType(const Lex&);
	LexType getLex(LexType type = LEX_NULL);

	void execute();

	LexAnalizer la;
	Lex lex;

	map<string, Data> vars;
	stack<Lex> typesStack;
	stack<vector<PolizJump*>*> whileExits;
	vector<PolizOp*> ops;
};

} //namespace
