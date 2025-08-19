#include<memory>
#include "parser.hpp"

const std::unordered_map<TokenType, Precedence> Parser::precedences = {
    { TokenType::Eq,        Precedence::EQUALS },
    { TokenType::NotEq,     Precedence::EQUALS },
    { TokenType::Lt,        Precedence::LESSGREATER },
    { TokenType::Gt,        Precedence::LESSGREATER },
    { TokenType::Plus,      Precedence::SUM },
    { TokenType::Minus,     Precedence::SUM },
    { TokenType::Slash,     Precedence::PRODUCT },
    { TokenType::Asterisk,  Precedence::PRODUCT },
};

Parser::Parser(std::unique_ptr<Lexer> lexer)
    : l(std::move(lexer)) {

    // compiler knows that "this" in capture means "return this->parseIdentifier();"
    registerPrefix(TokenType::Ident, [this]() { return parseIdentifier(); });
    registerPrefix(TokenType::Int, [this]() { return parseIntegerLiteral(); });

    registerPrefix(TokenType::Bang, [this]() { return parsePrefixExpression(); });
    registerPrefix(TokenType::Minus, [this]() { return parsePrefixExpression(); });

    registerInfix(TokenType::Plus, [this](std::unique_ptr<Expression> left) { return parseInfixExpression(std::move(left)); });
    registerInfix(TokenType::Minus, [this](std::unique_ptr<Expression> left) { return parseInfixExpression(std::move(left)); });
    registerInfix(TokenType::Slash, [this](std::unique_ptr<Expression> left) { return parseInfixExpression(std::move(left)); });
    registerInfix(TokenType::Asterisk, [this](std::unique_ptr<Expression> left) { return parseInfixExpression(std::move(left)); });
    registerInfix(TokenType::Eq, [this](std::unique_ptr<Expression> left) { return parseInfixExpression(std::move(left)); });
    registerInfix(TokenType::NotEq, [this](std::unique_ptr<Expression> left) { return parseInfixExpression(std::move(left)); });
    registerInfix(TokenType::Lt, [this](std::unique_ptr<Expression> left) { return parseInfixExpression(std::move(left)); });
    registerInfix(TokenType::Gt, [this](std::unique_ptr<Expression> left) { return parseInfixExpression(std::move(left)); });

    nextToken();
    nextToken();
}

void Parser::nextToken() {
    curToken = peekToken;
    peekToken = l->NextToken();
}

std::unique_ptr<Expression> Parser::parseIdentifier() {
    auto ident = std::make_unique<Identifier>();
    ident->token = curToken;
    ident->Value = curToken.literal;

    return ident;
}

std::unique_ptr<Expression> Parser::parseIntegerLiteral() {
    auto lit = std::make_unique<IntegerLiteral>();
    lit->token = curToken;
    // std::cout << "CURTOKEN IS: " << curToken.literal << "\n";

    std::stringstream ss(curToken.literal);
    int64_t value;

    if (!(ss >> value) || !ss.eof()) {
        std::string msg = "could not parse \"" + curToken.literal + "\" as integer";
        errors.push_back(msg);
        return nullptr;
    }

    lit->Value = value;

    return lit;
}

std::unique_ptr<Expression> Parser::parsePrefixExpression() {
    auto expression = std::make_unique<PrefixExpression>();
    expression->token = curToken;
    expression->Operator = curToken.literal;

    nextToken();

    expression->Right = parseExpression(static_cast<int>(Precedence::PREFIX));

    return expression;
}

std::unique_ptr<Expression> Parser::parseInfixExpression(std::unique_ptr<Expression> left) {
    auto expression = std::make_unique<InfixExpression>();
    expression->token = curToken;
    // std::cout << "curToken for infixExpression method is: " << curToken.literal << "\n";
    expression->Operator = curToken.literal;
    expression->Left = std::move(left);
    // std::cout << "Left in infixExpression method: " << expression->Left->TokenLiteral() << "\n";

    int precedence = curPrecedence();
    // std::cout << "in between left and right: " << curToken.literal << "\n";
    nextToken();
    expression->Right = parseExpression(precedence);
    // std::cout << "Right in infixExpression method: " << expression->Right->TokenLiteral() << "\n";

    return expression;
}

// <==----------------------------------From here parser starts parsing the program-----------------------------------------==>
std::unique_ptr<Program> Parser::ParseProgram() {
    auto program = std::make_unique<Program>();

    while (curToken.type != TokenType::Eof) {
        auto stmt = parseStatement(); // Type => Statement
        if (stmt != nullptr) {
            program->Statements.push_back(std::move(stmt));
        }
        // std::cout << "im right its semicolon..." << curToken.literal << "\n";
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
    // std::cout << "curToken is: " << curToken.literal << "\n";

    stmt->Expression = parseExpression(static_cast<int>(Precedence::LOWEST));

    if (peekTokenIs(TokenType::Semicolon)) {
        nextToken();
    }

    return stmt;
}

std::unique_ptr<Expression> Parser::parseExpression(int precedence) {
    auto prefixItr = prefixParseFns.find(curToken.type);
    // std::cout << "curToken for parseExpression method is: " << curToken.literal << "\n";
    if (prefixItr == prefixParseFns.end()) {
        noPrefixParseFnError(curToken.type);
        return nullptr;
    }

    prefixParseFn prefix = prefixItr->second;
    std::unique_ptr<Expression> leftExp = prefix();

    while (!peekTokenIs(TokenType::Semicolon) && precedence < peekPrecedence()) {
        auto infixItr = infixParseFns.find(peekToken.type);
        // std::cout << "peekToken type for precedence check: " << peekToken.literal << "\n";
        if (infixItr == infixParseFns.end()) {
            return leftExp;
        }

        infixParseFn infix = infixItr->second;

        nextToken();
        leftExp = infix(std::move(leftExp));
        std::cout << "leftExp: " << leftExp->TokenLiteral() << "\n";
    }

    return leftExp;
}

void Parser::registerPrefix(TokenType tokenType, prefixParseFn fn) {
    prefixParseFns[tokenType] = fn;
}

void Parser::registerInfix(TokenType tokenType, infixParseFn fn) {
    infixParseFns[tokenType] = fn;
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

int Parser::peekPrecedence() {
    auto it = precedences.find(peekToken.type);
    // std::cout << "precedence: " << static_cast<int>(it->second) << "\n";
    if (it != precedences.end()) {
        return static_cast<int>(it->second);
    }

    return static_cast<int>(Precedence::LOWEST);
}

int Parser::curPrecedence() {
    auto it = precedences.find(peekToken.type);
    if (it != precedences.end()) {
        return static_cast<int>(it->second);
    }

    return static_cast<int>(Precedence::LOWEST);
}

void Parser::peekErrors(TokenType t) {
    std::ostringstream oss;
    oss << "expected next token to be " << TokenTypeMap::tokenTypeToString(t)
        << ", got " << TokenTypeMap::tokenTypeToString(peekToken.type) << " instead";
    errors.push_back(oss.str());
}

void Parser::noPrefixParseFnError(TokenType t) {
    std::ostringstream oss;
    oss << "no prefix parse function for " << TokenTypeMap::tokenTypeToString(t) << " found";
    errors.push_back(oss.str());
}

std::vector<std::string> Parser::Errors() {
    return errors;
}


