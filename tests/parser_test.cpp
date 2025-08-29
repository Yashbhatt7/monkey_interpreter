#include <cstdint>
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

struct TestCase {
    std::string input;
    std::string expected;
};

struct TestCs {
    std::string input;
    bool expectedBoolean;
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

bool testIdentifier(Expression* exp, const std::string& value) {
    auto ident = dynamic_cast<Identifier*> (exp);
    if (!ident) {
        ADD_FAILURE() << "exp not Identifier. got=" << typeid(*exp).name();
        return false;
    }

    if (ident->Value != value) {
        ADD_FAILURE() << "ident.Value not " << value << ". got=" << ident->Value;
        return false;
    }

    if (ident->TokenLiteral() != value) {
        ADD_FAILURE() << "ident.TokenLiteral not " << value << ". got=" << ident->TokenLiteral();
        return false;
    }

    return true;
}

using LiteralValue = std::variant<int, int64_t, std::string>;
bool testLiteralExpression(Expression* exp, const LiteralValue& expected) {
    return std::visit([exp](const auto& value) -> bool {
        using T = std::decay_t<decltype(value)>;
        if constexpr (std::is_same_v<T, int>) {
            return testIntegerLiteral(exp, static_cast<int64_t>(value));
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return testIntegerLiteral(exp, value);
        } else if constexpr (std::is_same_v<T, std::string>) {
            return testIdentifier(exp, value);
        } else {
            ADD_FAILURE() << "type of expected not handled. got=" << typeid(T).name();
            return false;
        }
    }, expected);
}

template<typename LeftType, typename RightType>
bool testInfixExpression(Expression* exp, const LeftType& left, const std::string& operator_str, const RightType& right) {
    InfixExpression* opExp = dynamic_cast<InfixExpression*> (exp);
    if (!opExp) {
        ADD_FAILURE() << "exp is not InfixExpression. got=" << typeid(*exp).name() << "(" << exp->String() << ")";
        return false;
    }

    if (!testLiteralExpression(opExp->Left.get(), left)) {
        return false;
    }

    if (opExp->Operator != operator_str) {
        ADD_FAILURE() << "exp.Operator is not '" << operator_str << "'. got=" << opExp->Operator;
        return false;
    }

    if (!testLiteralExpression(opExp->Right.get(), right)) {
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
        {"5 + 4;", 5, "+", 4},
        {"5 - 4;", 5, "-", 4},
        {"5 * 4;", 5, "*", 4},
        {"5 / 4;", 5, "/", 4},
        {"5 > 4;", 5, ">", 4},
        {"5 < 4;", 5, "<", 4},
        {"5 == 4;", 5, "==", 4},
        {"5 != 4;", 5, "!=", 4},
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

TEST(ParserTest, TestOperatorPrecedenceParsing) {
    std::vector<TestCase> tests = {
        {
            "-a * b",
            "((-a) * b)",
        },
        {
            "!-a",
            "(!(-a))",
        },
        {
            "a + b + c",
            "((a + b) + c)",
        },
        {
            "a + b - c",
            "((a + b) - c)",
        },
        {
            "a * b * c",
            "((a * b) * c)",
        },
        {
            "a * b / c",
            "((a * b) / c)",
        },
        {
            "a + b / c",
            "(a + (b / c))",
        },
        {
            "a + b * c + d / e - f",
            "(((a + (b * c)) + (d / e)) - f)",
        },
        {
            "3 + 4; -5 * 5",
            "(3 + 4)((-5) * 5)",
        },
        {
            "5 > 4 == 3 < 4",
            "((5 > 4) == (3 < 4))",
        },
        {
            "5 < 4 != 3 > 4",
            "((5 < 4) != (3 > 4))",
        },
        {
            "3 + 4 * 5 == 3 * 1 + 4 * 5",
            "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))",
        },
        {
            "3 + 4 * 5 == 3 * 1 + 4 * 5",
            "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))",
        },
        {
            "true",
            "true",
        },
        {
            "false",
            "false",
        },
        {
            "3 > 5 == false",
            "((3 > 5) == false)",
        },
        {
            "3 < 5 == true",
            "((3 < 5) == true)",
        },
    };

    for (const auto& tt : tests) {
        auto l = std::make_unique<Lexer>(tt.input);
        Parser p(std::move(l));
        auto program = p.ParseProgram();
        checkParserErrors(&p);

        std::string actual = program->String();

        EXPECT_EQ(tt.expected, actual)
            << "expected " << tt.expected << " got=" << actual;
    }
}

TEST(ParserTest, TestBooleanExpression) {
    std::vector<TestCs> tests = {
        { "true", true },
        { "false", false },
    };

    for (const auto& tt : tests) {
        auto l = std::make_unique<Lexer>(tt.input);
        Parser p(std::move(l));
        auto program = p.ParseProgram();
        checkParserErrors(&p);

        ASSERT_EQ(program->Statements.size(), 1)
            << "prgram.Statements does not contain 1 statements. got=" << program->Statements.size();

        auto stmt = dynamic_cast<ExpressionStatement*>(program->Statements[0].get());
        ASSERT_NE(stmt, nullptr)
            << "program.Statements[0] is not ExpressionStatement";

        auto exp = dynamic_cast<Boolean*>(stmt->Expression.get());
        ASSERT_NE(exp, nullptr)
            << "exp is not Boolean. got=";

        EXPECT_EQ(exp->Value, tt.expectedBoolean)
            << "exp.Value is not" << tt.expectedBoolean << ". got=" << exp->Value;
    }
}

