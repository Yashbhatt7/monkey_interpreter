#include<memory>
#include "parser.hpp"

Parser::Parser(std::unique_ptr<Lexer> lexer)
    : l(std::move(lexer)){
    nextToken();
    nextToken();
}

void Parser::nextToken() {
    curToken = peekToken;
    peekToken = l->NextToken();
}

std::unique_ptr<Program> Parser::ParseProgram() {
    auto program = std::make_unique<Program>();

    while (curToken.type != TokenType::Eof) {
        auto stmt = parseStatement();
        if (stmt != nullptr) {
            // std::cout << "currToken is:.." << stm->Name->Value << "\n";
            // std::cout << "curToken is:.." << stmt->token;
            program->Statements.push_back(std::move(stmt));
        }
        nextToken();
    }

    return program;
}

std::unique_ptr<Statement> Parser::parseStatement() {
    switch (curToken.type) {
        case TokenType::Let:
            return parseLetStatement();
        case TokenType::Return:
            return parseReturnStatement();
        default:
            return parseExpressionStatement();
    }
}

std::unique_ptr<LetStatement> Parser::parseLetStatement() {
    auto stmt = std::make_unique<LetStatement>();
    stmt->token = curToken;

    if (!expectPeek(TokenType::Ident)) {
    // std::cout << "curToken is:.." << curToken.literal << std::endl;
    // std::cout << "peekToken is.." << peekToken.literal << std::endl;
        return nullptr;
    }
    // std::cout << "curToken is:.." << curToken.literal << std::endl;
    // std::cout << "peekToken is.." << peekToken.literal << std::endl;

    stmt->Name = std::make_unique<Identifier>();
    stmt->Name->token = curToken;
    stmt->Name->Value = curToken.literal;

    if (!expectPeek(TokenType::Assign)) {
        // std::cout << "curToken in assign is:.." << curToken.literal << std::endl;
        return nullptr;
    }

    while (!curTokenIs(TokenType::Semicolon)) {
        // std::cout << "curToken is:.." << curToken.literal << std::endl;
        nextToken();
    }

    // std::cout << "curToken is:.." << curToken.literal << std::endl;
    return stmt;
}

std::unique_ptr<ReturnStatement> Parser::parseReturnStatement() {
    auto stmt = std::make_unique<ReturnStatement>();
    stmt->token = curToken;

    nextToken();

    while (!curTokenIs(TokenType::Semicolon)) {
        nextToken();
    }

    return stmt;
}

std::unique_ptr<ExpressionStatement> Parser::parseExpressionStatement() {
    auto stmt = std::make_unique<ExpressionStatement>();
    stmt->token = curToken;

    stmt->Expression = parseExpression(LOWEST);

    if (peekTokenIs(TokenType::Semicolon)) {
        nextToken();
    }

    return stmt;
}

bool Parser::expectPeek(TokenType t) {
    if (peekTokenIs(t)) {
        nextToken();
        return true;
    } else {
        peekErrors(t);
        return false;
    }
}

bool Parser::curTokenIs(TokenType t) {
    return curToken.type == t;
}

bool Parser::peekTokenIs(TokenType t) {
    return peekToken.type == t;
}

void Parser::peekErrors(TokenType t) {
    std::ostringstream oss;
    oss << "expected next token to be " << TokenTypeMap::tokenTypeToString(t)
        << ", got " << TokenTypeMap::tokenTypeToString(peekToken.type) << " instead";
    errors.push_back(oss.str());
}

std::vector<std::string> Parser::Errors() {
    return errors;
}

void Parser::registerPrefix(TokenType tokenType, prefixParseFn fn) {
    prefixParseFns[tokenType] = fn;
}

void Parser::registerInfix(TokenType tokenType, infixParseFn fn) {
    infixParseFns[tokenType] = fn;
}


