#ifndef OBJECT_HPP
#define OBJECT_HPP

#include<vector>
#include<cstdint>
#include<string>
#include<memory>
#include<functional>

class Identifier;
class BlockStatement;
class Environment;

using ObjectType = std::string;

inline const ObjectType INTEGER_OBJ = "INTEGER";
inline const ObjectType BOOLEAN_OBJ = "BOOLEAN";
inline const ObjectType STRING_OBJ = "STRING";
inline const ObjectType NULL_OBJ    = "NULL";
inline const ObjectType RETURN_VALUE_OBJ  = "RETURN_VALUE";
inline const ObjectType ERROR_OBJ = "ERROR";
inline const ObjectType FUNCTION_OBJ = "FUNCTION";
inline const ObjectType BUILTIN_OBJ = "BUILTIN";
inline const ObjectType ARRAY_OBJ = "ARRAY";

class Object {
public:
    virtual ~Object() = default;
    virtual ObjectType Type() const = 0;
    virtual std::string Inspect() const = 0;
};

class Integer : public Object {
public:
    int64_t Value;

    Integer(int64_t value);
    ObjectType Type() const override;
    std::string Inspect() const override;
};

class String : public Object {
public:
    std::string Value;

    String(const std::string& Value);
    ObjectType Type() const override;
    std::string Inspect() const override;
};

class Boolean : public Object {
public:
    bool Value;

    Boolean(bool value);
    ObjectType Type() const override;
    std::string Inspect() const override;
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

#endif // OBJECT_HPP

