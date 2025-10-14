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
    { TokenType::LParen,    Precedence::CALL },
    { TokenType::LBracket,  Precedence::INDEX },
};

Parser::Parser(std::unique_ptr<Lexer> lexer)
    : l(std::move(lexer)) {

    // compiler knows that "this" in capture means "return this->parseIdentifier();"
    registerPrefix(TokenType::Ident, [this]() { return parseIdentifier(); });
    registerPrefix(TokenType::Int, [this]() { return parseIntegerLiteral(); });
    registerPrefix(TokenType::String, [this]() { return parseStringLiteral(); });

    registerPrefix(TokenType::If, [this]() { return parseIfExpression(); });
    registerPrefix(TokenType::Function, [this]() { return parseFunctionLiteral(); });

    registerPrefix(TokenType::Bang, [this]() { return parsePrefixExpression(); });
    registerPrefix(TokenType::Minus, [this]() { return parsePrefixExpression(); });
    registerPrefix(TokenType::LParen, [this]() { return parseGroupedExpression(); });
    registerPrefix(TokenType::LBracket, [this]() { return parseArrayLiteral(); });
    registerPrefix(TokenType::LSquirly, [this]() { return parseHashLiteral(); });

    registerPrefix(TokenType::True, [this]() { return parseBoolean(); });
    registerPrefix(TokenType::False, [this]() { return parseBoolean(); });

    registerInfix(TokenType::Plus, [this](std::unique_ptr<Expression> left) { return parseInfixExpression(std::move(left)); });
    registerInfix(TokenType::Minus, [this](std::unique_ptr<Expression> left) { return parseInfixExpression(std::move(left)); });
    registerInfix(TokenType::Slash, [this](std::unique_ptr<Expression> left) { return parseInfixExpression(std::move(left)); });
    registerInfix(TokenType::Asterisk, [this](std::unique_ptr<Expression> left) { return parseInfixExpression(std::move(left)); });
    registerInfix(TokenType::Eq, [this](std::unique_ptr<Expression> left) { return parseInfixExpression(std::move(left)); });
    registerInfix(TokenType::NotEq, [this](std::unique_ptr<Expression> left) { return parseInfixExpression(std::move(left)); });
    registerInfix(TokenType::Lt, [this](std::unique_ptr<Expression> left) { return parseInfixExpression(std::move(left)); });
    registerInfix(TokenType::Gt, [this](std::unique_ptr<Expression> left) { return parseInfixExpression(std::move(left)); });
    registerInfix(TokenType::LParen, [this](std::unique_ptr<Expression> left) { return parseCallExpression(std::move(left)); });
    registerInfix(TokenType::LBracket, [this](std::unique_ptr<Expression> left) { return parseIndexExpression(std::move(left)); });

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

std::unique_ptr<Expression> Parser::parseBoolean() {
    // Trace trace("parseBoolean");
    auto boolean = std::make_unique<BooleanLiteral>();
    boolean->token = curToken;
    boolean->Value = curTokenIs(TokenType::True);

    return boolean;
}

std::unique_ptr<Expression> Parser::parseGroupedExpression() {
    nextToken();

    auto exp = parseExpression(static_cast<int>(Precedence::LOWEST));

    if (!expectPeek(TokenType::RParen)) {
        return nullptr;
    }

    return exp;
}

std::unique_ptr<Expression> Parser::parseIfExpression() {
    auto expression = std::make_unique<IfExpression>();
    expression->token = curToken;

    if (!expectPeek(TokenType::LParen)) {
        return nullptr;
    }

    nextToken();
    expression->Condition = parseExpression(static_cast<int>(Precedence::LOWEST));

    if (!expectPeek(TokenType::RParen)) {
        return nullptr;
    }

    if (!expectPeek(TokenType::LSquirly)) {
        return nullptr;
    }

    expression->Consequence = parseBlockStatement();

    if (peekTokenIs(TokenType::Else)) {
        nextToken();

        if (!expectPeek(TokenType::LSquirly)) {
            return nullptr;
        }

        expression->Alternative = parseBlockStatement();
    }

    return expression;
}

std::unique_ptr<Expression> Parser::parseFunctionLiteral() {
    auto lit = std::make_unique<FunctionLiteral>();

    lit->token = curToken;

    if (!expectPeek(TokenType::LParen)) {
        return nullptr;
    }

    lit->Parameters = parseFunctionParameters();

    if (!expectPeek(TokenType::LSquirly)) {
        return nullptr;
    }

    lit->Body = parseBlockStatement();

    return lit;
}

std::vector<std::unique_ptr<Identifier>> Parser::parseFunctionParameters() {
    std::vector<std::unique_ptr<Identifier>> identifiers;

    if (peekTokenIs(TokenType::RParen)) {
        nextToken();
        return identifiers;
    }

    nextToken();

    auto ident = std::make_unique<Identifier>();
    ident->token = curToken;
    ident->Value = curToken.literal;

    identifiers.push_back(std::move(ident));

    while (peekTokenIs(TokenType::Comma)) {
        nextToken();
        nextToken();
        auto ident = std::make_unique<Identifier>();
        ident->token = curToken;
        ident->Value = curToken.literal;
        identifiers.push_back(std::move(ident));
    }

    if (!expectPeek(TokenType::RParen)) {
        return {};
    }

    return identifiers;
}

std::unique_ptr<Expression> Parser::parseIntegerLiteral() {
    // Trace trace("parseIntegerLiteral");
    auto lit = std::make_unique<IntegerLiteral>();
    lit->token = curToken;

    std::stringstream ss(curToken.literal);
    int64_t value;

    // Here operator >> is an stream extraction operator to extract the value from ss to value
    if (!(ss >> value) || !ss.eof()) {
        std::string msg = "could not parse \"" + curToken.literal + "\" as integer";
        errors.push_back(msg);
        return nullptr;
    }

    lit->Value = value;

    return lit;
}

std::unique_ptr<Expression> Parser::parseStringLiteral() {
    auto lit = std::make_unique<StringLiteral>();
    lit->token = curToken;
    lit->Value = curToken.literal;
    return lit;
}

std::unique_ptr<Expression> Parser::parseArrayLiteral() {
    auto array = std::make_unique<ArrayLiteral>();
    array->token = curToken;

    array->Elements = parseExpressionList(TokenType::RBracket);

    return array;
}

std::unique_ptr<Expression> Parser::parseHashLiteral() {
    auto hash = std::make_unique<HashLiteral>();
    hash->token = curToken;

    while (!peekTokenIs(TokenType::RSquirly)) {
        nextToken();
        auto key = parseExpression(static_cast<int>(Precedence::LOWEST));

        if (!expectPeek(TokenType::Colon)) {
            return nullptr;
        }

        nextToken();
        auto value = parseExpression(static_cast<int>(Precedence::LOWEST));

        Expression* keyPtr = key.get();
        hash->Pairs[keyPtr] = std::move(value);
        key.release();

        if (!peekTokenIs(TokenType::RSquirly) && !expectPeek(TokenType::Comma)) {
            return nullptr;
        }
    }

    if (!expectPeek(TokenType::RSquirly)) {
        return nullptr;
    }

    return hash;
}

std::unique_ptr<Expression> Parser::parsePrefixExpression() {
    // Trace trace("parsePrefixExpression");
    auto expression = std::make_unique<PrefixExpression>();
    expression->token = curToken;
    expression->Operator = curToken.literal;

    nextToken();

    expression->Right = parseExpression(static_cast<int>(Precedence::PREFIX));

    return expression;
}

std::unique_ptr<Expression> Parser::parseInfixExpression(std::unique_ptr<Expression> left) {
    // Trace trace("parseInfixExpression");
    auto expression = std::make_unique<InfixExpression>();
    expression->token = curToken;
    expression->Operator = curToken.literal;
    expression->Left = std::move(left);

    int precedence = curPrecedence();
    nextToken();
    expression->Right = parseExpression(precedence);

    // For demonstration purpose
    // if (expression->Operator == "+") {
    //     expression->Right = parseExpression(precedence - 1);
    // } else {
    //     expression->Right = parseExpression(precedence);
    // }


    return expression;
}

std::unique_ptr<Expression> Parser::parseCallExpression(std::unique_ptr<Expression> left) {
    auto exp = std::make_unique<CallExpression>();
    exp->token = curToken;
    exp->Function = std::move(left);
    // exp->Arguments = parseCallArguments();
    exp->Arguments = parseExpressionList(TokenType::RParen);

    return exp;
}

std::vector<std::unique_ptr<Expression>> Parser::parseExpressionList(TokenType end) {
    std::vector<std::unique_ptr<Expression>> list;

    if (peekTokenIs(end)) {
        nextToken();
        return list;
    }

    nextToken();
    list.push_back(parseExpression(static_cast<int>(Precedence::LOWEST)));

    while (peekTokenIs(TokenType::Comma)) {
        nextToken();
        nextToken();
        list.push_back(parseExpression(static_cast<int>(Precedence::LOWEST)));
    }

    if (!expectPeek(end)) {
        return {};
    }

    return list;
}

std::unique_ptr<Expression> Parser::parseIndexExpression(std::unique_ptr<Expression> left) {
    auto expr = std::make_unique<IndexExpression>();
    expr->token = curToken;
    expr->Left = std::move(left);

    nextToken();
    expr->Index = parseExpression(static_cast<int>(Precedence::LOWEST));

    if (!expectPeek(TokenType::RBracket)) {
        return nullptr;
    }

    return expr;
}

// <<- ----------------------------------From here parser starts parsing the program------------------------------------------ ->>
std::unique_ptr<Program> Parser::ParseProgram() {
    auto program = std::make_unique<Program>();

    while (curToken.type != TokenType::Eof) {
        auto stmt = parseStatement(); // Type => Statement
        if (stmt != nullptr) {
            program->Statements.push_back(std::move(stmt));
        }
        nextToken();
    }

    return program;
}

std::unique_ptr<BlockStatement> Parser::parseBlockStatement() {
    auto block = std::make_unique<BlockStatement>();

    nextToken();

    while (!curTokenIs(TokenType::RSquirly) && !curTokenIs(TokenType::Eof)) {
        auto stmt = parseStatement();

        if (stmt != nullptr) {
            block->Statements.push_back(std::move(stmt));
        }
        nextToken();
    }

    return block;
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
        return nullptr;
    }

    stmt->Name = std::make_unique<Identifier>();
    stmt->Name->token = curToken;
    stmt->Name->Value = curToken.literal;

    if (!expectPeek(TokenType::Assign)) {
        return nullptr;
    }

    nextToken();

    stmt->Value = parseExpression(static_cast<int>(Precedence::LOWEST));

    if (peekTokenIs(TokenType::Semicolon)) {
        nextToken();
    }

    return stmt;
}

std::unique_ptr<ReturnStatement> Parser::parseReturnStatement() {
    auto stmt = std::make_unique<ReturnStatement>();
    stmt->token = curToken;

    nextToken();

    stmt->ReturnValue = parseExpression(static_cast<int>(Precedence::LOWEST));

    if (peekTokenIs(TokenType::Semicolon)) {
        nextToken();
    }

    return stmt;
}

std::unique_ptr<ExpressionStatement> Parser::parseExpressionStatement() {
    // Trace trace("parseExpressionStatement");
    auto stmt = std::make_unique<ExpressionStatement>();
    stmt->token = curToken;

    stmt->ExpressionPtr = parseExpression(static_cast<int>(Precedence::LOWEST));

    if (peekTokenIs(TokenType::Semicolon)) {
        nextToken();
    }

    return stmt;
}

// so if the Left-Binding-Power is greater than the Right-Binding-Power
// then Left-Binding-Power of the operator(peekOperator in this case)
// "sucks in" what we parsed so far and uses it as the "left arm" of the AST node it is constructing
std::unique_ptr<Expression> Parser::parseExpression(int precedence) {
    // Trace trace("parseExpression");
    auto prefixItr = prefixParseFns.find(curToken.type);
    if (prefixItr == prefixParseFns.end()) {
        noPrefixParseFnError(curToken.type);
        return nullptr;
    }

    prefixParseFn prefix = prefixItr->second;
    std::unique_ptr<Expression> leftExp = prefix();

    // Left-Binding-Power ->> peekPrecedence() > Right-Binding-Power ->> precedence
    while (!peekTokenIs(TokenType::Semicolon) && precedence < peekPrecedence()) {
        auto infixItr = infixParseFns.find(peekToken.type);
        if (infixItr == infixParseFns.end()) {
            return leftExp;
        }

        infixParseFn infix = infixItr->second;

        nextToken();
        leftExp = infix(std::move(leftExp));
    }

    return leftExp;
}

int Parser::peekPrecedence() {
    auto it = precedences.find(peekToken.type);
    if (it != precedences.end()) {
        return static_cast<int>(it->second);
    }

    return static_cast<int>(Precedence::LOWEST);
}

int Parser::curPrecedence() {
    auto it = precedences.find(curToken.type);
    if (it != precedences.end()) {
        return static_cast<int>(it->second);
    }

    return static_cast<int>(Precedence::LOWEST);
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

void Parser::noPrefixParseFnError(TokenType t) {
    std::ostringstream oss;
    oss << "no prefix parse function for " << TokenTypeMap::tokenTypeToString(t) << " found";
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

// Tracer functions
// std::string get_indent() {
//     return std::string(trace_level * 2, ' ');
// }
//
// void trace(const std::string& name) {
//     ++trace_level;
// }
//
// void untrace(const std::string& name) {
//     --trace_level;
// }

