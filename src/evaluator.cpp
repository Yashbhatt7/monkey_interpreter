#include "evaluator.hpp"
#include "ast.hpp"
#include "object.hpp"

std::unique_ptr<Object> evalStatements(const std::vector<std::unique_ptr<Statement>>& stmts) {
    std::unique_ptr<Object> result;
    // std::cout << "will it call evalStatements?\n";

    for (const auto& statement : stmts) {
        // std::cout << "how many times loop ran?\n";
        result = Eval(statement.get());
    }

    // std::cout << "did this come out of loop?\n";
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
        return nativeNullObject();
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

    return nativeNullObject();
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

    return nativeNullObject();
}

std::unique_ptr<Object> evalInfixExpression(const std::string& op, Object* left, Object* right) {
    if (dynamic_cast<Integer*>(left) && dynamic_cast<Integer*>(right)) {
        return evalIntegerInfixExpression(op, left, right);
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

    return nativeNullObject();
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
            // std::cout << "will it be program?\n";
            return evalStatements(program->Statements);
        }

        // Statements
        case NodeType::EXPRESSION_STATEMENT: {
            auto exprStmt = static_cast<ExpressionStatement*>(node);
            // std::cout << "will it be Statement?\n";
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
            // std::cout << "will it be BooleanLiteral?\n";
            auto right = Eval(prefixExpr->Right.get());
            return evalPrefixExpression(prefixExpr->Operator, right.get());
        }
        case NodeType::INFIX_EXPRESSION: {
            auto infixExpr = static_cast<InfixExpression*>(node);
            auto left = Eval(infixExpr->Left.get());
            auto right = Eval(infixExpr->Right.get());
            return evalInfixExpression(infixExpr->Operator, left.get(), right.get());
        }
        case NodeType::BLOCK_STATEMENT: {
            auto blockStmt = static_cast<BlockStatement*>(node);
            return evalStatements(blockStmt->Statements);
        }
        case NodeType::IF_EXPRESSION: {
            auto ifExpr = static_cast<IfExpression*>(node);
            return evalIfExpression(ifExpr);
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

