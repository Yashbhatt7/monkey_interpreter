#include<fmt/format.h>
#include "evaluator.hpp"
#include "ast.hpp"
#include "object.hpp"
#include "environment.hpp"
#include "builtins.hpp"

template<typename... Args>
std::unique_ptr<Error> newError(fmt::format_string<Args...> format, Args&&... args) {
    return std::make_unique<Error>(fmt::format(format, std::forward<Args>(args)...));
}

bool isError(Object* obj) {
    if (obj != nullptr) {
        return obj->Type() == ERROR_OBJ;
    }
    // std::cout << "what it will return false?\n";
    return false;
}

std::unique_ptr<Object> evalProgram(const std::vector<std::unique_ptr<Statement>>& stmts, std::shared_ptr<Environment> env) {
    std::unique_ptr<Object> result;
    // std::cout << "will it call evalProgram?\n";

    for (const auto& statement : stmts) {
        // std::cout << "how many times loop ran?\n";
        result = Eval(statement.get(), env);

        if (auto returnValue = dynamic_cast<ReturnValue*>(result.get())) {
            // auto get = dynamic_cast<Integer*>(returnValue->Value.get());
            // std::cout << "\n\n\nwhat we got here: " << get->Value << "\n\n\n";
            return std::move(returnValue->Value);
        }

        if (auto returnValue = dynamic_cast<Error*>(result.get())) {
            return result;
        }
    }

    // std::cout << "did this come out of loop?\n";
    return result;
}

std::unique_ptr<Object> evalBlockStatement(const std::vector<std::unique_ptr<Statement>>& stmts, std::shared_ptr<Environment> env) {
    std::unique_ptr<Object> result;

    for (const auto& statement : stmts) {
        result = Eval(statement.get(), env);

        if (result) {
            ObjectType rt = result->Type();
            if (rt == RETURN_VALUE_OBJ || rt == ERROR_OBJ) {
                return result;
            }
        }
    }

    return result;
}

// Reuse static Boolean instances instead of allocating new objects each time
// Note: understand singltons this(starting from here to the function nativeNullObject) is not fully singlton design...
// here singlton can be done with either raw pointers or shared_ptr<>.
namespace {
    static Null MONKEY_NULL{};
    static Boolean TRUE_OBJ{true};
    static Boolean FALSE_OBJ{false};
}

std::unique_ptr<Boolean> nativeBoolToBooleanObject(bool input) {
    if (input) {
        return std::make_unique<Boolean>(TRUE_OBJ);
    }
    return std::make_unique<Boolean>(FALSE_OBJ);
}

std::unique_ptr<Null> nativeNullObject() {
    return std::make_unique<Null>(MONKEY_NULL);
}

std::unique_ptr<Object> evalBangOperatorExpression(Object* right) {
    if (auto boolObj = dynamic_cast<Boolean*>(right)) {
        if (boolObj->Value == true) {
            return nativeBoolToBooleanObject(false);
        } else {
            return nativeBoolToBooleanObject(true);
        }
    }

    if (dynamic_cast<Null*>(right)) {
        return nativeBoolToBooleanObject(true);
    }

    return nativeBoolToBooleanObject(false);
}

std::unique_ptr<Object> evalMinusPrefixOperatorExpression(Object* right) {
    auto intObj = dynamic_cast<Integer*>(right);
    if (!intObj) {
        return newError("unknown operator: -{}", right->Type().c_str());
    }

    int64_t value = intObj->Value;
    return std::make_unique<Integer>(-value);
}

std::unique_ptr<Object> evalPrefixExpression(const std::string& op, Object* right) {
    if (op == "!") {
        return evalBangOperatorExpression(right);
    } else if (op == "-") {
        return evalMinusPrefixOperatorExpression(right);
    }

    return newError("unknown operator: {}{}", op.c_str(), right->Type().c_str());
}

std::unique_ptr<Object> evalIntegerInfixExpression(const std::string& op, Object* left, Object* right) {
    auto leftInt = static_cast<Integer*>(left);
    auto rightInt = static_cast<Integer*>(right);

    int64_t leftVal = leftInt->Value;
    int64_t rightVal = rightInt->Value;

    if (op == "+") {
        return std::make_unique<Integer>(leftVal + rightVal);
    } else if (op == "-") {
        return std::make_unique<Integer>(leftVal - rightVal);
    } else if (op == "*") {
        return std::make_unique<Integer>(leftVal * rightVal);
    } else if (op == "/") {
        return std::make_unique<Integer>(leftVal / rightVal);
    } else if (op == "<") {
        return nativeBoolToBooleanObject(leftVal < rightVal);
    } else if (op == ">") {
        return nativeBoolToBooleanObject(leftVal > rightVal);
    } else if (op == "==") {
        return nativeBoolToBooleanObject(leftVal == rightVal);
    } else if (op == "!=") {
        return nativeBoolToBooleanObject(leftVal != rightVal);
    }

    return newError("unknown operator: {} {} {}", left->Type().c_str(), op.c_str(), right->Type().c_str());
}

std::unique_ptr<Object> evalStringInfixExpression(const std::string& op, Object* left, Object* right) {
    if (op != "+") {
        return newError("unknown operator: {} {} {}", left->Type().c_str(), op.c_str(), right->Type().c_str());
    }

    auto leftStr = static_cast<String*>(left);
    auto rightStr = static_cast<String*>(right);

    std::string leftVal = leftStr->Value;
    std::string rightVal = rightStr->Value;

    return std::make_unique<String>(leftVal + rightVal);
}

std::unique_ptr<Object> evalInfixExpression(const std::string& op, Object* left, Object* right) {
    if (dynamic_cast<Integer*>(left) && dynamic_cast<Integer*>(right)) {
        return evalIntegerInfixExpression(op, left, right);
    }

    if (dynamic_cast<String*>(left) && dynamic_cast<String*>(right)) {
        return evalStringInfixExpression(op, left, right);
    }

    if (left->Type() != right->Type()) {
        return newError("type mismatch: {} {} {}", left->Type().c_str(), op.c_str(), right->Type().c_str());
    }

    if (op == "==") {
        auto leftBool = dynamic_cast<Boolean*>(left);
        auto rightBool = dynamic_cast<Boolean*>(right);
        if (leftBool && rightBool) {
            return nativeBoolToBooleanObject(leftBool->Value == rightBool->Value);
        }
        return nativeBoolToBooleanObject(false);
    }

    if (op == "!=") {
        auto leftBool = dynamic_cast<Boolean*>(left);
        auto rightBool = dynamic_cast<Boolean*>(right);
        if (leftBool && rightBool) {
            return nativeBoolToBooleanObject(leftBool->Value != rightBool->Value);
        }
        return nativeBoolToBooleanObject(true);
    }

    return newError("unknown operator: {} {} {}", left->Type().c_str(), op.c_str(), right->Type().c_str());
}

bool isTruthy(Object* obj) {
    if (dynamic_cast<Null*>(obj)) {
        return false;
    }

    if (auto boolObj = dynamic_cast<Boolean*>(obj)) {
        return boolObj->Value;
    }
    return true;
}

std::unique_ptr<Object> evalIfExpression(IfExpression* ie, std::shared_ptr<Environment> env) {
    auto condition = Eval(ie->Condition.get(), env);

    if (isError(condition.get())) {
        return condition;
    }

    if (isTruthy(condition.get())) {
        return Eval(ie->Consequence.get(), env);
    } else if (ie->Alternative != nullptr) {
        return Eval(ie->Alternative.get(), env);
    } else {
        return nativeNullObject();
    }
}

std::unique_ptr<Object> evalIdentifier(Identifier* ident, std::shared_ptr<Environment> env) {
    Object* val = env->Get(ident->Value);

    if (val) {
        if (auto intObj = dynamic_cast<Integer*>(val)) {
            return std::make_unique<Integer>(*intObj);
        } else if (auto boolObj = dynamic_cast<Boolean*>(val)) {
            return std::make_unique<Boolean>(*boolObj);
        } else if (auto nullObj = dynamic_cast<Null*>(val)) {
            return std::make_unique<Null>();
        } else if (auto funcObj = dynamic_cast<Function*>(val)) {
            return std::make_unique<Function>(funcObj->Parameters, funcObj->Body, funcObj->Env);
        } else if (auto strObj = dynamic_cast<String*>(val)) {
            return std::make_unique<String>(*strObj);
        } else if (auto builtinObj = dynamic_cast<Builtin*>(val)) {
            return std::make_unique<Builtin>(*builtinObj);
        } else if (auto arrayObj = dynamic_cast<Array*>(val)) {
            std::vector<std::unique_ptr<Object>> newElements;
            for (const auto& elem : arrayObj->Elements) {
                if (auto intElem = dynamic_cast<Integer*>(elem.get())) {
                    newElements.push_back(std::make_unique<Integer>(*intElem));
                } else if (auto boolElem = dynamic_cast<Boolean*>(elem.get())) {
                    newElements.push_back(std::make_unique<Boolean>(*boolElem));
                } else if (auto strElem = dynamic_cast<String*>(elem.get())) {
                    newElements.push_back(std::make_unique<String>(*strElem));
                }
            }
            return std::make_unique<Array>(std::move(newElements));
        }
    }

    if (Builtin* builtin = getBuiltin(ident->Value)) {
        return std::make_unique<Builtin>(builtin->Fn);
    }

    return newError("identifier not found: {}", ident->Value);
}

std::vector<std::unique_ptr<Object>> evalExpressions(const std::vector<std::unique_ptr<Expression>>& exps, std::shared_ptr<Environment> env) {
    std::vector<std::unique_ptr<Object>> result;

    for (const auto& e : exps) {
        auto evaluated = Eval(e.get(), env);
        if (isError(evaluated.get())) {
            std::vector<std::unique_ptr<Object>> errorResult;
            errorResult.push_back(std::move(evaluated));
            return errorResult;
        }
        result.push_back(std::move(evaluated));
    }

    return result;
}

std::shared_ptr<Environment> extendFunctionEnv(Function* fn, std::vector<std::unique_ptr<Object>> args) {
    auto env = NewEnclosedEnvironment(fn->Env);

    for (size_t paramIdx = 0; paramIdx < fn->Parameters.size(); ++paramIdx) {
        env->Set(fn->Parameters[paramIdx], std::move(args[paramIdx]));
    }

    return env;
}

std::unique_ptr<Object> unwrapReturnValue(std::unique_ptr<Object> obj) {
    if (auto returnValue = dynamic_cast<ReturnValue*>(obj.get())) {
        return std::move(returnValue->Value);
    }

    return obj;
}

std::unique_ptr<Object> applyFunction(std::unique_ptr<Object> fn, std::vector<std::unique_ptr<Object>> args) {
    if (auto function = dynamic_cast<Function*>(fn.get())) {
        if (function->Parameters.size() != args.size()) {
            return newError("wrong number of arguments: expected {}, got {}", function->Parameters.size(), args.size());
        }

        auto extendedEnv = extendFunctionEnv(function, std::move(args));
        auto evaluated = Eval(function->Body, extendedEnv);
        return unwrapReturnValue(std::move(evaluated));
    }

    if (auto builtin = dynamic_cast<Builtin*>(fn.get())) {
        return builtin->Fn(std::move(args));
    }

    return newError("not a function: {}", fn->Type());
}

std::unique_ptr<Object> evalArrayIndexExpression(Object* array, Object* index) {
    auto arrayObject = static_cast<Array*>(array);
    auto idx = static_cast<Integer*>(index)->Value;
    int64_t max = static_cast<int64_t>(arrayObject->Elements.size()) - 1;

    if (idx < 0 || idx > max) {
        return nativeNullObject();
    }

    Object* elem = arrayObject->Elements[idx].get();
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

    return nativeNullObject();
}

std::unique_ptr<Object> evalIndexExpression(Object* left, Object* index) {
    if (left->Type() == "ARRAY" && index->Type() == "INTEGER") {
        return evalArrayIndexExpression(left, index);
    }

    return newError("index operator not supported: {}", left->Type().c_str());
}

std::unique_ptr<Object> Eval(Node* node, std::shared_ptr<Environment> env) {
    switch (node->Type()) {
        // Program
        case NodeType::PROGRAM: {
            auto program = static_cast<Program*>(node);
            // std::cout << "Program?\n";
            return evalProgram(program->Statements, env);
        }

        // Statements
        case NodeType::EXPRESSION_STATEMENT: {
            auto exprStmt = static_cast<ExpressionStatement*>(node);
            // std::cout << "expression Statement?\n";
            return Eval(exprStmt->ExpressionPtr.get(), env);
        }

        // Expressions
        case NodeType::INTEGER_LITERAL: {
            auto intLiteral = static_cast<IntegerLiteral*>(node);
            // std::cout << "will it be IntegerLiteral?\n";
            return std::make_unique<Integer>(intLiteral->Value);
        }
        case NodeType::STRING_LITERAL: {
            auto stringLiteral = static_cast<StringLiteral*>(node);
            return std::make_unique<String>(stringLiteral->Value);
        }
        case NodeType::BOOLEAN_LITERAL: {
            auto boolLiteral = static_cast<BooleanLiteral*>(node);
            // std::cout << "will it be BooleanLiteral?\n";
            return nativeBoolToBooleanObject(boolLiteral->Value);
        }
        case NodeType::PREFIX_EXPRESSION: {
            auto prefixExpr = static_cast<PrefixExpression*>(node);
            auto right = Eval(prefixExpr->Right.get(), env);

            if (isError(right.get())) {
                return right;
            }
            // std::cout << "will it be BooleanLiteral?\n";
            return evalPrefixExpression(prefixExpr->Operator, right.get());
        }
        case NodeType::INFIX_EXPRESSION: {
            auto infixExpr = static_cast<InfixExpression*>(node);
            // std::cout << "will it be InfixExpression?\n";
            auto left = Eval(infixExpr->Left.get(), env);
            if (isError(left.get())) {
                return left;
            }

            auto right = Eval(infixExpr->Right.get(), env);
            if (isError(right.get())) {
                return right;
            }

            return evalInfixExpression(infixExpr->Operator, left.get(), right.get());
        }
        case NodeType::BLOCK_STATEMENT: {
            auto blockStmt = static_cast<BlockStatement*>(node);
            // std::cout << "will it be BlockStatement?\n";
            return evalBlockStatement(blockStmt->Statements, env);
        }
        case NodeType::IF_EXPRESSION: {
            auto ifExpr = static_cast<IfExpression*>(node);
            // std::cout << "will it be IFExpression?\n";
            return evalIfExpression(ifExpr, env);
        }
        case NodeType::RETURN_STATEMENT: {
            auto returnStmt = static_cast<ReturnStatement*>(node);
            // std::cout << "will it be ReturnStatement?\n";
            auto val = Eval(returnStmt->ReturnValue.get(), env);

            if (isError(val.get())) {
                return val;
            }
            return std::make_unique<ReturnValue>(std::move(val));
        }
        case NodeType::LET_STATEMENT: {
            auto letStmt = static_cast<LetStatement*>(node);
            // std::cout << "will it be LetStatement?\n";
            auto val = Eval(letStmt->Value.get(), env);

            if (isError(val.get())) {
                // std::cout << "isError?\n";
                return val;
            }
            // std::cout << "calling set?\n";

            env->Set(letStmt->Name->Value, std::move(val));
            return nativeNullObject();
        }
        case NodeType::IDENTIFIER: {
            auto ident = static_cast<Identifier*>(node);
            // std::cout << "will it be Identifier?\n";
            return evalIdentifier(ident, env);
        }
        case NodeType::FUNCTION_LITERAL: {
            auto funcLit = static_cast<FunctionLiteral*>(node);
            // std::cout << "will it be FunctionLiteral?\n";

            std::vector<std::string> params;
            for (const auto& p : funcLit->Parameters) {
                params.push_back(p->Value);
            }

            BlockStatement* body = funcLit->Body.get();

            return std::make_unique<Function>(params, body, env);
        }
        case NodeType::CALL_EXPRESSION: {
            auto callExpr = static_cast<CallExpression*>(node);
            // std::cout << "will it be CallExpression?\n";

            auto function = Eval(callExpr->Function.get(), env);

            if (isError(function.get())) {
                return function;
            }

            auto args = evalExpressions(callExpr->Arguments, env);
            if (args.size() == 1 && isError(args[0].get())) {
                return std::move(args[0]);
            }

            return applyFunction(std::move(function), std::move(args));
        }
        case NodeType::ARRAY_LITERAL: {
            auto arrayLit = static_cast<ArrayLiteral*>(node);
            auto elements = evalExpressions(arrayLit->Elements, env);

            if (elements.size() == 1 && isError(elements[0].get())) {
                return std::move(elements[0]);
            }

            return std::make_unique<Array>(std::move(elements));
        }
        case NodeType::INDEX_EXPRESSION: {
            auto indexExpr = static_cast<IndexExpression*>(node);
            auto left = Eval(indexExpr->Left.get(), env);

            if (isError(left.get())) {
                return left;
            }

            auto index = Eval(indexExpr->Index.get(), env);

            if (isError(index.get())) {
                return index;
            }

            return evalIndexExpression(left.get(), index.get());
        }
    }

    return nativeNullObject();
}

