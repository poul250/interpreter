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
        val.i = 0;
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
    if (src.type == LEX_NUM) {
        val.i = src.val.i;
    } else if (src.type == LEX_REAL_NUM) {
        val.d = src.val.d;
    } else if (src.type == LEX_STRING) {
        cpy(src.val.s);
    }
}

Data::Data(Lex lex) {
    type = lex.type;
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
        switch (t1) {
        case LEX_NUM:
            switch (t2) {
            case LEX_NUM:      return LEX_NUM;
            case LEX_REAL_NUM: return LEX_NUM;
            case LEX_STRING:   throw "incompatible types: int and string";
            default:           throw "unknown type";
            }
        case LEX_REAL_NUM:
            switch (t2) {
            case LEX_NUM:      return LEX_REAL_NUM;
            case LEX_REAL_NUM: return LEX_REAL_NUM;
            case LEX_STRING:   throw "incompatible types: real and string";
            default:           throw "unknown type";
            }
        case LEX_STRING:
            switch (t2) {
            case LEX_NUM:      throw "incompatible types: string and int";
            case LEX_REAL_NUM: throw "incompatible types: string and real";
            case LEX_STRING:   return LEX_STRING;
            default:           throw "unknown type";
            }
        default:
            throw "unknown type";
        }
    case LEX_PLUS:
    case LEX_MINUS:
    case LEX_DIV:
    case LEX_MUL:
        switch (t1) {
        case LEX_NUM:
            switch (t2) {
            case LEX_NUM:      return LEX_NUM;
            case LEX_REAL_NUM: return LEX_REAL_NUM;
            case LEX_STRING:   throw "incompatible types: int and string";
            default:           throw "unknown type";
            }
        case LEX_REAL_NUM:
            switch (t2) {
            case LEX_NUM:      return LEX_REAL_NUM;
            case LEX_REAL_NUM: return LEX_REAL_NUM;
            case LEX_STRING:   throw "incompatible types: int and string";
            default:           throw "unknown type";
            }
        case LEX_STRING:
            switch (t2) {
            case LEX_NUM:      throw "incompatible types: string and int";
            case LEX_REAL_NUM: throw "incompatible types: string and real";
            case LEX_STRING:   return LEX_STRING;
            default:           throw "unknown type";
            }
        }
    }
}

void Data::cpy(const string& src) {
    val.s = new char[src.size() + 1];
    strcpy(val.s, src.c_str());
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
            }
        case LEX_NUM:
            switch (src.type) {
                case LEX_NUM:      return val.i = src.val.i;
                case LEX_REAL_NUM: return val.i = src.val.d;
                default: throw "runtime error: incompatible types";
            }
        case LEX_REAL_NUM:
            switch (src.type) {
                case LEX_NUM: return val.d = src.val.i;
                case LEX_REAL_NUM: return val.d = src.val.d;
                default: throw "runtime error: incompatible types";
            }
        case LEX_STRING:
            switch (src.type) {
                case LEX_NUM:
                case LEX_REAL_NUM:
                    throw "runtime error: uncompatible types";
                case LEX_STRING:
                    delete[] val.s;
                    cpy(src.val.s);
                    return src.val.s;
            }
    }
}

Data Data::operator+(const Data& src) {
    if (type == LEX_STRING && src.type == LEX_STRING)
        return string(*this) + string(src);
    if (type == LEX_NUM && src.type == LEX_NUM) {
        return int(*this) + int(src);
    }
    double answer = (this->type == LEX_INT? int(*this) : double(*this));
    return answer + (src.type == LEX_INT? int(src) : double(src));
}

Data Data::operator-(const Data& src) {
    if (type == LEX_NUM && src.type == LEX_NUM) {
        return int(*this) - int(src);
    }
    double answer = (this->type == LEX_INT? int(*this) : double(*this));
    return answer - (src.type == LEX_INT? int(src) : double(src));
}

Data Data::operator*(const Data& src) {
    if (type == LEX_NUM && src.type == LEX_NUM) {
        return int(*this) * int(src);
    }
    double answer = (this->type == LEX_INT? int(*this) : double(*this));
    return answer * (src.type == LEX_INT? int(src) : double(src));
}

Data Data::operator/(const Data& src) {
    if (type == LEX_NUM && src.type == LEX_NUM) {
        return int(*this) / int(src);
    }
    double answer = (this->type == LEX_INT? int(*this) : double(*this));
    return answer / (src.type == LEX_INT? int(src) : double(src));
}

Data Data::operator<(const Data& src) {
    if (type == LEX_STRING && src.type == LEX_STRING)
        return strcmp(val.s, src.val.s) < 0;
    if (type == LEX_NUM && src.type == LEX_NUM) {
        return int(*this) < int(src);
    }
    double answer = (type == LEX_INT? int(*this) : double(*this));
    return answer < (src.type == LEX_INT? int(src) : double(src));
}

Data Data::operator>(const Data& src) {
    if (type == LEX_STRING && src.type == LEX_STRING)
        return strcmp(val.s, src.val.s) > 0;
    if (type == LEX_NUM && src.type == LEX_NUM) {
        return int(*this) < int(src);
    }
    double answer = (type == LEX_INT? int(*this) : double(*this));
    return answer > (src.type == LEX_INT? int(src) : double(src));
}

Data Data::operator<=(const Data& src) {
    if (type == LEX_STRING && src.type == LEX_STRING)
        return strcmp(val.s, src.val.s) <= 0;
    if (type == LEX_NUM && src.type == LEX_NUM) {
        return int(*this) < int(src);
    }
    double answer = (type == LEX_INT? int(*this) : double(*this));
    return answer <= (src.type == LEX_INT? int(src) : double(src));
}

Data Data::operator>=(const Data& src) {
    if (type == LEX_STRING && src.type == LEX_STRING)
        return strcmp(val.s, src.val.s) >= 0;
    if (type == LEX_NUM && src.type == LEX_NUM) {
        return int(*this) < int(src);
    }
    double answer = (type == LEX_INT? int(*this) : double(*this));
    return answer >= (src.type == LEX_INT? int(src) : double(src));
}

Data Data::operator==(const Data& src) {
    if (type == LEX_STRING && src.type == LEX_STRING)
        return strcmp(val.s, src.val.s) == 0;
    if (type == LEX_NUM && src.type == LEX_NUM) {
        return int(*this) < int(src);
    }
    double answer = (type == LEX_INT? int(*this) : double(*this));
    return answer == (src.type == LEX_INT? int(src) : double(src));
}

Data Data::operator!=(const Data& src) {
    if (type == LEX_STRING && src.type == LEX_STRING)
        return strcmp(val.s, src.val.s) != 0;
    if (type == LEX_NUM && src.type == LEX_NUM) {
        return int(*this) < int(src);
    }
    double answer = (type == LEX_INT? int(*this) : double(*this));
    return answer != (src.type == LEX_INT? int(src) : double(src));
}

Data Data::operator&&(const Data& src) {
    return int(*this) && int(src);
}

Data Data::operator||(const Data& src) {
    return int(*this) || int(src);
}

Data Data::operator!() {
    return !(int(*this));
}
} //namespace
