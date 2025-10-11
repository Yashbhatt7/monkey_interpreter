#include "environment.hpp"
#include <memory>

Object* Environment::Get(const std::string& name) {
    auto it = store.find(name);

    if (it != store.end()) {
        return it->second.get();
    }

    if (outer != nullptr) {
        return outer->Get(name);
    }

    return nullptr;
}

Object* Environment::Set(const std::string& name, std::unique_ptr<Object> val) {
    Object* ptr = val.get();
    store[name] = std::move(val);
    return ptr;
}

std::shared_ptr<Environment> NewEnvironment() {
    return std::make_unique<Environment>();
}

std::shared_ptr<Environment> NewEnclosedEnvironment(std::shared_ptr<Environment> outer) {
    auto env = std::make_unique<Environment>();
    env->outer = outer;
    return env;
}

