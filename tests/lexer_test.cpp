#include<gtest/gtest.h>
#include "../src/lexer.hpp"
#include "../src/token.hpp"

// g++ -std=c++17 lexer_test.cpp ../src/lexer.cpp -o lexer_test -lgtest -lgtest_main -pthread -I../src

TEST(LexerTest, NextTokenFullTest) {
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

        10 == 10;
        10 != 9;
        "foobar"
        "foo bar"
        [1, 2];
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
        {TokenType::LSquirly, "{"},
        {TokenType::Ident, "x"},
        {TokenType::Plus, "+"},
        {TokenType::Ident, "y"},
        {TokenType::Semicolon, ";"},
        {TokenType::RSquirly, "}"},
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
        {TokenType::LParen, "("},
        {TokenType::Int, "5"},
        {TokenType::Lt, "<"},
        {TokenType::Int, "10"},
        {TokenType::RParen, ")"},
        {TokenType::LSquirly, "{"},
        {TokenType::Return, "return"},
        {TokenType::True, "true"},
        {TokenType::Semicolon, ";"},
        {TokenType::RSquirly, "}"},
        {TokenType::Else, "else"},
        {TokenType::LSquirly, "{"},
        {TokenType::Return, "return"},
        {TokenType::False, "false"},
        {TokenType::Semicolon, ";"},
        {TokenType::RSquirly, "}"},
        // 10 == 10;
        // 10 != 9;
        {TokenType::Int, "10"},
        {TokenType::Eq, "=="},
        {TokenType::Int, "10"},
        {TokenType::Semicolon, ";"},
        {TokenType::Int, "10"},
        {TokenType::NotEq, "!="},
        {TokenType::Int, "9"},
        {TokenType::Semicolon, ";"},
        // "foobar"
        // "foo bar"
        {TokenType::String, "foobar"},
        {TokenType::String, "foo bar"},
        // [1, 2];
        {TokenType::LBracket, "["},
        {TokenType::Int, "1"},
        {TokenType::Comma, ","},
        {TokenType::Int, "2"},
        {TokenType::RBracket, "]"},
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

