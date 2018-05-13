#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <stack>
#include "Lex.hpp"
#include "Poliz.hpp"
#include "utils.hpp"
#include "types.hpp"

using namespace std;

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
	void IfBlock();
	void ForBlock();
	void WhileBlock();
	void ReadFunc();
	void WriteFunc();
	void BreakOp();
	void OpExpression();

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
	vector<PolizOp*> ops;
};

Interpretator::Interpretator(istream& stream)
	: la(stream)
{	}

Interpretator::~Interpretator() {
	for(i : ops)
		delete i;
}

bool inline Interpretator::declared(const string& name) {
	return data.count(name) > 0;
}

void inline Interpretator::checkLex(LexType type) {
	if (lex.type != type)
		throw lex;
}

LexType inline Interpretator::getType(const Lex& l) {
	return l.type == LEX_ID ? data[l.str].getType() : l.type;
}

LexType Interpretator::getLex(LexType type) {
	la.moveNext();
	lex = la.current();
	if (type != LEX_NULL && type != lex.type)
		throw lex;
	cout << lex << endl;
	return lex.type;
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
	} catch (const char* s) {
		cout << "Error: Line " << la.getLine() << ", " << s << endl;
	} catch (out_of_range) {
		cout << "Error: Line " << la.getLine() << ", " << "out of range" <<endl;
	}
	execute();
}

void Interpretator::execute() {
	Context cont(data);
	while (cont.commandIndex < ops.size()) {
		PolizOp* op = ops[cont.commandIndex];
		cont.commandIndex++;
		op->execute(cont);
	}
}

void Interpretator::Descriptions() {
	while (lex.type == LEX_INT || lex.type == LEX_STR || lex.type == LEX_REAL) {
		switch (lex.type) {
			case LEX_INT:  ReadId(LEX_NUM);      break;
			case LEX_STR:  ReadId(LEX_STRING);   break;
			case LEX_REAL: ReadId(LEX_REAL_NUM); break;
		}
	}
}

void Interpretator::ReadId(LexType type) {
	getLex(LEX_ID);
	if (declared(lex.str))
		throw "variable \"" + lex.str + "\" has already been declared";

	string name = lex.str;

	data[name] = Data(type);
	switch(getLex()) {
		case LEX_COMMA:
			ReadId(type);
			break;
		case LEX_ASSIGN:
			ReadValue(name, type);
			break;
		case LEX_SEMICOLON:
			getLex();
			break;
		default:
			throw lex;
			break;
	}
}

void Interpretator::ReadValue(const string& name, LexType type) {
	getLex();
	if (type == LEX_NUM || type == LEX_REAL_NUM) {
		if (lex.type == LEX_PLUS || lex.type == LEX_MINUS) {
			string s = lex.str;
			getLex();
			lex.str = s + lex.str;
		}
	}
	Data::compatible(type, LEX_ASSIGN, lex.type);

	data[name] = lex;

	switch (getLex()) {
		case LEX_COMMA:     ReadId(type); break;
		case LEX_SEMICOLON: getLex();     break;
		default:            throw lex;    break;
	}
}

void Interpretator::Operators() {
	while 	(lex.type != LEX_CL_BRACE) {
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
		case LEX_READ:  ReadFunc();     break;
		case LEX_WRITE: WriteFunc();    break;
		case LEX_IF:    IfBlock();      break;
		case LEX_FOR:   ForBlock();     break;
		case LEX_ID:    OpExpression(); break;
		case LEX_WHILE: WhileBlock();   break;
		case LEX_BREAK: BreakOp();      break;
		default:        throw lex;      break;
	}
}

void Interpretator::ReadFunc() {
	getLex(LEX_OP_ROUND);
	getLex(LEX_ID);
	ops.push_back(new PolizData(lex.str));
	ops.push_back(new PolizRead());
	getLex(LEX_CL_ROUND);
	getLex(LEX_SEMICOLON);
	getLex();
}

void Interpretator::WriteFunc() {
	getLex(LEX_OP_ROUND);
	do {
		getLex();
		st = stack<Lex>();
		Expression();
		ops.push_back(new PolizWrite());
	} while (lex.type == LEX_COMMA);
	checkLex(LEX_CL_ROUND);
	getLex(LEX_SEMICOLON);
	getLex();
}

void Interpretator::BreakOp() {
	getLex(LEX_SEMICOLON);
	getLex();
}

void Interpretator::IfBlock() {
	getLex(LEX_OP_ROUND);
	getLex();
	st = stack<Lex>();
	Expression();
	checkLex(LEX_CL_ROUND);
	if (st.top().type != LEX_NUM)
		throw "logical value expected";

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
	st = stack<Lex>();
	Expression();
	if (getType(st.top()) != LEX_NUM)
		throw "logical value expected";
	checkLex(LEX_SEMICOLON);

	getLex();
	st = stack<Lex>();
	Expression();
	checkLex(LEX_SEMICOLON);

	getLex();
	st = stack<Lex>();
	Expression();
	checkLex(LEX_CL_ROUND);

	if (getLex() == LEX_OP_BRACE) {
		getLex();
		Operators();
	} else {
		Operator();
	}
}

void Interpretator::WhileBlock() {
	getLex(LEX_OP_ROUND);
	getLex();
	st = stack<Lex>();
	Expression();
	if(getType(st.top()) != LEX_NUM) {
		throw "expected logical value";
	}
	checkLex(LEX_CL_ROUND);
	getLex();
	if (lex.type == LEX_OP_BRACE) {
		getLex();
		Operators();
	} else {
		Operator();
	}
}

void Interpretator::OpExpression() {
	st = stack<Lex>();
	Expression();
	checkLex(LEX_SEMICOLON);
	getLex();
}

//Expressions

void Interpretator::Expression() {
	Or();
	int shift = ops.size();
	if (lex.type == LEX_ASSIGN) {
		getLex();
		Or();
		AssignOp(shift);
	}
}

void Interpretator::AssignOp(int shift) {
	LexType t2 = getType(st.top());
	st.pop();
	Lex l1 = st.top();
	st.pop();
	if (l1.type != LEX_ID)
		throw "tried to assign const";
	delete ops[shift];
	ops[shift] = new PolizData(l1.str);
	st.push(Data::compatible(getType(l1), LEX_ASSIGN, t2));
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
	LexType t2 = getType(st.top());
	st.pop();
	LexType t1 = getType(st.top());
	st.pop();
	if (t1 != t2 || t1 != LEX_NUM)
		throw "incompatible types of operands";
	st.push(Lex(LEX_NUM));
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
	LexType t2 = getType(st.top());
	st.pop();
	LexType t1 = getType(st.top());
	st.pop();
	if (t1 != t2 || t1 != LEX_NUM)
		throw "incompatible types of operands";
	st.push(Lex(LEX_NUM));
}

void Interpretator::Relation() {
	Sum();
	if (lex.type == LEX_LESS || lex.type == LEX_GREATER || lex.type == LEX_LE ||
			lex.type == LEX_GE || lex.type == LEX_EQ || lex.type == LEX_NE)
	{
		LexType t = lex.type;
		getLex();
		Sum();
		RelationOp(t);
	}
}

void Interpretator::RelationOp(LexType type) {
	LexType t2 = getType(st.top());
	st.pop();
	LexType t1 = getType(st.top());
	st.pop();
	if (t1 != t2 && (t1 == LEX_STRING || t2 == LEX_STRING))
		throw "incompatible types of operands";
	st.push(Lex(LEX_NUM));
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
	LexType t2 = getType(st.top());
	st.pop();
	LexType t1 = getType(st.top());
	st.pop();

	st.push(Data::compatible(t1, type, t2));
	ops.push_back(new PolizExpr(type));
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
	LexType t2 = getType(st.top());
	st.pop();
	LexType t1 = getType(st.top());
	st.pop();

	st.push(Data::compatible(t1, type, t2));
}

void Interpretator::Not() {
	if (lex.type == LEX_PLUS || lex.type == LEX_MINUS || lex.type == LEX_NOT) {
		LexType t = lex.type;
		if (t == LEX_MINUS)
			ops.push_back(new PolizData(0));
		getLex();
		Atom();
		NotOp(t);
	}
	else {
		Atom();
	}
}

void Interpretator::NotOp(LexType type) {
	LexType t = getType(st.top());
	st.pop();
	if (t != LEX_NUM)
		throw "incompatible types of operands";
	if (type == LEX_MINUS || type == LEX_NOT)
		ops.push_back(new PolizExpr(type));
	st.push(Lex(LEX_NUM));
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
			ops.push_back(new PolizData(lex.str, true));
			st.push(lex);
			break;
		case LEX_NUM:
		case LEX_REAL_NUM:
		case LEX_STRING:
			ops.push_back(new PolizData(lex));
			st.push(lex);
			break;
		default:
			throw lex;
			break;
	}
	getLex();
}
} //namespace

using namespace Pawka;

int main(int argc, char* argv[])
{
	ifstream fstream;
	bool fromFile = false;
	if (argc >= 3) {
		cout << "Usage: " << argv[0] << " [file_name]" << endl;
		return -1;
	}
	else if (argc == 2) {
		fstream.open(argv[1]);
		fromFile = true;
		if (!fromFile) {
			cout << "Error: can't open file" << endl;
			return -1;
		}
	}

	istream& stream = fromFile ? fstream : cin;

	Interpretator sa(stream);
	sa.Inerpretate();

	return 0;
}
