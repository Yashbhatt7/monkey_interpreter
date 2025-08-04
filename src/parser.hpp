#ifndef PARSER_HPP
#define PARSER_HPP

#include "ast.hpp"
#include "lexer.hpp"
#include "token.hpp"

class Parser {
public:
    Lexer* l;

    Token curToken;
    Token peekToken;

    explicit Parser(Lexer* lexer);
    void nextToken();

    std::unique_ptr<Program> ParseProgram();
};

#endif // PARSER_HPP
