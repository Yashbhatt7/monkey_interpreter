#include<gtest/gtest.h>
#include <memory>
#include "../src/ast.hpp"
#include "../src/token.hpp"

TEST(ASTTest, TestString) {
    auto program = std::make_unique<Program>();

    auto letStmt = std::make_unique<LetStatement>();
    letStmt->token = Token{ TokenType::Let, "let" };

    auto nameIdent = std::make_unique<Identifier>();
    nameIdent->token = Token{ TokenType::Ident, "myVar" };
    nameIdent->Value = "myVar";
    letStmt->Name = std::move(nameIdent);

    auto valueIdent = std::make_unique<Identifier>();
    valueIdent->token = Token{ TokenType::Ident, "anotherVar" };
    valueIdent->Value = "anotherVar";
    letStmt->Value = std::move(valueIdent);

    program->Statements.push_back(std::move(letStmt));

    std::string expected = "let myVar = anotherVar;";
    std::string actual = program->String();

    EXPECT_EQ(expected, actual) << "program.String() wrong. got=" << actual;
}


