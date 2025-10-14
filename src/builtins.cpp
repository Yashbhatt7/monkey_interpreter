#include<iostream>
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

    if (auto arr = dynamic_cast<Array*>(args[0].get())) {
        return std::make_unique<Integer>(static_cast<int64_t>(arr->Elements.size()));
    }

    return std::make_unique<Error>(fmt::format("argument to `len` not supported, got {}", args[0]->Type()));
}

std::unique_ptr<Object> builtinFirst(std::vector<std::unique_ptr<Object>> args) {
    if (args.size() != 1) {
        return std::make_unique<Error>(fmt::format("wrong number of arguments. got={}, want=1", args.size()));
    }

    if (args[0]->Type() != ARRAY_OBJ) {
        return std::make_unique<Error>(fmt::format("argument to `first` must be ARRAY, got {}", args[0]->Type()));
    }

    auto arr = static_cast<Array*>(args[0].get());

    if (arr->Elements.size() > 0) {
        Object* elem = arr->Elements[0].get();

        if (auto intElem = dynamic_cast<Integer*>(elem)) {
            return std::make_unique<Integer>(*intElem);
        } else if (auto boolElem = dynamic_cast<Boolean*>(elem)) {
            return std::make_unique<Boolean>(*boolElem);
        } else if (auto strElem = dynamic_cast<String*>(elem)) {
            return std::make_unique<String>(*strElem);
        } else if (auto nullElem = dynamic_cast<Null*>(elem)) {
            return std::make_unique<Null>(*nullElem);
        } else if (auto funcElem = dynamic_cast<Function*>(elem)) {
            return std::make_unique<Function>(funcElem->Parameters, funcElem->Body, funcElem->Env);
        }
    }

    return std::make_unique<Null>();
}

std::unique_ptr<Object> builtinLast(std::vector<std::unique_ptr<Object>> args) {
    if (args.size() != 1) {
        return std::make_unique<Error>(fmt::format("wrong number of arguments. got={}, want=1", args.size()));
    }

    if (args[0]->Type() != ARRAY_OBJ) {
        return std::make_unique<Error>(fmt::format("argument to `last` must be ARRAY, got {}", args[0]->Type()));
    }

    auto arr = static_cast<Array*>(args[0].get());
    size_t length = arr->Elements.size();

    if (length > 0) {
        Object* elem = arr->Elements[length - 1].get();

        if (auto intElem = dynamic_cast<Integer*>(elem)) {
            return std::make_unique<Integer>(*intElem);
        } else if (auto boolElem = dynamic_cast<Boolean*>(elem)) {
            return std::make_unique<Boolean>(*boolElem);
        } else if (auto strElem = dynamic_cast<String*>(elem)) {
            return std::make_unique<String>(*strElem);
        } else if (auto nullElem = dynamic_cast<Null*>(elem)) {
            return std::make_unique<Null>(*nullElem);
        } else if (auto funcElem = dynamic_cast<Function*>(elem)) {
            return std::make_unique<Function>(funcElem->Parameters, funcElem->Body, funcElem->Env);
        }
    }

    return std::make_unique<Null>();
}

std::unique_ptr<Object> builtinRest(std::vector<std::unique_ptr<Object>> args) {
    if (args.size() != 1) {
        return std::make_unique<Error>(fmt::format("wrong number of arguments. got={}, want=1", args.size()));
    }

    if (args[0]->Type() != ARRAY_OBJ) {
        return std::make_unique<Error>(fmt::format("argument to `rest` must be ARRAY, got {}", args[0]->Type()));
    }

    auto arr = static_cast<Array*>(args[0].get());
    size_t length = arr->Elements.size();

    if (length > 0) {
        std::vector<std::unique_ptr<Object>> newElements;

        for (size_t i = 1; i < length; ++i) {
            Object* elem = arr->Elements[i].get();

            if (auto intElem = dynamic_cast<Integer*>(elem)) {
                newElements.push_back(std::make_unique<Integer>(*intElem));
            } else if (auto boolElem = dynamic_cast<Boolean*>(elem)) {
                newElements.push_back(std::make_unique<Boolean>(*boolElem));
            } else if (auto strElem = dynamic_cast<String*>(elem)) {
                newElements.push_back(std::make_unique<String>(*strElem));
            } else if (auto nullElem = dynamic_cast<Null*>(elem)) {
                newElements.push_back(std::make_unique<Null>(*nullElem));
            } else if (auto funcElem = dynamic_cast<Function*>(elem)) {
                newElements.push_back(std::make_unique<Function>(funcElem->Parameters, funcElem->Body, funcElem->Env));
            }
        }

        return std::make_unique<Array>(std::move(newElements));
    }

    return std::make_unique<Null>();
}

std::unique_ptr<Object> builtinPush(std::vector<std::unique_ptr<Object>> args) {
    if (args.size() != 2) {
        return std::make_unique<Error>(fmt::format("wrong number of arguments. got={}, want=2", args.size()));
    }

    if (args[0]->Type() != ARRAY_OBJ) {
        return std::make_unique<Error>(fmt::format("argument to `push` must be ARRAY, got {}", args[0]->Type()));
    }

    auto arr = static_cast<Array*>(args[0].get());
    size_t length = arr->Elements.size();

    std::vector<std::unique_ptr<Object>> newElements;

    for (size_t i = 0; i < length; ++i) {
        Object* elem = arr->Elements[i].get();

        if (auto intElem = dynamic_cast<Integer*>(elem)) {
            newElements.push_back(std::make_unique<Integer>(*intElem));
        } else if (auto boolElem = dynamic_cast<Boolean*>(elem)) {
            newElements.push_back(std::make_unique<Boolean>(*boolElem));
        } else if (auto strElem = dynamic_cast<String*>(elem)) {
            newElements.push_back(std::make_unique<String>(*strElem));
        } else if (auto nullElem = dynamic_cast<Null*>(elem)) {
            newElements.push_back(std::make_unique<Null>(*nullElem));
        } else if (auto funcElem = dynamic_cast<Function*>(elem)) {
            newElements.push_back(std::make_unique<Function>(funcElem->Parameters, funcElem->Body, funcElem->Env));
        }
    }

    newElements.push_back(std::move(args[1]));

    return std::make_unique<Array>(std::move(newElements));
}

std::unique_ptr<Object> builtinPuts(std::vector<std::unique_ptr<Object>> args) {
    for (const auto& arg : args) {
        std::cout << arg->Inspect() << std::endl;
    }

    return std::make_unique<Null>();
}

void initBuiltins() {
    builtins["len"] = std::make_unique<Builtin>(builtinLen);
    builtins["first"] = std::make_unique<Builtin>(builtinFirst);
    builtins["last"] = std::make_unique<Builtin>(builtinLast);
    builtins["rest"] = std::make_unique<Builtin>(builtinRest);
    builtins["push"] = std::make_unique<Builtin>(builtinPush);
    builtins["puts"] = std::make_unique<Builtin>(builtinPuts);
}

Builtin* getBuiltin(const std::string& name) {
    auto it = builtins.find(name);
    if (it != builtins.end()) {
        return it->second.get();
    }
    return nullptr;
}

