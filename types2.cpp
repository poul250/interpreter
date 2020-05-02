#include "types2.hpp"

#include <type_traits>

namespace Pawka {
namespace traits {
namespace {

struct No{};

template<typename T>
struct performable {
    enum { value = !std::is_same<T, No>::value };
};

template <typename T>
struct is_num {
    enum { value = std::is_same_v<T, int> || std::is_same_v<T, double> };
};

template<typename L, typename R> No operator+ (const L&, const R&);
template<typename L, typename R> No operator- (const L&, const R&);
template<typename L, typename R> No operator* (const L&, const R&);
template<typename L, typename R> No operator/ (const L&, const R&);
template<typename L, typename R> No operator< (const L&, const R&);
template<typename L, typename R> No operator<=(const L&, const R&);
template<typename L, typename R> No operator> (const L&, const R&);
template<typename L, typename R> No operator>=(const L&, const R&);
template<typename L, typename R> No operator!=(const L&, const R&);
template<typename L, typename R> No operator==(const L&, const R&);

} // namespace

template<typename L, typename R>
struct has {  
    enum {
        assign = is_num<L>::value ^ is_num<R>::value,
        plus  = performable<decltype(std::declval<L>() +  std::declval<R>())>::value,
        minus = performable<decltype(std::declval<L>() -  std::declval<R>())>::value,
        mul   = performable<decltype(std::declval<L>() *  std::declval<R>())>::value,
        div   = performable<decltype(std::declval<L>() /  std::declval<R>())>::value,
        lt    = performable<decltype(std::declval<L>() <  std::declval<R>())>::value,
        le    = performable<decltype(std::declval<L>() <= std::declval<R>())>::value,
        gt    = performable<decltype(std::declval<L>() >  std::declval<R>())>::value,
        ge    = performable<decltype(std::declval<L>() >= std::declval<R>())>::value,
        ne    = performable<decltype(std::declval<L>() != std::declval<R>())>::value,
        eq    = performable<decltype(std::declval<L>() == std::declval<R>())>::value,
    };
};

} // namespace traits

Value& Value::operator= (const Value& other) {
    std::visit([](auto& lhs, const auto& rhs) {
        if constexpr (traits::has<decltype(lhs), decltype(rhs)>::assign) lhs = rhs;
        else throw "Unsupported";
    }, val, other.val); 

    return *this;
}
Value Value::operator+ (const Value& other) const { 
    return std::visit([](const auto& lhs, const auto& rhs) -> Value {
        if constexpr (traits::has<decltype(lhs), decltype(rhs)>::plus) return {lhs + rhs};
        throw "Unsopported";
    }, val, other.val);    
}
Value Value::operator- (const Value& other) const { 
    return std::visit([](const auto& lhs, const auto& rhs) -> Value {
        if constexpr (traits::has<decltype(lhs), decltype(rhs)>::minus) return {lhs - rhs};
        throw "Unsopported";
    }, val, other.val);
}
Value Value::operator* (const Value& other) const { 
    return std::visit([](const auto& lhs, const auto& rhs) -> Value {
        if constexpr (traits::has<decltype(lhs), decltype(rhs)>::mul) return {lhs * rhs};
        throw "Unsopported";
    }, val, other.val);
}
Value Value::operator/ (const Value& other) const { 
    return std::visit([](const auto& lhs, const auto& rhs) -> Value {
        if constexpr (traits::has<decltype(lhs), decltype(rhs)>::div) return {lhs / rhs};
        throw "Unsopported";
    }, val, other.val);
}
Value Value::operator< (const Value& other) const { 
    return std::visit([](const auto& lhs, const auto& rhs) -> Value {
        if constexpr (traits::has<decltype(lhs), decltype(rhs)>::lt) return {lhs < rhs};
        throw "Unsopported";
    }, val, other.val);
}
Value Value::operator<=(const Value& other) const { 
    return std::visit([](const auto& lhs, const auto& rhs) -> Value {
        if constexpr (traits::has<decltype(lhs), decltype(rhs)>::le) return {lhs <= rhs};
        throw "Unsopported";
    }, val, other.val);
}
Value Value::operator> (const Value& other) const { 
    return std::visit([](const auto& lhs, const auto& rhs) -> Value {
        if constexpr (traits::has<decltype(lhs), decltype(rhs)>::gt) return {lhs > rhs};
        throw "Unsopported";
    }, val, other.val);
}
Value Value::operator>=(const Value& other) const { 
    return std::visit([](const auto& lhs, const auto& rhs) -> Value {
        if constexpr (traits::has<decltype(lhs), decltype(rhs)>::ge) return {lhs >= rhs};
        throw "Unsopported";
    }, val, other.val);
}
Value Value::operator!=(const Value& other) const { 
    return std::visit([](const auto& lhs, const auto& rhs) -> Value {
        if constexpr (traits::has<decltype(lhs), decltype(rhs)>::ne) return {lhs != rhs};
        throw "Unsopported";
    }, val, other.val);
}
Value Value::operator==(const Value& other) const { 
    return std::visit([](const auto& lhs, const auto& rhs) -> Value {
        if constexpr (traits::has<decltype(lhs), decltype(rhs)>::eq) return {lhs == rhs};
        throw "Unsopported";
    }, val, other.val);
}

} // namespace Pawka