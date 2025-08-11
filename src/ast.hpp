#ifndef AST_HPP
#define AST_HPP

#include<string>
#include<vector>
#include<memory>
#include "token.hpp"

// note: dont forget to use override keyword.

// Base Node interface
class Node {
public:
    virtual std::string TokenLiteral() const = 0;
    virtual ~Node() = default;
};

// two types of Node: expressions and statements
// Statement interface
class Statement : public Node {
public:
    virtual void statementNode() = 0;
};

// Expression interface
class Expression : public Node {
public:
    virtual void expressionNode() = 0;
};

// Program Node will be root Node of every AST our parser produces
class Program : public Node {
public:
    Token curToken;

    std::vector<std::unique_ptr<Statement>> Statements;

    std::string TokenLiteral() const override;
};

class Identifier : public Expression {
public:
    Token token;
    std::string Value;

    void expressionNode() override {}
    std::string TokenLiteral() const override;
};

class LetStatement : public Statement {
public:
    Token token;
    std::unique_ptr<Identifier> Name;
    std::unique_ptr<Expression> Value;

    void statementNode() override {}
    std::string TokenLiteral() const override;
};

class ReturnStatement : public Statement {
public:
    Token token;
    std::unique_ptr<Expression> ReturnValue;

    void statementNode() override {}
    std::string TokenLiteral() const override;
};

class ExpressionStatement : public Statement {
    Token token;
    std::unique_ptr<Expression> Expression;

    void statementNode() override {}
    std::string TokenLiteral() const override;
};

#endif // AST_HPP
