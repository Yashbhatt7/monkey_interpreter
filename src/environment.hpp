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
    std::shared_ptr<Environment> outer;

    Environment() : outer(nullptr) {}

    Object* Get(const std::string& name);

    Object* Set(const std::string& name, std::unique_ptr<Object> val);
};

std::shared_ptr<Environment> NewEnvironment();
std::shared_ptr<Environment> NewEnclosedEnvironment(std::shared_ptr<Environment> outer);

#endif

