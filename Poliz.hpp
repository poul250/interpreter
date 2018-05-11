#pragma once
#include <vector>
#include <stack>

using namespace std;

namespace Pawka {

class PolizOp {
    virtual void execute() = 0;
};

class PolizSign : public PolizOp {

};

class PolizGO : public PolizOp {

};

class PolizFGO : public PolizOp {

};

class PolizAdress : public PolizOp {

};

class Poliz {
public:
    Poliz(map<string, Ident*>*, map<string, int>*,  
			map<string, string>*, map<string, double>*);
    ~Poliz();
private:
    vector<PolizOp*> prog;
    stack<PolizOp*> st;
	
	map<string, Ident*>* vars;
	map<string, int>* integers;
	map<string, string>* strings;
	map<string, double>* reals;
};

}// namespace
