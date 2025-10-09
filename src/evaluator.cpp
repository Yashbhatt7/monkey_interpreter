#include<fmt/format.h>
#include "evaluator.hpp"
#include "ast.hpp"
#include "object.hpp"

template<typename... Args>
std::unique_ptr<Error> newError(fmt::format_string<Args...> format, Args&&... args) {
    return std::make_unique<Error>(fmt::format(format, std::forward<Args>(args)...));
}

bool isError(Object* obj) {
    if (obj != nullptr) {
        return obj->Type() == ERROR_OBJ;
    }
    return false;
}

std::unique_ptr<Object> evalProgram(const std::vector<std::unique_ptr<Statement>>& stmts) {
    std::unique_ptr<Object> result;
    // std::cout << "will it call evalProgram?\n";

    for (const auto& statement : stmts) {
        // std::cout << "how many times loop ran?\n";
        result = Eval(statement.get());

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

std::unique_ptr<Object> evalBlockStatement(const std::vector<std::unique_ptr<Statement>>& stmts) {
    std::unique_ptr<Object> result;

    for (const auto& statement : stmts) {
        result = Eval(statement.get());

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

std::unique_ptr<Object> evalInfixExpression(const std::string& op, Object* left, Object* right) {
    if (dynamic_cast<Integer*>(left) && dynamic_cast<Integer*>(right)) {
        return evalIntegerInfixExpression(op, left, right);
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

std::unique_ptr<Object> evalIfExpression(IfExpression* ie) {
    auto condition = Eval(ie->Condition.get());

    if (isError(condition.get())) {
        return condition;
    }

    if (isTruthy(condition.get())) {
        return Eval(ie->Consequence.get());
    } else if (ie->Alternative != nullptr) {
        return Eval(ie->Alternative.get());
    } else {
        return nativeNullObject();
    }
}

// Experiment (till now enum design is working)
std::unique_ptr<Object> Eval(Node* node) {
    switch (node->Type()) {
        // Program
        case NodeType::PROGRAM: {
            auto program = static_cast<Program*>(node);
            // std::cout << "Program?\n";
            return evalProgram(program->Statements);
        }

        // Statements
        case NodeType::EXPRESSION_STATEMENT: {
            auto exprStmt = static_cast<ExpressionStatement*>(node);
            // std::cout << "expression Statement?\n";
            return Eval(exprStmt->ExpressionPtr.get());
        }

        // Expressions
        case NodeType::INTEGER_LITERAL: {
            auto intLiteral = static_cast<IntegerLiteral*>(node);
            // std::cout << "will it be IntegerLiteral?\n";
            return std::make_unique<Integer>(intLiteral->Value);
        }
        case NodeType::BOOLEAN_LITERAL: {
            auto boolLiteral = static_cast<BooleanLiteral*>(node);
            // std::cout << "will it be BooleanLiteral?\n";
            return nativeBoolToBooleanObject(boolLiteral->Value);
        }
        case NodeType::PREFIX_EXPRESSION: {
            auto prefixExpr = static_cast<PrefixExpression*>(node);
            auto right = Eval(prefixExpr->Right.get());

            if (isError(right.get())) {
                return right;
            }
            // std::cout << "will it be BooleanLiteral?\n";
            return evalPrefixExpression(prefixExpr->Operator, right.get());
        }
        case NodeType::INFIX_EXPRESSION: {
            auto infixExpr = static_cast<InfixExpression*>(node);
            // std::cout << "will it be InfixExpression?\n";
            auto left = Eval(infixExpr->Left.get());
            if (isError(left.get())) {
                return left;
            }

            auto right = Eval(infixExpr->Right.get());
            if (isError(right.get())) {
                return right;
            }

            return evalInfixExpression(infixExpr->Operator, left.get(), right.get());
        }
        case NodeType::BLOCK_STATEMENT: {
            auto blockStmt = static_cast<BlockStatement*>(node);
            // std::cout << "will it be BlockStatement?\n";
            return evalBlockStatement(blockStmt->Statements);
        }
        case NodeType::IF_EXPRESSION: {
            auto ifExpr = static_cast<IfExpression*>(node);
            // std::cout << "will it be IFExpression?\n";
            return evalIfExpression(ifExpr);
        }
        case NodeType::RETURN_STATEMENT: {
            auto returnStmt = static_cast<ReturnStatement*>(node);
            auto val = Eval(returnStmt->ReturnValue.get());

            if (isError(val.get())) {
                return val;
            }
            // std::cout << "will it be ReturnStatement?\n";
            return std::make_unique<ReturnValue>(std::move(val));
        }
    }

    return nativeNullObject();
}



// std::unique_ptr<Object> Eval(Node* node) {
//     // Program
//     if (auto program = dynamic_cast<Program*>(node)) {
//         return evalStatements(program->Statements);
//     }
//
//     // Statements
//     if (auto exprStmt = dynamic_cast<ExpressionStatement*>(node)) {
//         return Eval(exprStmt->Expression.get());
//     }
//
//     // Expressions
//     if (auto intLiteral = dynamic_cast<IntegerLiteral*>(node)) {
//         return std::make_unique<Integer>(intLiteral->Value);
//     } else if (auto boolLiteral = dynamic_cast<BooleanLiteral*>(node)) {
//         return std::make_unique<Boolean>(boolLiteral->Value);
//     }
//
//     return nullptr;
// }

