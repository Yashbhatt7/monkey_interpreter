#ifndef BUILTIN_HPP
#define BUILTIN_HPP

#include<string>
#include<unordered_map>
#include<memory>
#include "object.hpp"

extern std::unordered_map<std::string, std::unique_ptr<Builtin>> builtins;

void initBuiltins();

Builtin* getBuiltin(const std::string& name);

#endif

