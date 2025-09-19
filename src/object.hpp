#ifndef OBJECT_HPP
#define OBJECT_HPP

#include<string>

using ObjectType = std::string;

extern const ObjectType INTEGER_OBJ;
extern const ObjectType BOOLEAN_OBJ;
extern const ObjectType NULL_OBJ;

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

#endif // OBJECT_HPP

