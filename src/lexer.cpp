#include "lexer.hpp"
#include "token.hpp"

Lexer::Lexer(const std::string& input)
    : input(input) {
    readChar();
}

Token Lexer::NextToken() {
    Token tok;
    skipWhitespace();

    switch (ch) {
        case '=': {
            if (peekChar() == '=') {
                char ch = this->ch;
                readChar();
                std::string literal = std::string(1, ch) + std::string(1, this->ch);
                tok = Token{TokenType::Eq, literal};
                std::string s = TokenTypeMap::tokenTypeToString(TokenType::Eq);
            } else {
                tok = newToken(TokenType::Assign, ch);
                std::string s = TokenTypeMap::tokenTypeToString(TokenType::Assign);
            }
        }
            break;
        case '!': {
            if (peekChar() == '=') {
                char ch = this->ch;
                readChar();
                std::string literal = std::string(1, ch) + std::string(1, this->ch);
                tok = Token{TokenType::NotEq, literal};
                std::string s = TokenTypeMap::tokenTypeToString(TokenType::NotEq);
            } else {
                tok = newToken(TokenType::Bang, ch);
                std::string s = TokenTypeMap::tokenTypeToString(TokenType::Bang);
            }
        }
            break;
        case '+': {
            tok = newToken(TokenType::Plus, ch); break;
            std::string s = TokenTypeMap::tokenTypeToString(TokenType::Plus);
        }
        case '-': {
            tok = newToken(TokenType::Minus, ch); break;
            std::string s = TokenTypeMap::tokenTypeToString(TokenType::Minus);
        }
        case '/': {
            tok = newToken(TokenType::Slash, ch); break;
            std::string s = TokenTypeMap::tokenTypeToString(TokenType::Slash);
        }
        case '*': {
            tok = newToken(TokenType::Asterisk, ch); break;
            std::string s = TokenTypeMap::tokenTypeToString(TokenType::Asterisk);
        }
        case '<': {
            tok = newToken(TokenType::Lt, ch); break;
            std::string s = TokenTypeMap::tokenTypeToString(TokenType::Lt);
        }
        case '>': {
            tok = newToken(TokenType::Gt, ch); break;
            std::string s = TokenTypeMap::tokenTypeToString(TokenType::Gt);
        }
        case ';': {
            tok = newToken(TokenType::Semicolon, ch); break;
            std::string s = TokenTypeMap::tokenTypeToString(tok.type);
        }
        case '(': {
            tok = newToken(TokenType::LParen, ch); break;
            std::string s = TokenTypeMap::tokenTypeToString(TokenType::LParen);
        }
        case ')': {
            tok = newToken(TokenType::RParen, ch); break;
            std::string s = TokenTypeMap::tokenTypeToString(TokenType::RParen);
        }
        case '{': {
            tok = newToken(TokenType::LSquirly, ch); break;
            std::string s = TokenTypeMap::tokenTypeToString(TokenType::LSquirly);
        }
        case '}': {
            tok = newToken(TokenType::RSquirly, ch); break;
            std::string s = TokenTypeMap::tokenTypeToString(TokenType::RSquirly);
        }
        case '[': {
            tok = newToken(TokenType::LBracket, ch); break;
            std::string s = TokenTypeMap::tokenTypeToString(tok.type);
        }
        case ']': {
            tok = newToken(TokenType::RBracket, ch); break;
            std::string s = TokenTypeMap::tokenTypeToString(tok.type);
        }
        case '"': {
            tok = newToken(TokenType::String, ch); break;
            std::string s = TokenTypeMap::tokenTypeToString(TokenType::String);
        }
        case ',': {
            tok = newToken(TokenType::Comma, ch); break;
            std::string s = TokenTypeMap::tokenTypeToString(TokenType::Comma);
        }
        case 0: {
            tok = Token{TokenType::Eof, ""}; break;
            std::string s = TokenTypeMap::tokenTypeToString(TokenType::Eof);
        }
        default: {
            if (isLetter(ch)) {
                tok.literal = readIdentifier();
                tok.type = TokenTypeMap::lookupIdent(tok.literal);
                return tok;
            } else if (isDigit(ch)) {
                tok.literal = readNumber();
                tok.type = TokenType::Int;
                return tok;
            } else {
                tok = newToken(TokenType::Illegal, ch);
            }
        }
    }

    readChar();
    return tok;
}

Token Lexer::newToken(TokenType type, char ch) const {
    return Token{type, std::string(1, ch)};
}

// Helper methods
void Lexer::readChar() {
    ch = (readPosition >= input.size()) ? 0 : input[readPosition];
    position = readPosition;
    ++readPosition;
}

char Lexer::peekChar() {
    return (readPosition >= input.size()) ? 0 : input[readPosition];
}

std::string Lexer::readIdentifier() {
    size_t start = position;
    while (isLetter(ch)) {
        readChar();
    }

    return input.substr(start, position - start);
}

std::string Lexer::readNumber() {
    size_t start = position;
    while (isDigit(ch)) {
        readChar();
    }

    return input.substr(start, position - start);
}

void Lexer::skipWhitespace() {
    while (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r') {
        readChar();
    }
}

bool Lexer::isLetter(char ch) const {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_';
}

bool Lexer::isDigit(char ch) const {
    return ch >= '0' && ch <= '9';
}

