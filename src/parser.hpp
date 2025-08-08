#ifndef PARSER_HPP
#define PARSER_HPP

#include<sstream>
#include "ast.hpp"
#include "lexer.hpp"
#include "token.hpp"

class Parser {
public:
    std::unique_ptr<Lexer> l;

    Token curToken;
    Token peekToken;
    Token token;
    std::vector<std::string> errors;

    explicit Parser(std::unique_ptr<Lexer> lexer);
    std::unique_ptr<Statement> parseStatement();
    std::unique_ptr<LetStatement> parseLetStatement();
    void nextToken();
    std::vector<std::string> Errors();

    std::unique_ptr<Program> ParseProgram();
    bool expectPeek(TokenType t);
    bool cutTokenIs(TokenType t);
    bool peekTokenIs(TokenType t);
    void peekErrors(TokenType t);
};

#endif // PARSER_HPP
