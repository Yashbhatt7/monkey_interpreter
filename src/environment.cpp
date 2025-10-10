#include "environment.hpp"

Object* Environment::Get(const std::string& name) {
    auto it = store.find(name);

    if (it != store.end()) {
        return it->second.get();
    }
    return nullptr;
}

Object* Environment::Set(const std::string& name, std::unique_ptr<Object> val) {
    Object* ptr = val.get();
    store[name] = std::move(val);
    return ptr;
}

std::unique_ptr<Environment> NewEnvironment() {
    return std::make_unique<Environment>();
}

