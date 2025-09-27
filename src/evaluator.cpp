#include "evaluator.hpp"

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
    static Null NULL_NULL_OBJ{};
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
    return std::make_unique<Null>(NULL_NULL_OBJ);
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

