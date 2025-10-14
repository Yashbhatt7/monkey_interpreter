#ifndef TOKEN_HPP
#define TOKEN_HPP

#include<iostream>
#include<unordered_map>

enum class TokenType {
    Illegal,
    Eof,

    // Identifiers + literals
    Ident,
    Int,

    // Operators
    Assign,
    Plus,
    Minus,
    Bang,
    Asterisk,
    Slash,
    Lt,
    Gt,
    Eq,
    NotEq,

    // Delimiters
    Comma,
    Colon,
    Semicolon,
    LParen,
    RParen,
    LSquirly,
    RSquirly,
    LBracket,
    RBracket,

    // Keywords
    Function,
    Let,
    True,
    False,
    If,
    Else,
    Return,

    //String
    String,
};

struct Token {
    TokenType type;
    std::string literal;
};

class TokenTypeMap {
public:
    static std::string tokenTypeToString(TokenType type) {
        static const std::unordered_map<TokenType, std::string> typeToString = {
            {TokenType::Illegal, "ILLEGAL"},
            {TokenType::Eof, "EOF"},
            {TokenType::Ident, "IDENT"},
            {TokenType::Int, "INT"},
            {TokenType::String, "\""},
            {TokenType::Assign, "="},
            {TokenType::Plus, "+"},
            {TokenType::Minus, "-"},
            {TokenType::Bang, "!"},
            {TokenType::Asterisk, "*"},
            {TokenType::Slash, "/"},
            {TokenType::Lt, "<"},
            {TokenType::Gt, ">"},
            {TokenType::Eq, "=="},
            {TokenType::NotEq, "!="},
            {TokenType::Comma, ","},
            {TokenType::Colon, ":"},
            {TokenType::Semicolon, ";"},
            {TokenType::LParen, "("},
            {TokenType::RParen, ")"},
            {TokenType::LSquirly, "{"},
            {TokenType::RSquirly, "}"},
            {TokenType::LBracket, "["},
            {TokenType::RBracket, "]"},
            {TokenType::Function, "FUNCTION"},
            {TokenType::Let, "LET"},
            {TokenType::True, "TRUE"},
            {TokenType::False, "FALSE"},
            {TokenType::If, "IF"},
            {TokenType::Else, "ELSE"},
            {TokenType::Return, "RETURN"},
        };
        return typeToString.at(type);
    }

    static TokenType lookupIdent(const std::string& ident) {
        static const std::unordered_map<std::string, TokenType> keywords = {
            { "fn", TokenType::Function },
            { "let", TokenType::Let },
            { "true", TokenType::True },
            { "false", TokenType::False },
            { "if", TokenType::If },
            { "else", TokenType::Else },
            { "return", TokenType::Return },
        };

        auto it = keywords.find(ident);
        if(it != keywords.end()) {
            return it->second;
        }
        return TokenType::Ident;
    }
};

#endif // TOKEN_HPP
