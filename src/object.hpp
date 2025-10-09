#ifndef OBJECT_HPP
#define OBJECT_HPP

#include<cstdint>
#include<string>
#include<memory>

using ObjectType = std::string;

inline const ObjectType INTEGER_OBJ = "INTEGER";
inline const ObjectType BOOLEAN_OBJ = "BOOLEAN";
inline const ObjectType NULL_OBJ    = "NULL";
inline const ObjectType RETURN_VALUE_OBJ  = "RETURN_VALUE";
inline const ObjectType ERROR_OBJ = "ERROR";

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

#endif // OBJECT_HPP

