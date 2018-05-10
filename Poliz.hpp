#pragma once
#include <vector>
#include <stack>

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
    Poliz();
    ~Poliz();
private:
    std::vector<PolizOp*> prog;
    std::stack<PolizOp*> st;
};

}// namespace
