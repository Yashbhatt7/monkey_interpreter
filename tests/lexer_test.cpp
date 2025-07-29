#include<gtest/gtest.h>
#include "../include/lexer.hpp"
#include "../include/token/token.hpp"

TEST(LexerTest, NextTokenFullTest) {
    // std::string input = R"(
    //     let five = 5;
    //     let ten = 10;
    //
    //     let add = fn(x, y) {
    //     x + y;
    //     };
    //
    //     let result = add(five, ten);
    //     !-/*5;
    //     5 < 10 > 5;
    //
    //     if (5 < 10) {
    //     return true;
    //     } else {
    //     return false;
    //     }
    //
    //     10 == 10;
    //     10 != 9;
    // )";


    // std::string input  = "=+(){},;";


    // std::string input = R"(
    //     let five = 5;
    //     let ten = 10;
    //
    //     let add = fn(x, y) {
    //         x + y;
    //     };
    //
    //     let result = add(five, ten);
    // )";


    std::string input = R"(
        let five = 5;
        let ten = 10;

        let add = fn(x, y) {
            x + y;
        };

        let result = add(five, ten);

        !-/*5;
        5 < 10 > 5;

        if (5 < 10) {
            return true;
        } else {
            return false;
        }
    )";


    Lexer l(input);

    struct TestCase {
        TokenType expectedType;
        std::string expectedLiteral;
    };

    std::vector<TestCase> tests = {
        {TokenType::Let, "let"},
        {TokenType::Ident, "five"},
        {TokenType::Assign, "="},
        {TokenType::Int, "5"},
        {TokenType::Semicolon, ";"},
        {TokenType::Let, "let"},
        {TokenType::Ident, "ten"},
        {TokenType::Assign, "="},
        {TokenType::Int, "10"},
        {TokenType::Semicolon, ";"},
        {TokenType::Let, "let"},
        {TokenType::Ident, "add"},
        {TokenType::Assign, "="},
        {TokenType::Function, "fn"},
        {TokenType::LParen, "("},
        {TokenType::Ident, "x"},
        {TokenType::Comma, ","},
        {TokenType::Ident, "y"},
        {TokenType::RParen, ")"},
        {TokenType::LSquarly, "{"},
        {TokenType::Ident, "x"},
        {TokenType::Plus, "+"},
        {TokenType::Ident, "y"},
        {TokenType::Semicolon, ";"},
        {TokenType::RSquarly, "}"},
        {TokenType::Semicolon, ";"},
        {TokenType::Let, "let"},
        {TokenType::Ident, "result"},
        {TokenType::Assign, "="},
        {TokenType::Ident, "add"},
        {TokenType::LParen, "("},
        {TokenType::Ident, "five"},
        {TokenType::Comma, ","},
        {TokenType::Ident, "ten"},
        {TokenType::RParen, ")"},
        {TokenType::Semicolon, ";"},
        // !-/*5;
        // 5 < 10 > 5;
        {TokenType::Bang, "!"},
        {TokenType::Minus, "-"},
        {TokenType::Slash, "/"},
        {TokenType::Asterisk, "*"},
        {TokenType::Int, "5"},
        {TokenType::Semicolon, ";"},
        {TokenType::Int, "5"},
        {TokenType::Lt, "<"},
        {TokenType::Int, "10"},
        {TokenType::Gt, ">"},
        {TokenType::Int, "5"},
        {TokenType::Semicolon, ";"},
        // if (5 < 10) {
        //     return true;
        // } else {
        //     return false;
        // }
        {TokenType::If, "if"},
        {TokenType::Slash, "("},
        {TokenType::Semicolon, "5"},
        {TokenType::Semicolon, "<"},
        {TokenType::Semicolon, "10"},
        {TokenType::Semicolon, ")"},
        {TokenType::Semicolon, "{"},
        {TokenType::Semicolon, "return"},
        {TokenType::Semicolon, "true"},
        {TokenType::Semicolon, ";"},
        {TokenType::Semicolon, "}"},
        {TokenType::Semicolon, "else"},
        {TokenType::Semicolon, "{"},
        {TokenType::Semicolon, "return"},
        {TokenType::Semicolon, "false"},
        {TokenType::Semicolon, ";"},
        {TokenType::Semicolon, "}"},
        {TokenType::Eof, ""},
    };

    for(size_t i = 0; i < tests.size(); ++i) {
        Token tok = l.NextToken();

        EXPECT_EQ(tok.type, tests[i].expectedType)
            << "Test [" << i << "] - TokenType wrong. Expected: "
            << TokenTypeMap::tokenTypeToString(tests[i].expectedType)
            << ", got: " << TokenTypeMap::tokenTypeToString(tok.type);

        EXPECT_EQ(tok.literal, tests[i].expectedLiteral)
            << "Test [" << i << "] - Literal wrong. Expected: '"
            << tests[i].expectedLiteral << "', got: '" << tok.literal << "'";
    }
}
