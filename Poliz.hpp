#pragma once
#include <vector>
#include <stack>
#include <map>
#include <string>
#include "utils.hpp"
#include "types.hpp"
#include "Lex.hpp"

using namespace std;

namespace Pawka {

struct Context {
	Context(map<string, Data>);

	stack<Data> st;
	map<string, Data>& vars;
	int commandIndex;
};

class PolizOp {
public:
	virtual void execute(Context& context) = 0;
};

class PolizRead : public PolizOp {
public:
	virtual void execute(Context& context) override;
};

class PolizWrite : public PolizOp {
public:
	virtual void execute(Context& context) override;
};

class PolizData : public PolizOp {
public:
	virtual void execute(Context& context) override;
	PolizData(Data src, bool v = false) : data(src), var(v){	}
private:
	Data data;
	bool var;
};

class PolizExpr : public PolizOp {
public:
	PolizExpr(LexType t) : type(t) { }
	virtual void execute(Context& context) override;
private:
	LexType type;
};

}// namespace
