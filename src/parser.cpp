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
};

Parser::Parser(std::unique_ptr<Lexer> lexer)
    : l(std::move(lexer)) {

    // compiler knows that "this" in capture means "return this->parseIdentifier();"
    registerPrefix(TokenType::Ident, [this]() { return parseIdentifier(); });
    registerPrefix(TokenType::Int, [this]() { return parseIntegerLiteral(); });

    registerPrefix(TokenType::If, [this]() { return parseIfExpression(); });
    registerPrefix(TokenType::Function, [this]() { return parseFunctionLiteral(); });

    registerPrefix(TokenType::Bang, [this]() { return parsePrefixExpression(); });
    registerPrefix(TokenType::Minus, [this]() { return parsePrefixExpression(); });
    registerPrefix(TokenType::LParen, [this]() { return parseGroupedExpression(); });

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
    // std::cout << "CURTOKEN IS: " << curToken.literal << "\n";

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
    // std::cout << "curToken for infixExpression method is: " << curToken.literal << "\n";
    expression->Operator = curToken.literal;
    expression->Left = std::move(left);
    // std::cout << "Left in infixExpression method: " << expression->Left->TokenLiteral() << "\n";

    int precedence = curPrecedence();
    // std::cout << "in between left and right: " << curToken.literal << "\n";
    nextToken();
    expression->Right = parseExpression(precedence);

    // For demonstration purpose
    // if (expression->Operator == "+") {
    //     expression->Right = parseExpression(precedence - 1);
    // } else {
    //     expression->Right = parseExpression(precedence);
    // }

    // std::cout << "Right in infixExpression method: " << expression->Right->TokenLiteral() << "\n";

    return expression;
}

std::unique_ptr<Expression> Parser::parseCallExpression(std::unique_ptr<Expression> left) {
    auto exp = std::make_unique<CallExpression>();
    exp->token = curToken;
    exp->Function = std::move(left);
    exp->Arguments = parseCallArguments();

    return exp;
}

std::vector<std::unique_ptr<Expression>> Parser::parseCallArguments() {
    std::vector<std::unique_ptr<Expression>> args;

    if (peekTokenIs(TokenType::RParen)) {
        nextToken();
        return args;
    }

    nextToken();
    args.push_back(parseExpression(static_cast<int>(Precedence::LOWEST)));

    while (peekTokenIs(TokenType::Comma)) {
        nextToken();
        nextToken();
        args.push_back(parseExpression(static_cast<int>(Precedence::LOWEST)));
    }

    if (!expectPeek(TokenType::RParen)) {
        return {};
    }

    return args;
}

// <<- ----------------------------------From here parser starts parsing the program------------------------------------------ ->>
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

    nextToken();

    stmt->Value = parseExpression(static_cast<int>(Precedence::LOWEST));

    if (peekTokenIs(TokenType::Semicolon)) {
        nextToken();
    }

    // std::cout << "curToken is:.." << curToken.literal << std::endl;
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
    // std::cout << "curToken is: " << curToken.literal << "\n";

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
    // std::cout << "curToken for parseExpression method is: " << curToken.literal << "\n";
    if (prefixItr == prefixParseFns.end()) {
        noPrefixParseFnError(curToken.type);
        return nullptr;
    }

    prefixParseFn prefix = prefixItr->second;
    std::unique_ptr<Expression> leftExp = prefix();

    // Left-Binding-Power ->> peekPrecedence() > Right-Binding-Power ->> precedence
    while (!peekTokenIs(TokenType::Semicolon) && precedence < peekPrecedence()) {
        auto infixItr = infixParseFns.find(peekToken.type);
        // std::cout << "peekToken type for precedence check: " << peekToken.literal << "\n";
        if (infixItr == infixParseFns.end()) {
            return leftExp;
        }

        infixParseFn infix = infixItr->second;

        nextToken();
        leftExp = infix(std::move(leftExp));
        // std::cout << "leftExp: " << leftExp->TokenLiteral() << "\n";
    }

    return leftExp;
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
std::string get_indent() {
    return std::string(trace_level * 2, ' ');
}

void trace(const std::string& name) {
    std::cout << get_indent() << "BEGIN " << name << "\n";
    ++trace_level;
}

void untrace(const std::string& name) {
    --trace_level;
    std::cout << get_indent() << "END " << name << "\n";
}


