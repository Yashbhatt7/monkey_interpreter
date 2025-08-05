#ifndef PARSER_HPP
#define PARSER_HPP

#include "ast.hpp"
#include "lexer.hpp"
#include "token.hpp"

class Parser {
public:
    std::unique_ptr<Lexer> l;

    Token curToken;
    Token peekToken;
    Token token;

    explicit Parser(std::unique_ptr<Lexer> l);
    std::unique_ptr<Statement> parseStatement();
    std::unique_ptr<Statement> parseLetStatement();
    void nextToken();

    std::unique_ptr<Program> ParseProgram();
};

#endif // PARSER_HPP
