#include<iostream>
#include<sstream>
#include "ast.hpp"

std::string Program::TokenLiteral() const {
    if (!Statements.empty()) {
        return Statements[0]->TokenLiteral();
    } else {
        return "";
    }
}

std::string LetStatement::TokenLiteral() const {
    return token.literal;
}

std::string Identifier::TokenLiteral() const {
    return token.literal;
}

std::string ReturnStatement::TokenLiteral() const {
    return token.literal;
}

std::string ExpressionStatement::TokenLiteral() const {
    return token.literal;
}

std::string IntegerLiteral::TokenLiteral() const {
    return token.literal;
}

std::string PrefixExpression::TokenLiteral() const {
    return token.literal;
}

std::string Program::String() const {
    std::ostringstream out;
    for (const auto& s : Statements) {
        out << s->String();
    }

    return out.str();
}

std::string LetStatement::String() const {
    std::ostringstream out;

    out << TokenLiteral() << " ";
    out << Name->String();
    out << " = ";

    if (Value != nullptr) {
        out << Value->String();
    }

    out << ";";

    return out.str();
}

std::string ReturnStatement::String() const {
    std::ostringstream out;

    out << TokenLiteral() << " ";

    if (ReturnValue != nullptr) {
        out << ReturnValue->String();
    }

    out << ";";

    return out.str();
}

std::string ExpressionStatement::String() const {
    if (Expression != nullptr) {
        return Expression->String();
    }

    return "";
}

std::string Identifier::String() const {
    return Value;
}

std::string IntegerLiteral::String() const {
    return token.literal;
}

std::string PrefixExpression::String() const {
    std::ostringstream out;

    out << "(";
    out << Operator;
    out << Right->String();
    out << ")";

    return out.str();
}

