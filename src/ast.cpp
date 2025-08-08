#include<iostream>
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
