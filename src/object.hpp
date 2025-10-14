#ifndef OBJECT_HPP
#define OBJECT_HPP

#include<vector>
#include<cstdint>
#include<string>
#include<memory>
#include<unordered_map>
#include<functional>

class Identifier;
class BlockStatement;
class Environment;

using ObjectType = std::string;

inline const ObjectType INTEGER_OBJ      = "INTEGER";
inline const ObjectType BOOLEAN_OBJ      = "BOOLEAN";
inline const ObjectType STRING_OBJ       = "STRING";
inline const ObjectType NULL_OBJ         = "NULL";
inline const ObjectType RETURN_VALUE_OBJ = "RETURN_VALUE";
inline const ObjectType ERROR_OBJ        = "ERROR";
inline const ObjectType FUNCTION_OBJ     = "FUNCTION";
inline const ObjectType BUILTIN_OBJ      = "BUILTIN";
inline const ObjectType ARRAY_OBJ        = "ARRAY";
inline const ObjectType HASH_OBJ         = "HASH";

struct HashKey {
    ObjectType Type;
    uint64_t Value;

    bool operator==(const HashKey& other) const {
        return Type == other.Type && Value == other.Value;
    }
};

namespace std {
    template<>
    struct hash<HashKey> {
        size_t operator()(const HashKey& k) const {
            return hash<string>()(k.Type) ^ hash<uint64_t>()(k.Value);
        }
    };
}

class Object {
public:
    virtual ~Object() = default;
    virtual ObjectType Type() const = 0;
    virtual std::string Inspect() const = 0;
};

class Hashable {
public:
    virtual HashKey HashKey_() const = 0;
    virtual ~Hashable() = default;
};

class Integer : public Object, public Hashable {
public:
    int64_t Value;

    Integer(int64_t value);
    ObjectType Type() const override;
    std::string Inspect() const override;
    HashKey HashKey_() const override;
};

class String : public Object, public Hashable {
public:
    std::string Value;

    String(const std::string& Value);
    ObjectType Type() const override;
    std::string Inspect() const override;
    HashKey HashKey_() const override;
};

class Boolean : public Object, public Hashable {
public:
    bool Value;

    Boolean(bool value);
    ObjectType Type() const override;
    std::string Inspect() const override;
    HashKey HashKey_() const override;
};

class Null : public Object {
public:
    Null() = default;
    ObjectType Type() const override;
    std::string Inspect() const override;
};

class ReturnValue : public Object {
public:
    std::unique_ptr<Object> Value;

    ReturnValue(std::unique_ptr<Object> value);
    ObjectType Type() const override;
    std::string Inspect() const override;
};

class Error : public Object {
public:
    std::string Message;

    Error(const std::string& Message);
    ObjectType Type() const override;
    std::string Inspect() const override;
};

class Function : public Object {
public:
    std::vector<std::string> Parameters;
    BlockStatement* Body;
    std::shared_ptr<Environment> Env;

    Function(std::vector<std::string> params, BlockStatement* body, std::shared_ptr<Environment> env);
    ObjectType Type() const override;
    std::string Inspect() const override;
};

// Built-in function type
using BuiltinFunction = std::function<std::unique_ptr<Object>(std::vector<std::unique_ptr<Object>>)>;

class Builtin : public Object {
public:
    BuiltinFunction Fn;

    Builtin(BuiltinFunction fn);
    ObjectType Type() const override;
    std::string Inspect() const override;
};

class Array : public Object {
public:
    std::vector<std::unique_ptr<Object>> Elements;

    Array() = default;
    Array(std::vector<std::unique_ptr<Object>> elements);
    ObjectType Type() const override;
    std::string Inspect() const override;
};

struct HashPair {
    std::unique_ptr<Object> Key;
    std::unique_ptr<Object> Value;

    HashPair() = default;
    HashPair(std::unique_ptr<Object> k, std::unique_ptr<Object> v) :Key(std::move(k)), Value(std::move(v)) {}
};

class Hash : public Object {
public:
    std::unordered_map<struct HashKey, HashPair> Pairs;

    Hash() = default;
    Hash(std::unordered_map<struct HashKey, HashPair> pairs);
    ObjectType Type() const override;
    std::string Inspect() const override;
};

#endif // OBJECT_HPP

