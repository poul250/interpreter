#include "Poliz.hpp"

using namespace std;

namespace Pawka {

Context::Context(map<string, Data>&vars)
        : vars(vars)
        , commandIndex(0) {
}

void PolizPop::execute(Context& cont) {
    cont.st.pop();
}

void PolizRead::execute(Context& cont) {
    string name = cont.st.top();
    cont.st.pop();
    cin >> cont.vars[name];
}

void PolizWrite::execute(Context& cont) {
    cout << cont.st.top();
    cont.st.pop();
}

void PolizData::execute(Context& cont) {
    if (var)
        cont.st.push(cont.vars[string(data)]);
    else
        cont.st.push(data);
}

void PolizExpr::execute(Context& cont) {
    Data d2 = cont.st.top();
    cont.st.pop();
    if (type == LEX_NOT) {
        cont.st.push(!d2);
    } else {
        Data d1 = cont.st.top();
        cont.st.pop();
        if (type == LEX_ASSIGN)
            cont.st.push(cont.vars[d1] = d2);
        else if (type == LEX_PLUS)
            cont.st.push(d1 + d2);
        else if (type == LEX_MINUS)
            cont.st.push(d1 - d2);
        else if (type == LEX_MUL)
            cont.st.push(d1 * d2);
        else if (type == LEX_DIV)
            cont.st.push(d1 / d2);
        else if (type == LEX_LESS)
            cont.st.push(d1 < d2);
        else if (type == LEX_GREATER)
            cont.st.push(d1 > d2);
        else if (type == LEX_LE)
            cont.st.push(d1 <= d2);
        else if (type == LEX_GE)
            cont.st.push(d1 >= d2);
        else if (type == LEX_EQ)
            cont.st.push(d1 == d2);
        else if (type == LEX_NE)
            cont.st.push(d1 != d2);
        else if (type == LEX_AND)
            cont.st.push(d1 && d2);
        else if (type == LEX_OR)
            cont.st.push(d1 || d2);
    }
}

void PolizIfNotJump::execute(Context& cont) {
    int result = cont.st.top();
    cont.st.pop();
    if (!result)
        cont.commandIndex = jump;
}

void PolizJump::execute(Context& cont) {
    cont.commandIndex = jump;
}
}//namespace
