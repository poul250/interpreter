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
	Context(map<string, Data>&);

	stack<Data> st;
	map<string, Data>& vars;
	int commandIndex;
};

class PolizOp {
public:
	virtual void execute(Context&) = 0;
};

class PolizEmpty : public PolizOp {
public:
	virtual void execute(Context&) override { }
};

class PolizPop : public PolizOp {
public:
	virtual void execute(Context&) override;
};

class PolizRead : public PolizOp {
public:
	virtual void execute(Context&) override;
};

class PolizWrite : public PolizOp {
public:
	virtual void execute(Context&) override;
};

class PolizData : public PolizOp {
public:
	virtual void execute(Context&) override;
	PolizData(Data src, bool v = false) : data(src), var(v){	}
private:
	Data data;
	bool var;
};

class PolizExpr : public PolizOp {
public:
	PolizExpr(LexType t) : type(t) { }
	virtual void execute(Context&) override;
private:
	LexType type;
};

class PolizIfNotJump : public PolizOp {
public:
	virtual void execute(Context&) override;
	PolizIfNotJump(int j = 0) : jump(j) { }
	void setJump(int j) { jump = j; }
private:
	int jump;
};

class PolizJump : public PolizOp {
public:
	virtual void execute(Context&) override;
	PolizJump(int j = 0) : jump(j) { }
	void setJump(int j) { jump = j; }
private:
	int jump;
};

}// namespace
