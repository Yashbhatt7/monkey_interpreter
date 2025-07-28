#include "../include/lexer.hpp"
#include "../include/token/token.hpp"

Lexer::Lexer(const std::string& input)
    : input(input) {
    readChar();
}

Token Lexer::NextToken() {
    Token tok;
    skipWhitespace();

    switch (ch) {

        case '=': {
            std::string s = TokenTypeMap::tokenTypeToString(TokenType::Assign);
            std::cout << "s: " << s << "\n";
            tok = newToken(TokenType::Assign, ch); break;
        }
        case '+': {
            std::string s = TokenTypeMap::tokenTypeToString(TokenType::Plus);
            std::cout << "s: " << s << "\n";
            tok = newToken(TokenType::Plus, ch); break;
        }
        case ';': {
            std::string s = TokenTypeMap::tokenTypeToString(TokenType::Semicolon);
            std::cout << "s: " << s << "\n";
            tok = newToken(TokenType::Semicolon, ch); break;
        }
        case '(': {
            std::string s = TokenTypeMap::tokenTypeToString(TokenType::LParen);
            std::cout << "s: " << s << "\n";
            tok = newToken(TokenType::LParen, ch); break;
        }
        case ')': {
            std::string s = TokenTypeMap::tokenTypeToString(TokenType::RParen);
            std::cout << "s: " << s << "\n";
            tok = newToken(TokenType::RParen, ch); break;
        }
        case '{': {
            std::string s = TokenTypeMap::tokenTypeToString(TokenType::LSquarly);
            std::cout << "s: " << s << "\n";
            tok = newToken(TokenType::LSquarly, ch); break;
        }
        case '}': {
            std::string s = TokenTypeMap::tokenTypeToString(TokenType::RSquarly);
            std::cout << "s: " << s << "\n";
            tok = newToken(TokenType::RSquarly, ch); break;
        }
        case ',': {
            std::string s = TokenTypeMap::tokenTypeToString(TokenType::Comma);
            std::cout << "s: " << s << "\n";
            tok = newToken(TokenType::Comma, ch); break;
        }
        case 0: {
            std::string s = TokenTypeMap::tokenTypeToString(TokenType::Eof);
            std::cout << "s: " << s << "\n";
            tok = Token{TokenType::Eof, ""}; break;
        }
        default: {
            if (isLetter(ch)) {
                tok.literal = readIdentifier();
                std::cout << "literal: " << tok.literal << "\n";
                tok.type = TokenTypeMap::lookupIdent(tok.literal);
                std::cout << "type is: ";
                std::cout << TokenTypeMap::tokenTypeToString(tok.type) << "\n\n";
                return tok;
            } else if (isDigit(ch)) {
                tok.literal = readNumber();
                std::cout << "literal: " << tok.literal << "\n";
                tok.type = TokenType::Int;
                std::cout << "type is: ";
                std::cout << TokenTypeMap::tokenTypeToString(tok.type) << "\n\n";
                return tok;
            } else {
                std::cout << "Illegal...\n";
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

