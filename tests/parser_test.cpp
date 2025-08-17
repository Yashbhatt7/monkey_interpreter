#include<gtest/gtest.h>
#include <memory>
#include "../src/lexer.hpp"
#include "../src/ast.hpp"
#include "../src/parser.hpp"

struct PrefixTests {
    std::string input;
    std::string operator_;
    int64_t integerValue;
};

struct InfixTests {
    std::string input;
    int64_t leftValue;
    std::string operator_;
    int64_t rightValue;
};

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

bool testIntegerLiteral(Expression* il, int64_t value) {
    IntegerLiteral* integ = dynamic_cast<IntegerLiteral*>(il);
    if (!integ) {
        ADD_FAILURE() << "il not IntegerLiteral. get=" << typeid(*il).name();
        return false;
    }

    if (integ->Value != value) {
        ADD_FAILURE() << "integ.Value not " << value << ". got=" << integ->Value;
        return false;
    }

    std::string expectedLiteral = std::to_string(value);
    if (integ->TokenLiteral() != expectedLiteral) {
        ADD_FAILURE() << "integ.TokenLiteral not " << value << ". got=" << integ->TokenLiteral();
        return false;
    }

    return true;
}

void checkParserErrors(Parser* p) {
    auto errors = p->Errors();
    if (errors.empty()) {
        return;
    }

    ADD_FAILURE() << "Parser has " << errors.size() << " errors:";

    for (const auto& msg : errors) {
        ADD_FAILURE() << "parser error: \"" << msg <<"\"";
    }

    FAIL();
}

TEST(ParserTest, TestLetStatement) {
    std::string input = R"(
        let x = 5;
        let y = 10;
        let foobar = 838383;
    )";

    auto lexer = std::make_unique<Lexer>(input);
    Parser p(std::move(lexer));

    auto program = p.ParseProgram();
    checkParserErrors(&p);

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
}

TEST(ParserTest, TestReturnStatement) {
    std::string input = R"(
        return 5;
        return 10;
        return 993322;
    )";

    auto lexer = std::make_unique<Lexer>(input);
    Parser p(std::move(lexer));

    auto program = p.ParseProgram();
    checkParserErrors(&p);

    ASSERT_NE(program, nullptr) << "ParserProgram() returned nullptr";

    ASSERT_EQ(program->Statements.size(), 3)
        << "program.Statements does not contain 3 statement. got=" << program->Statements.size();

    for (auto& stmtPtr : program->Statements) {
        auto* returnStmt = dynamic_cast<ReturnStatement*>(stmtPtr.get());
        ASSERT_NE(returnStmt, nullptr)
            << "stmt is not ReturnStatement. got=" << typeid(stmtPtr).name();

        EXPECT_EQ(returnStmt->TokenLiteral(), "return");
    }
}

TEST(ParserTest, TestIdentifierExpression) {
    std::string input = "foobar;";

    auto l = std::make_unique<Lexer>(input);

    Parser p(std::move(l));

    auto program = p.ParseProgram();
    checkParserErrors(&p);

    ASSERT_EQ(program->Statements.size(), 1)
        << "program has not enough statements. got=" << program->Statements.size();

    auto stmt = dynamic_cast<ExpressionStatement*>(program->Statements[0].get());
    ASSERT_NE(stmt, nullptr)
        << "program.Statements[0] is not ExpressionStatement.";

    auto ident = dynamic_cast<Identifier*>(stmt->Expression.get());
    ASSERT_NE(ident, nullptr)
        << "exp not Identifier.";

    EXPECT_EQ(ident->Value, "foobar")
        << "ident.Value not foobar. get=" << ident->Value;

    EXPECT_EQ(ident->TokenLiteral(), "foobar")
        << "ident.TokenLiteral not foobar. got=" << ident->TokenLiteral();
}

TEST(ParserTest, TestIntegerLiteralExpression) {
    std::string input = "5;";

    auto l = std::make_unique<Lexer>(input);
    Parser p(std::move(l));

    auto program = p.ParseProgram();
    checkParserErrors(&p);

    ASSERT_EQ(program->Statements.size(), 1)
        << "program has not enough statements. got=" << program->Statements.size();

    auto stmt = dynamic_cast<ExpressionStatement*>(program->Statements[0].get());
    ASSERT_NE(stmt, nullptr)
        << "program.Statement[0] is not ExpressionStatement.";

    auto literal = dynamic_cast<IntegerLiteral*>(stmt->Expression.get());
    ASSERT_NE(literal, nullptr)
        << "exp not integerliteral.";

    EXPECT_EQ(literal->Value, 5)
        << "literal.Value not " << 5 << ". got=" << literal->Value;

    EXPECT_EQ(literal->TokenLiteral(), "5")
        << "literal.TokenLiteral not " << "5. " << "got=" << literal->TokenLiteral();
}

TEST(ParserTest, TestParsingPrefixExpressions) {
    std::vector<PrefixTests> prefixTests {
        {"!5;", "!", 5},
        {"-15;", "-", 15},
    };

    for (const auto& tt :prefixTests) {
        auto l = std::make_unique<Lexer>(tt.input);
        Parser p(std::move(l));
        auto program = p.ParseProgram();
        checkParserErrors(&p);

        ASSERT_EQ(program->Statements.size(), 1)
            << "program.Statements does not contain 1 statements. got=" << program->Statements.size();

        auto stmt = dynamic_cast<ExpressionStatement*>(program->Statements[0].get());
        ASSERT_NE(stmt, nullptr)
            << "program.Statements[0] is not ExpressionStatement";

        auto exp = dynamic_cast<PrefixExpression*>(stmt->Expression.get());
        ASSERT_NE(exp, nullptr)
            << "stmt is not PrefixExpression";

        EXPECT_EQ(exp->Operator, tt.operator_)
            << "exp.Operator is not" << tt.operator_ << ". got=" << exp->Operator;

        EXPECT_TRUE(testIntegerLiteral(exp->Right.get(), tt.integerValue));
    }
}

TEST(ParserTest, TestParsingInfixExpressions) {
    std::vector<InfixTests> infixTests {
        {"5 + 5;", 5, "+", 5},
        {"5 - 5;", 5, "-", 5},
        {"5 * 5;", 5, "*", 5},
        {"5 / 5;", 5, "/", 5},
        {"5 > 5;", 5, ">", 5},
        {"5 < 5;", 5, "<", 5},
        {"5 == 5;", 5, "==", 5},
        {"5 != 5;", 5, "!=", 5},
    };

    for (const auto& tt : infixTests) {
        auto l = std::make_unique<Lexer>(tt.input);
        Parser p(std::move(l));
        auto program = p.ParseProgram();
        checkParserErrors(&p);

        ASSERT_EQ(program->Statements.size(), 1)
            << "program.Statements does not contain 1 statements. got=" << program->Statements.size();

        auto stmt = dynamic_cast<ExpressionStatement*>(program->Statements[0].get());
        ASSERT_NE(stmt, nullptr)
            << "program.Statements[0] is not ExpressionStatement";

        auto exp = dynamic_cast<InfixExpression*>(stmt->Expression.get());
        ASSERT_NE(exp, nullptr)
            << "exp is not InfixExpression";

        EXPECT_TRUE(testIntegerLiteral(exp->Left.get(), tt.leftValue));

        EXPECT_EQ(exp->Operator, tt.operator_)
            << "exp.Operator is not" << tt.operator_ << ". got=" << exp->Operator;

        EXPECT_TRUE(testIntegerLiteral(exp->Right.get(), tt.rightValue));
    }
}

