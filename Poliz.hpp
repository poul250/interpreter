#pragma once
#include <vector>
#include <stack>
#include <map>

using namespace std;

namespace Pawka {

struct Context {
	stack<PolizOp> st;
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

}// namespace
