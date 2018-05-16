#include <string>
#include <cstring>
#include <iostream>
#include "types.hpp"
#include "Lex.hpp"

using namespace std;

namespace Pawka {

ostream& operator<<(ostream& stream, const Data& data) {
    if (data.type == LEX_NUM)
        stream << data.val.i;
    else if (data.type == LEX_REAL_NUM)
        stream << data.val.d;
    else if (data.type == LEX_STRING)
        stream << data.val.s;
    return stream;
}

istream& operator>>(istream& stream, Data& data) {
    if (data.type == LEX_NUM) {
        stream >> data.val.i;
    } else if (data.type == LEX_REAL_NUM) {
        stream >> data.val.d;
    } else if (data.type == LEX_STRING) {
        string s;
        cin >> s;
        delete[] data.val.s;
        data.cpy(s);
    }
    return stream;
}

Data::Data(LexType type)
        : type(type) {
    if (type == LEX_NULL)
        val.s = nullptr;
    else if (type == LEX_STRING)
        cpy("");
    else if (type == LEX_NUM)
        val.i = 0;
    else if (type == LEX_REAL_NUM)
        val.d = 0.;
}

Data::Data(int x)
        : type(LEX_NUM) {
    val.i = x;
}

Data::Data(double x)
        : type(LEX_REAL_NUM) {
    val.d = x;
}

Data::Data(string x)
        : type(LEX_STRING) {
    cpy(x);
}

Data::Data(const char* x)
        : type(LEX_STRING) {
    cpy(x);
}

Data::Data(const Data& src)
        : type(src.type) {
    if (type == LEX_NUM) {
        val.i = src.val.i;
    } else if (type == LEX_REAL_NUM) {
        val.d = src.val.d;
    } else if (type == LEX_STRING) {
        // cout << src.val.s << "  ";
        cpy(src.val.s);
        // cout << val.s;
    }
}

Data::Data(Lex lex)
        :type(lex.type) {
    switch (lex.type) {
        case LEX_NUM:      val.i = stoi(lex.str); break;
        case LEX_REAL_NUM: val.d = stod(lex.str); break;
        case LEX_STRING:   cpy(lex.str);          break;
        default:           throw "unknown type";
    }
}

Data::~Data() {
    if (type == LEX_STRING)
        delete[] val.s;
}

LexType Data::compatible(LexType t1, LexType op, LexType t2) {
    switch (op) {
    case LEX_ASSIGN:
        if (t1 == LEX_STRING && t2 == LEX_STRING)
            return LEX_STRING;
        else if ((t1 == LEX_NUM) && (t2 != LEX_STRING))
            return LEX_NUM;
        else if (t1 == LEX_REAL_NUM && t2 != LEX_STRING)
            return LEX_REAL_NUM;
        else
            throw "incompatible types";
    case LEX_PLUS:
        if (t1 == LEX_STRING && t2 == LEX_STRING)
            return LEX_STRING;
        else if (t1 == LEX_NUM && t2 == LEX_NUM)
            return LEX_NUM;
        else if (t1 != LEX_STRING && t2 != LEX_STRING)
            return LEX_REAL_NUM;
        else
            throw "incompatible types";
    case LEX_MINUS:
    case LEX_DIV:
    case LEX_MUL:
        if (t1 == LEX_NUM && t2 == LEX_NUM)
            return LEX_NUM;
        else if (t1 != LEX_STRING && t2 != LEX_STRING)
            return LEX_REAL_NUM;
        else
            throw "incompatible types";
    case LEX_LESS:
    case LEX_GREATER:
    case LEX_EQ:
    case LEX_NE:
    case LEX_LE:
    case LEX_GE:
    case LEX_AND:
    case LEX_OR:
        if (t1 != LEX_NUM || t2 != LEX_NUM)
            throw "incompatible types";
        return LEX_NUM;
    case LEX_NOT:
        if (t2 != LEX_NUM)
            throw "incompatible types";
        return LEX_NUM;
    }
}

void Data::cpy(const string& src) {
    const char* str = src.c_str();
    val.s = new char[strlen(str) + 1];
    strcpy(val.s, str);
}

Data::operator int() const {
    if (type == LEX_NUM)
        return val.i;
    else if (type == LEX_REAL_NUM)
        return val.d;
    else if (type == LEX_STRING)
        throw "bad cast to int";
}
Data::operator double() const {
    if (type == LEX_NUM)
        return val.i;
    else if (type == LEX_REAL_NUM)
        return val.d;
    else if (type == LEX_STRING)
        throw "bad cast to double";
}
Data::operator string() const {
    if (type == LEX_NUM || type == LEX_REAL_NUM)
        throw "bad cast to sring";
    else if (type == LEX_STRING)
        return string(val.s);
}

// void Data::cpy(const char* src) {
//     if (src == nullptr) {
//
//     }
// }

Data Data::operator=(const Data& src) {
    switch (type) {
        case LEX_NULL:
            type = src.type;
            switch (src.type) {
                case LEX_NUM:      return val.i = src.val.i;
                case LEX_REAL_NUM: return val.d = src.val.d;
                case LEX_STRING:   cpy(src.val.s); return *this;
                default:           throw "?????";
            }
        case LEX_NUM:
            switch (src.type) {
                case LEX_NUM:      return val.i = src.val.i;
                case LEX_REAL_NUM: return val.i = src.val.d;
                default: throw "runtime error: incompatible types NUM";
            }
        case LEX_REAL_NUM:
            switch (src.type) {
                case LEX_NUM: return val.d = src.val.i;
                case LEX_REAL_NUM: return val.d = src.val.d;
                default: throw "runtime error: incompatible types REAL_NUM";
            }
        case LEX_STRING:
            switch (src.type) {
                case LEX_NUM:
                case LEX_REAL_NUM:
                    throw "runtime error: uncompatible types STRING";
                case LEX_STRING:
                    delete[] val.s;
                    cpy(src.val.s);
                    return src.val.s;
            }
    }
}

Data Data::operator+(const Data& src) {
    LexType t = compatible(type, LEX_ASSIGN, src.type);
    if (t == LEX_STRING)
        return string(*this) + string(src);
    else if (t == LEX_NUM)
        return int(*this) + int(src);
    return double(*this) + double(src);
}

Data Data::operator-(const Data& src) {
    LexType t = compatible(type, LEX_MINUS, src.type);
    if (t == LEX_NUM)
        return int(*this) - int(src);
    return double(*this) - double(src);
}

Data Data::operator*(const Data& src) {
    LexType t = compatible(type, LEX_MUL, src.type);
    if (t == LEX_NUM)
        return int(*this) * int(src);
    return double(*this) * double(src);
}

Data Data::operator/(const Data& src) {
    LexType t = compatible(type, LEX_DIV, src.type);
    if (t == LEX_NUM)
        return int(*this) / int(src);
    return double(*this) / double(src);
}

Data Data::operator<(const Data& src) {
    compatible(type, LEX_LESS, src.type);
    if (type == LEX_STRING && src.type == LEX_STRING)
        return strcmp(val.s, src.val.s) < 0;
    if (type == LEX_NUM && src.type == LEX_NUM)
        return int(*this) < int(src);
    return double(*this) < double(src);
}

Data Data::operator>(const Data& src) {
    compatible(type, LEX_GREATER, src.type);
    if (type == LEX_STRING && src.type == LEX_STRING)
        return strcmp(val.s, src.val.s) > 0;
    if (type == LEX_NUM && src.type == LEX_NUM)
        return int(*this) > int(src);
    return double(*this) > double(src);
}

Data Data::operator<=(const Data& src) {
    compatible(type, LEX_LE, src.type);
    if (type == LEX_STRING && src.type == LEX_STRING)
        return strcmp(val.s, src.val.s) <= 0;
    if (type == LEX_NUM && src.type == LEX_NUM)
        return int(*this) <= int(src);
    return double(*this) <= double(src);
}

Data Data::operator>=(const Data& src) {
    compatible(type, LEX_GE, src.type);
    if (type == LEX_STRING && src.type == LEX_STRING)
        return strcmp(val.s, src.val.s) >= 0;
    if (type == LEX_NUM && src.type == LEX_NUM)
        return int(*this) >= int(src);
    return double(*this) >= double(src);
}

Data Data::operator==(const Data& src) {
    compatible(type, LEX_EQ, src.type);
    if (type == LEX_STRING && src.type == LEX_STRING)
        return strcmp(val.s, src.val.s) == 0;
    if (type == LEX_NUM && src.type == LEX_NUM)
        return int(*this) == int(src);
    return double(*this) == double(src);
}

Data Data::operator!=(const Data& src) {
    compatible(type, LEX_NE, src.type);
    if (type == LEX_STRING && src.type == LEX_STRING)
        return strcmp(val.s, src.val.s) != 0;
    if (type == LEX_NUM && src.type == LEX_NUM)
        return int(*this) != int(src);
    return double(*this) != double(src);
}

Data Data::operator&&(const Data& src) {
    compatible(type, LEX_AND, src.type);
    return int(*this) && int(src);
}

Data Data::operator||(const Data& src) {
    compatible(type, LEX_OR, src.type);
    return int(*this) || int(src);
}

Data Data::operator!() {
    compatible(LEX_NULL, LEX_NOT, type);
    return !(int(*this));
}
} //namespace
