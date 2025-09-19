#include "object.hpp"

// Integer implementation
Integer::Integer(int64_t value) : Value(value) {}

ObjectType Integer::Type() const {
    return INTEGER_OBJ;
}

std::string Integer::Inspect() const {
    return std::to_string(Value);
}

// Boolean implementation
Boolean::Boolean(bool value) : Value(value) {}

ObjectType Boolean::Type() const {
    return BOOLEAN_OBJ;
}

std::string Boolean::Inspect() const {
    return Value ? "true" : "false";
}

// Null implementation
ObjectType Null::Type() const {
    return NULL_OBJ;
}

std::string Null::Inspect() const {
    return "null";
}

