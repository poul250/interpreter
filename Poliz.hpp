#pragma once
#include <vector>
#include <stack>
#include <map>
#include "utils.hpp"
#include "Lex.hpp"

using namespace std;

namespace Pawka {

struct Context {
	stack<Value> st;
	map<string, Ident*>& vars;
	map<string, int>& integers;
	map<string, string>& strings;
	map<string, double>& reals;
	int commandIndex;
};

class PolizOp {
public:
	virtual void execute(Context& context) = 0;
};

class PolizRead : public PolizOp {
	virtual void execute(Context& context) override;
};

}// namespace
