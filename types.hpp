#pragma once
#include "Lex.hpp"

namespace Pawka {

union Value{
    int i;
    double d;
    char *s;
};

class Data {
public:
    static LexType compatible(LexType t1, LexType op, LexType t2);

    Data(LexType type = LEX_NULL);
    Data(int);
    Data(double);
    Data(const string&);
    Data(const char*);
    Data(const Data&);
    Data(const Lex&);
    ~Data();

    Data operator= (const Data&);
    // Data operator+ (const Data&);
    // Data operator- (const Data&);
    // Data operator* (const Data&);
    // Data operator/ (const Data&);
    // Data operator< (const Data&);
    // Data operator> (const Data&);
    // Data operator<=(const Data&);
    // Data operator>=(const Data&);
    // Data operator==(const Data&);
    // Data operator!=(const Data&);
    // Data operator&&(const Data&);
    // Data operator||(const Data&);
    // Data operator! (const Data&);
    LexType getType() const { return type; }

    friend ostream& operator<<(ostream&, const Data&);
private:
    void cpy(const string&);
    // void cpy(const char&);

    LexType type;
    Value val;
};
} //namespace
