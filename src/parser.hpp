#ifndef PARSER_HPP
#define PARSER_HPP

#include<sstream>
#include<functional>
#include "ast.hpp"
#include "lexer.hpp"
#include "token.hpp"

enum class Precedence {
    LOWEST = 1,    // Start at 1 (skipping the placeholder)
    EQUALS,        // ==
    LESSGREATER,   // > or <
    SUM,           // +
    PRODUCT,       // *
    PREFIX,        // -X or !X
    CALL,          // myFunction(X)
};

using prefixParseFn = std::function<std::unique_ptr<Expression>()>;
using infixParseFn = std::function<std::unique_ptr<Expression>(std::unique_ptr<Expression>)>;

class Parser {
public:
    std::unique_ptr<Lexer> l;

    Token curToken;
    Token peekToken;
    Token token;
    std::vector<std::string> errors;

    std::unordered_map<TokenType, prefixParseFn> prefixParseFns;
    std::unordered_map<TokenType, infixParseFn> infixParseFns;

    explicit Parser(std::unique_ptr<Lexer> lexer);
    std::unique_ptr<Program> ParseProgram();
    std::unique_ptr<Statement> parseStatement();
    std::unique_ptr<LetStatement> parseLetStatement();
    std::unique_ptr<ReturnStatement> parseReturnStatement();
    std::unique_ptr<ExpressionStatement> parseExpressionStatement();
    std::unique_ptr<Expression> parseExpression(int precedence);
    std::unique_ptr<Expression> parseIdentifier();
    std::unique_ptr<Expression> parseIntegerLiteral();
    std::unique_ptr<Expression> parsePrefixExpression();
    std::unique_ptr<Expression> parseInfixExpression(std::unique_ptr<Expression> left);
    void nextToken();
    std::vector<std::string> Errors();
    void noPrefixParseFnError(TokenType t);

    static const std::unordered_map<TokenType, Precedence> precedences;

    int peekPrecedence();
    int curPrecedence();

    bool expectPeek(TokenType t);
    bool curTokenIs(TokenType t);
    bool peekTokenIs(TokenType t);
    void peekErrors(TokenType t);

    void registerPrefix(TokenType tokenType, prefixParseFn);
    void registerInfix(TokenType tokenType, infixParseFn);
};

#endif // PARSER_HPP
