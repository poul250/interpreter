#include "Poliz.hpp"

using namespace std;

namespace Pawka {

Context::Context(map<string, Data>vars)
        : vars(vars)
        , commandIndex(0)
{   }

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
        cont.st.push(cont.vars[data]);
    else
        cont.st.push(data);

}

}//namespace
