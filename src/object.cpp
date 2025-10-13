#include "object.hpp"
#include "ast.hpp"

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

// ReturnValue implementation
ReturnValue::ReturnValue(std::unique_ptr<Object> value) : Value(std::move(value)) {}

ObjectType ReturnValue::Type() const {
    return RETURN_VALUE_OBJ;
}

std::string ReturnValue::Inspect() const {
    return Value->Inspect();
}

// Error implementation
Error::Error(const std::string& message) : Message(message) {}

ObjectType Error::Type() const {
    return ERROR_OBJ;
}

std::string Error::Inspect() const {
    return "Error: " + Message;
}

// Function implementation
Function::Function(std::vector<std::string> params, BlockStatement* body, std::shared_ptr<Environment> env)
    : Parameters(std::move(params)), Body(body), Env(env) {}

ObjectType Function::Type() const {
    return FUNCTION_OBJ;
}

std::string Function::Inspect() const {
    std::string out = "fn(";

    for (size_t i = 0; i < Parameters.size(); ++i) {
        out += Parameters[i];
        if (i < Parameters.size() - 1) {
            out += ", ";
        }
    }

    out += ") {\n";
    out += Body->String();
    out += "\n}";

    return out;
}

// String implementation
String::String(const std::string& value) : Value(value) {}

ObjectType String::Type() const {
    return STRING_OBJ;
}

ObjectType String::Inspect() const {
    return Value;
}

