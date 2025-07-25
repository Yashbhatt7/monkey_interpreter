#pragma once
#include<string>
#include<../token/token.hpp>

class Lexer {
private:
    std::string input;
    size_t position = 0;
    size_t readPosition = 0;
    char ch = 0;

public:
    Lexer New(std::string input) {
        return Lexer(std::move(input));
    }

    void readChar() {
    }

    Lexer(std::string input)
        : input(std::move(input)) {
        readChar();
    }

    Token NextToken() {
        Token tok;

        return tok;
    }

};




