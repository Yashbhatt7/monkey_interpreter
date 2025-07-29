#include "../include/lexer.hpp"
#include "../include/token/token.hpp"

Lexer::Lexer(const std::string& input)
    : input(input) {
    readChar();
}

// std::string s = TokenTypeMap::tokenTypeToString(TokenType::Assign);
// std::cout << "s: " << s << "\n";

Token Lexer::NextToken() {
    Token tok;
    // std::cout << "our dear ch: " << ch << "\n\n";
    skipWhitespace();

    switch (ch) {
        case '=': {
            if (peekChar() == '=') {
                char ch = this->ch;
                readChar();
                std::string literal = std::string(1, ch) + std::string(1, this->ch);
                tok = Token{TokenType::Eq, literal};
            } else {
                tok = newToken(TokenType::Assign, ch);
            }
        }
            break;
        case '!': {
            if(peekChar() == '=') {
                char ch = this->ch;
                readChar();
                std::string literal = std::string(1, ch) + std::string(1, this->ch);
                tok = Token{TokenType::NotEq, literal};
            } else {
                tok = newToken(TokenType::Bang, ch);
            }
        }
            break;
        case '+':
            tok = newToken(TokenType::Plus, ch); break;
        case '-':
            tok = newToken(TokenType::Minus, ch); break;
        case '/':
            tok = newToken(TokenType::Slash, ch); break;
        case '*':
            tok = newToken(TokenType::Asterisk, ch); break;
        case '<':
            tok = newToken(TokenType::Lt, ch); break;
        case '>':
            tok = newToken(TokenType::Gt, ch); break;
        case ';':
            tok = newToken(TokenType::Semicolon, ch); break;
        case '(':
            tok = newToken(TokenType::LParen, ch); break;
        case ')':
            tok = newToken(TokenType::RParen, ch); break;
        case '{':
            tok = newToken(TokenType::LSquarly, ch); break;
        case '}':
            tok = newToken(TokenType::RSquarly, ch); break;
        case ',':
            tok = newToken(TokenType::Comma, ch); break;
        case 0:
            tok = Token{TokenType::Eof, ""}; break;
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

