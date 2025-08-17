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
    virtual std::string String() const = 0;
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
    std::string String() const override;
};

class Identifier : public Expression {
public:
    Token token;
    std::string Value;

    void expressionNode() override {}
    std::string TokenLiteral() const override;
    std::string String() const override;
};

class LetStatement : public Statement {
public:
    Token token;
    std::unique_ptr<Identifier> Name;
    std::unique_ptr<Expression> Value;

    void statementNode() override {}
    std::string TokenLiteral() const override;
    std::string String() const override;
};

class ReturnStatement : public Statement {
public:
    Token token;
    std::unique_ptr<Expression> ReturnValue;

    void statementNode() override {}
    std::string TokenLiteral() const override;
    std::string String() const override;
};

class ExpressionStatement : public Statement {
public:
    Token token;
    std::unique_ptr<Expression> Expression;

    void statementNode() override {}
    std::string TokenLiteral() const override;
    std::string String() const override;
};

class IntegerLiteral : public Expression {
public:
    Token token;
    int64_t Value;

    void expressionNode() override {}
    std::string TokenLiteral() const override;
    std::string String() const override;
};

class PrefixExpression : public Expression {
public:
    Token token; // The prefix token, e.g. !
    std::string Operator;

    // The "Right" field contains the expression to the right of the "Operator"
    std::unique_ptr<Expression> Right;

    void expressionNode() override {}
    std::string TokenLiteral() const override;
    std::string String() const override;
};

class InfixExpression : public Expression {
public:
    Token token; // The operator token, e.g. +
    // The "Left" field, which can hold any expression
    std::unique_ptr<Expression> Left;
    std::string Operator;
    // The "Right" field contains the expression to the right of the "Operator"
    std::unique_ptr<Expression> Right;

    void expressionNode() override {}
    std::string TokenLiteral() const override;
    std::string String() const override;
};

#endif // AST_HPP
