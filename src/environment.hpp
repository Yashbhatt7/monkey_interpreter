#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include<string>
#include<unordered_map>
#include<memory>
#include "object.hpp"

class Environment {
private:
    std::unordered_map<std::string, std::unique_ptr<Object>> store;
public:
    Environment() = default;

    Object* Get(const std::string& name);

    Object* Set(const std::string& name, std::unique_ptr<Object> val);
};

std::unique_ptr<Environment> NewEnvironment();

#endif

