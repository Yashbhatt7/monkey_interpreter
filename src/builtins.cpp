#include<fmt/format.h>
#include "builtins.hpp"

std::unordered_map<std::string, std::unique_ptr<Builtin>> builtins;

std::unique_ptr<Object> builtinLen(std::vector<std::unique_ptr<Object>> args) {
    if (args.size() != 1) {
        return std::make_unique<Error>(fmt::format("wrong number of arguments. got={}, want=1", args.size()));
    }

    if (auto str = dynamic_cast<String*>(args[0].get())) {
        return std::make_unique<Integer>(static_cast<int64_t>(str->Value.length()));
    }

    return std::make_unique<Error>(fmt::format("argument to 'len' not supported, got {}", args[0]->Type()));
}

void initBuiltins() {
    builtins["len"] = std::make_unique<Builtin>(builtinLen);
}

Builtin* getBuiltin(const std::string& name) {
    auto it = builtins.find(name);
    if (it != builtins.end()) {
        return it->second.get();
    }
    return nullptr;
}

