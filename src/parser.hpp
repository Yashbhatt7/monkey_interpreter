#ifndef PARSER_HPP
#define PARSER_HPP

#include<sstream>
#include<functional>
#include<chrono>
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
    INDEX,         // for array index [1]
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
    std::unique_ptr<Expression> parseBoolean();
    std::unique_ptr<Expression> parseGroupedExpression();
    std::unique_ptr<Expression> parseIfExpression();
    std::unique_ptr<BlockStatement> parseBlockStatement();
    std::unique_ptr<Expression> parseFunctionLiteral();
    std::vector<std::unique_ptr<Identifier>> parseFunctionParameters();
    std::unique_ptr<Expression> parseCallExpression(std::unique_ptr<Expression> left);
    // std::vector<std::unique_ptr<Expression>> parseCallArguments();
    std::unique_ptr<Expression> parseStringLiteral();
    std::unique_ptr<Expression> parseArrayLiteral();
    std::unique_ptr<Expression> parseIndexExpression(std::unique_ptr<Expression> left);
    std::vector<std::unique_ptr<Expression>> parseExpressionList(TokenType end);
    std::unique_ptr<Expression> parseHashLiteral();

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

// Tracer functions
// static int trace_level = 0;
// std::string get_indent();
// void trace(const std::string& name);
// void untrace(const std::string& name);
//
// class Trace {
// private:
//     std::string function_name;
//
// public:
//     explicit Trace(const std::string& name) : function_name(name) {
//         trace(name);
//     }
//
//     ~Trace() {
//         untrace(function_name);
//     }
// };

#endif // PARSER_HPP
