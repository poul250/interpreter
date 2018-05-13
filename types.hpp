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
    int getI() const { return val.i; }
    double getD() const { return val.d; }
    string getS() const { return val.s; }

    operator int() const { return val.i; }
    operator double() const { return val.d; }
    operator string() const { return val.s; }

    friend ostream& operator<<(ostream&, const Data&);
    friend istream& operator>>(istream&, Data&);
private:
    void cpy(const string&);
    // void cpy(const char&);

    LexType type;
    Value val;
};
} //namespace
