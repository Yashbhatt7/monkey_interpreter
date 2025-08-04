#include "parser.hpp"

Parser::Parser(Lexer* lexer)
    : l(lexer) {
    nextToken();
    nextToken();
}

void Parser::nextToken() {
    curToken = peekToken;
    peekToken = l->NextToken();
}

std::unique_ptr<Program> Parser::ParseProgram() {
    return nullptr;
}

