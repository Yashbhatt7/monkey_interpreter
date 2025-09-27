#ifndef AST_HPP
#define AST_HPP

#include<string>
#include<vector>
#include<memory>
#include "token.hpp"

enum class NodeType {
    PROGRAM,
    LET_STATEMENT,
    RETURN_STATEMENT,
    EXPRESSION_STATEMENT,
    BLOCK_STATEMENT,
    IDENTIFIER,
    INTEGER_LITERAL,
    BOOLEAN_LITERAL,
    PREFIX_EXPRESSION,
    INFIX_EXPRESSION,
    IF_EXPRESSION,
    FUNCTION_LITERAL,
    CALL_EXPRESSION,
};

// Base Node interface
class Node {
public:
    virtual std::string TokenLiteral() const = 0;
    virtual std::string String() const = 0;
    virtual ~Node() = default;
    virtual NodeType Type() const = 0;
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
    NodeType Type() const override { return NodeType::PROGRAM; };
};

class Identifier : public Expression {
public:
    Token token;
    std::string Value;

    void expressionNode() override {}
    std::string TokenLiteral() const override;
    std::string String() const override;
    NodeType Type() const override { return NodeType::IDENTIFIER; };
};

class LetStatement : public Statement {
public:
    Token token;
    std::unique_ptr<Identifier> Name;
    std::unique_ptr<Expression> Value;

    void statementNode() override {}
    std::string TokenLiteral() const override;
    std::string String() const override;
    NodeType Type() const override { return NodeType::LET_STATEMENT; };
};

class ReturnStatement : public Statement {
public:
    Token token;
    std::unique_ptr<Expression> ReturnValue;

    void statementNode() override {}
    std::string TokenLiteral() const override;
    std::string String() const override;
    NodeType Type() const override { return NodeType::RETURN_STATEMENT; };
};

class ExpressionStatement : public Statement {
public:
    Token token;
    std::unique_ptr<Expression> ExpressionPtr;

    void statementNode() override {}
    std::string TokenLiteral() const override;
    std::string String() const override;
    NodeType Type() const override { return NodeType::EXPRESSION_STATEMENT; };
};

class IntegerLiteral : public Expression {
public:
    Token token;
    int64_t Value;

    void expressionNode() override {}
    std::string TokenLiteral() const override;
    std::string String() const override;
    NodeType Type() const override { return NodeType::INTEGER_LITERAL; };
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
    NodeType Type() const override { return NodeType::PREFIX_EXPRESSION; };
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
    NodeType Type() const override { return NodeType::INFIX_EXPRESSION; };
};

class BooleanLiteral : public Expression {
public:
    Token token;
    bool Value;

    void expressionNode() override {}
    std::string TokenLiteral() const override;
    std::string String() const override;
    NodeType Type() const override { return NodeType::BOOLEAN_LITERAL; };
};

class BlockStatement : public Statement {
public:
    Token token;
    std::vector<std::unique_ptr<Statement>> Statements;

    void statementNode() override {}
    std::string TokenLiteral() const override;
    std::string String() const override;
    NodeType Type() const override { return NodeType::BLOCK_STATEMENT; };
};

class IfExpression : public Expression {
public:
    Token token;
    std::unique_ptr<Expression> Condition;
    std::unique_ptr<BlockStatement> Consequence;
    std::unique_ptr<BlockStatement> Alternative;

    void expressionNode() override {}
    std::string TokenLiteral() const override;
    std::string String() const override;
    NodeType Type() const override { return NodeType::IF_EXPRESSION; };
};

class FunctionLiteral : public Expression {
public:
    Token token;
    std::vector<std::unique_ptr<Identifier>> Parameters;
    std::unique_ptr<BlockStatement> Body;

    void expressionNode() override {}
    std::string TokenLiteral() const override;
    std::string String() const override;
    NodeType Type() const override { return NodeType::FUNCTION_LITERAL; };
};

class CallExpression : public Expression {
public:
    Token token;
    std::unique_ptr<Expression> Function;
    std::vector<std::unique_ptr<Expression>> Arguments;

    void expressionNode() override {}
    std::string TokenLiteral() const override;
    std::string String() const override;
    NodeType Type() const override { return NodeType::CALL_EXPRESSION; };
};

#endif // AST_HPP
