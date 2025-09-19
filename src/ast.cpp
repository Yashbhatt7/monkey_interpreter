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

std::string InfixExpression::TokenLiteral() const {
    return token.literal;
}

std::string BooleanExpression::TokenLiteral() const {
    return token.literal;
}

std::string IfExpression::TokenLiteral() const {
    return token.literal;
}

std::string BlockStatement::TokenLiteral() const {
    return token.literal;
}

std::string FunctionLiteral::TokenLiteral() const {
    return token.literal;
}

std::string CallExpression::TokenLiteral() const {
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

std::string InfixExpression::String() const {
    std::ostringstream out;

    out << "(";
    out << Left->String();
    out << " " << Operator << " ";
    out << Right->String();
    out << ")";

    return out.str();
}

std::string BooleanExpression::String() const {
    return token.literal;
}

std::string IfExpression::String() const {
    std::ostringstream out;

    out << "if";
    out << Condition->String();
    out << " ";
    out << Consequence->String();

    if (Alternative != nullptr) {
        out << "else ";
        out << Alternative->String();
    }

    return out.str();
}

std::string BlockStatement::String() const {
    std::ostringstream out;

    for (const auto& s : Statements) {
        out << s->String();
    }

    return out.str();
}

std::string FunctionLiteral::String() const {
    std::ostringstream out;
    std::vector<std::string> params;

    for (const auto& p : Parameters) {
        params.push_back(p->String());
    }

    out << TokenLiteral();
    out << "(";

    for (size_t i = 0; i < params.size(); ++i) {
        if (i > 0) {
            out << ", ";
        }
        out << params[i];
    }

    out << ")";
    out << Body->String();

    return out.str();
}

std::string CallExpression::String() const {
    std::ostringstream out;
    std::vector<std::string> args;

    for (const auto& a : Arguments) {
        args.push_back(a->String());
    }

    out << Function->String();
    out << "(";

    for (size_t i = 0; i < args.size(); ++i) {
        if (i > 0) {
            out << ", ";
        }
        out << args[i];
    }

    out << ")";

    return out.str();
}

