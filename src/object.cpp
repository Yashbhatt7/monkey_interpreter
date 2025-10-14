#include<sstream>
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

std::string String::Inspect() const {
    return Value;
}

// Builtin implementation
Builtin::Builtin(BuiltinFunction fn) : Fn(fn) {}

ObjectType Builtin::Type() const {
    return BUILTIN_OBJ;
}

std::string Builtin::Inspect() const {
    return "builtin function";
}

// Array implementation
Array::Array(std::vector<std::unique_ptr<Object>> elements) : Elements(std::move(elements)) {}

ObjectType Array::Type() const {
    return ARRAY_OBJ;
}

std::string Array::Inspect() const {
    std::string out = "[";

    for (size_t i = 0; i < Elements.size(); ++i) {
        out += Elements[i]->Inspect();
        if (i < Elements.size() - 1) {
            out += ", ";
        }
    }
    out += "]";

    return out;
}

// FNV-1a hash function
uint64_t fnv1a_hash(const std::string& str) {
    uint64_t hash = 14695981039346656037ULL; // FNV offset basis

    for (char c : str) {
        hash ^= static_cast<uint64_t>(c);
        hash *= 1099511628211ULL; // FNV prime
    }

    return hash;
}

// HashKey methods for Integer, String, Boolean
HashKey Integer::HashKey_() const {
    return {Type(), static_cast<uint64_t>(Value)};
}

HashKey String::HashKey_() const {
    return {Type(), fnv1a_hash(Value)};
}

HashKey Boolean::HashKey_() const {
    uint64_t value = Value ? 1 : 0;
    return {Type(), value};
}

// Hash implementation
Hash::Hash(std::unordered_map<struct HashKey, HashPair> pairs) : Pairs(std::move(pairs)) {}

ObjectType Hash::Type() const {
    return HASH_OBJ;
}

std::string Hash::Inspect() const {
    std::ostringstream out;
    std::vector<std::string> pairs;

    for (const auto& pair : Pairs) {
        pairs.push_back(pair.second.Key->Inspect() + ": " + pair.second.Value->Inspect());
    }

    out << "{";
    for (size_t i = 0; i < pairs.size(); ++i) {
        if (i > 0) {
            out << ", ";
        }
        out << pairs[i];
    }
    out << "}";

    return out.str();
}

