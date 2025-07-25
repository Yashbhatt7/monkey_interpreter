#include "lexer.hpp"
#include "../token/token.hpp"
#include<gtest/gtest.h>

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

    std::string input  = "=+(){},;";

    Lexer l(input);

    struct TestCase {
        TokenType expectedType;
        std::string expectedLiteral;
    };

    std::vector<TestCase> tests = {
        {TokenType::Assign, "="},
        {TokenType::Plus, "+"},
        {TokenType::LParen, "("},
        {TokenType::RParen, ")"},
        {TokenType::LSquarly, "{"},
        {TokenType::RSquarly, "}"},
        {TokenType::Comma, ","},
        {TokenType::Semicolon, ";"},
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


