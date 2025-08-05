#include<gtest/gtest.h>
#include <memory>
#include "../src/lexer.hpp"
#include "../src/ast.hpp"
#include "../src/parser.hpp"

// Helper function to test let statement
bool testLetStatement(Statement* s, const std::string& name) {
    if (s->TokenLiteral() != "let") {
        ADD_FAILURE() << "s.TokenLiteral not 'let'. got=" << s->TokenLiteral();
        return false;
    }

    // Try to cast to LetStatement
    LetStatement* letStmt = dynamic_cast<LetStatement*>(s);
    if (!letStmt) {
        ADD_FAILURE() << "s not LetStatement. got=" << typeid(*s).name();
        return false;
    }

    if (letStmt->Name->Value != name) {
        ADD_FAILURE() << "letStmt.Name.Value not '" << name << "'. got=" << letStmt->Name->Value;
        return false;
    }

    if (letStmt->Name->TokenLiteral() != name) {
        ADD_FAILURE() << "s.Name not '" << name << "'. got=" << letStmt->Name->TokenLiteral();
        return false;
    }

    return true;
}

TEST(ParserTest, TestLetStatement) {
    std::string input = R"(
        let x = 5;
        let y = 10;
        let foobar = 838383;
    )";

    auto lexer = std::make_unique<Lexer> (input);
    Parser p(std::move(lexer));

    auto program = p.ParseProgram();

    ASSERT_NE(program, nullptr) << "ParserProgram() returned nullptr";

    ASSERT_EQ(program->Statements.size(), 3)
        << "program.Statements does not contain 3 statement. got=" << program->Statements.size();

    struct TestCase {
        std::string expectedIdentifier;
    };

    std::vector<TestCase> tests = {
        {"x"},
        {"y"},
        {"foobar"},
    };

    for (size_t i = 0; i < tests.size(); ++i) {
        const auto& tt = tests[i];
        Statement* stmt = program->Statements[i].get();

        EXPECT_TRUE(testLetStatement(stmt, tt.expectedIdentifier))
            << "Test [" << i << "] - Let statement test failed for identifier: " << tt.expectedIdentifier;
    }

    // Check for parser errors
    // auto errors = p->Errors();
}


