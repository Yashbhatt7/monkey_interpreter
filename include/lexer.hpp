// include/lexer.hpp
#ifndef LEXER_HPP
#define LEXER_HPP

#include<string>
#include "token/token.hpp"

struct Token;

class Lexer {
private:
    std::string input;
    size_t position = 0;
    size_t readPosition = 0;
    char ch = 0;

    // Functions to read characters and tokens
    void readChar();
    char peekChar();
    std::string readIdentifier();
    std::string readNumber();
    void skipWhitespace();
    bool isLetter(char ch) const;
    bool isDigit(char ch) const;
    Token newToken(TokenType type, char ch) const;

public:
    explicit Lexer(const std::string& input);
    Token NextToken();
};

#endif // LEXER_HPP
