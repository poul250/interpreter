#pragma once
#include <string>
#include <variant>

#include "Lex.hpp"

namespace Pawka {

class Value {
private:
    using val_t = std::variant<int, double, std::string>;
    val_t val;
public:
    Value& operator= (const Value& other);
    Value operator+ (const Value& other) const;
    Value operator- (const Value& other) const;
    Value operator* (const Value& other) const;
    Value operator/ (const Value& other) const;
    Value operator< (const Value& other) const;
    Value operator<=(const Value& other) const;
    Value operator> (const Value& other) const;
    Value operator>=(const Value& other) const;
    Value operator!=(const Value& other) const;
    Value operator==(const Value& other) const;
};

} // namespace Pawka