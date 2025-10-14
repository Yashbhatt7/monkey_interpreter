#include<cstdint>
#include<gtest/gtest.h>
#include<memory>
#include<variant>
#include "../src/lexer.hpp"
#include "../src/ast.hpp"
#include "../src/parser.hpp"

using Value = std::variant<int64_t, std::string, bool>;
struct PrefixTests {
    std::string input;
    std::string operator_;
    Value integerValue;
};

using Value = std::variant<int64_t, std::string, bool>;
struct InfixTests {
    std::string input;
    Value leftValue;
    std::string operator_;
    Value rightValue;
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

bool testBooleanLiteral(Expression* exp, bool value) {
    BooleanLiteral* boolean = dynamic_cast<BooleanLiteral*>(exp);
    if (!boolean) {
        ADD_FAILURE() << "exp not Boolean. got=" << typeid(*exp).name();
        return false;
    }

    if (boolean->Value != value) {
        ADD_FAILURE() << "bo.Value not " << (value ? "true" : "false")
            << ". got=" << (boolean->Value ? "true" : "false");
        return false;
    }

    std::string expectedLiteral = value ? "true" : "false";
    if (boolean->TokenLiteral() != expectedLiteral) {
        ADD_FAILURE() << "boolean.TokenLiteral not " << expectedLiteral
            << ". got=" << boolean->TokenLiteral();
        return false;
    }

    return true;
}

using LiteralValue = std::variant<int64_t, std::string, bool>;
bool testLiteralExpression(Expression* exp, const LiteralValue& expected) {
    return std::visit([exp](const auto& value) -> bool {
        using T = std::decay_t<decltype(value)>;
        if constexpr (std::is_same_v<T, int>) {
            return testIntegerLiteral(exp, static_cast<int64_t>(value));
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return testIntegerLiteral(exp, value);
        } else if constexpr (std::is_same_v<T, std::string>) {
            return testIdentifier(exp, value);
        } else if constexpr (std::is_same_v<T, bool>) {
            return testBooleanLiteral(exp, value);
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
    struct TestCase {
        std::string input;
        std::string expectedIdentifier;
        std::variant<int64_t, std::string, bool> expectedValue;
    };

    std::vector<TestCase> tests = {
        {"let x = 5;", "x", int64_t(5)},
        {"let y = true;", "y", true},
        {"let foobar = y;", "foobar", std::string("y")}
    };

    for (const auto& tt : tests) {
        auto lexer = std::make_unique<Lexer>(tt.input);
        Parser p(std::move(lexer));
        auto program = p.ParseProgram();

        checkParserErrors(&p);

        ASSERT_EQ(program->Statements.size(), 1)
            << "program.Statements does not contain 1 statements. got=" << program->Statements.size();

        auto stmt = program->Statements[0].get();

        ASSERT_NE(stmt, nullptr)
            << "Statement is null";

        auto letStmt = dynamic_cast<LetStatement*>(stmt);
        if (letStmt == nullptr) {
            FAIL() << "Failed to cast to LetStatement. Actual type: " << typeid(*stmt).name();
        }

        ASSERT_TRUE(testLetStatement(stmt, tt.expectedIdentifier));

        ASSERT_NE(letStmt->Value.get(), nullptr)
            << "LetStatement Value is null";

        ASSERT_TRUE(testLiteralExpression(letStmt->Value.get(), tt.expectedValue));
    }
}

TEST(ParserTest, TestReturnStatement) {
    struct TestCase {
        std::string input;
        std::variant<int64_t, std::string, bool> expectedValue;
    };

    std::vector<TestCase> tests = {
        {"return 5;", int64_t(5)},
        {"return true;", true},
        {"return foobar;", std::string("foobar")}
    };

    for (const auto& tt : tests) {
        auto lexer = std::make_unique<Lexer>(tt.input);
        Parser p(std::move(lexer));
        auto program = p.ParseProgram();

        checkParserErrors(&p);

        ASSERT_EQ(program->Statements.size(), 1)
            << "program.Statements does not contain 1 statements. got="
            << program->Statements.size();

        auto stmt = program->Statements[0].get();

        auto returnStmt = dynamic_cast<ReturnStatement*>(stmt);
        ASSERT_NE(returnStmt, nullptr)
            << "stmt not ReturnStatement. got=" << typeid(*stmt).name();

        ASSERT_EQ(returnStmt->TokenLiteral(), "return")
            << "returnStmt.TokenLiteral not 'return', got " << returnStmt->TokenLiteral();

        // Check if ReturnValue exists before testing it
        ASSERT_NE(returnStmt->ReturnValue.get(), nullptr)
            << "ReturnStatement ReturnValue is null";

        ASSERT_TRUE(testLiteralExpression(returnStmt->ReturnValue.get(), tt.expectedValue))
            << "testLiteralExpression failed for input: " << tt.input;
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

    auto ident = dynamic_cast<Identifier*>(stmt->ExpressionPtr.get());
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

    auto literal = dynamic_cast<IntegerLiteral*>(stmt->ExpressionPtr.get());
    ASSERT_NE(literal, nullptr)
        << "exp not integerliteral.";

    EXPECT_EQ(literal->Value, 5)
        << "literal.Value not " << 5 << ". got=" << literal->Value;

    EXPECT_EQ(literal->TokenLiteral(), "5")
        << "literal.TokenLiteral not " << "5. " << "got=" << literal->TokenLiteral();
}

TEST(ParserTest, TestParsingPrefixExpressions) {
    std::vector<PrefixTests> prefixTests {
        { "!5;", "!", 5 },
        { "-15;", "-", 15 },
        { "!true;", "!", true },
        { "!false", "!", false },
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

        auto exp = dynamic_cast<PrefixExpression*>(stmt->ExpressionPtr.get());
        ASSERT_NE(exp, nullptr)
            << "stmt is not PrefixExpression";

        EXPECT_EQ(exp->Operator, tt.operator_)
            << "exp.Operator is not" << tt.operator_ << ". got=" << exp->Operator;

        EXPECT_TRUE(testLiteralExpression(exp->Right.get(), tt.integerValue));
    }
}

TEST(ParserTest, TestParsingInfixExpressions) {
    std::vector<InfixTests> infixTests {
        { "5 + 4;", 5, "+", 4 },
        { "5 - 4;", 5, "-", 4 },
        { "5 * 4;", 5, "*", 4 },
        { "5 / 4;", 5, "/", 4 },
        { "5 > 4;", 5, ">", 4 },
        { "5 < 4;", 5, "<", 4 },
        { "5 == 4;", 5, "==", 4 },
        { "5 != 4;", 5, "!=", 4 },
        { "true == true", true, "==", true },
        { "true != false", true, "!=", false },
        { "false == false", false, "==", false },
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

        auto exp = dynamic_cast<InfixExpression*>(stmt->ExpressionPtr.get());
        ASSERT_NE(exp, nullptr)
            << "exp is not InfixExpression";

        EXPECT_TRUE(testLiteralExpression(exp->Left.get(), tt.leftValue))
            << "Failed testing left operand for input: " << tt.input;


        EXPECT_EQ(exp->Operator, tt.operator_)
            << "exp.Operator is not" << tt.operator_ << ". got=" << exp->Operator;

        EXPECT_TRUE(testLiteralExpression(exp->Right.get(), tt.rightValue))
            << "Failed testing right operand for input: " << tt.input;
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
        {
            "1 + (2 + 3) + 4",
            "((1 + (2 + 3)) + 4)",
        },
        {
            "(5 + 5) * 2",
            "((5 + 5) * 2)",
        },
        {
            "2 / (5 + 5)",
            "(2 / (5 + 5))",
        },
        {
            "-(5 + 5)",
            "(-(5 + 5))",
        },
        {
            "!(true == true)",
            "(!(true == true))",
        },
        {
            "a + add(b * c) + d",
            "((a + add((b * c))) + d)",
        },
        {
            "add(a, b, 1, 2 * 3, 4 + 5, add(6, 7 * 8))",
            "add(a, b, 1, (2 * 3), (4 + 5), add(6, (7 * 8)))",
        },
        {
            "add(a + b + c * d / f + g)",
            "add((((a + b) + ((c * d) / f)) + g))",
        },
        {
            "a * [1, 2, 3, 4][b * c] * d",
            "((a * ([1, 2, 3, 4][(b * c)])) * d)",
        },
        {
            "add(a * b[2], b[1], 2 * [1, 2][1])",
            "add((a * (b[2])), (b[1]), (2 * ([1, 2][1])))",
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

TEST(ParserTest, TestBooleanLiteral) {
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

        auto exp = dynamic_cast<BooleanLiteral*>(stmt->ExpressionPtr.get());
        ASSERT_NE(exp, nullptr)
            << "exp is not Boolean. got=";

        EXPECT_EQ(exp->Value, tt.expectedBoolean)
            << "exp.Value is not" << tt.expectedBoolean << ". got=" << exp->Value;
    }
}

TEST(ParserTest, TestIfExpression) {
    std::string input = R"(
        if (x < y) { x }
    )";

    auto l = std::make_unique<Lexer>(input);
    Parser p(std::move(l));
    auto program = p.ParseProgram();
    checkParserErrors(&p);

    ASSERT_EQ(program->Statements.size(), 1)
        << "program.Body does not contain 1 statements. got=" << program->Statements.size();

    auto stmt = dynamic_cast<ExpressionStatement*>(program->Statements[0].get());
    ASSERT_TRUE(stmt != nullptr)
        << "program.Statements[0] is not ExpressionStatement";

    auto exp = dynamic_cast<IfExpression*>(stmt->ExpressionPtr.get());
    ASSERT_TRUE(exp != nullptr)
        << "stmt.Expression is not IfExpression.";

    EXPECT_TRUE(testInfixExpression(exp->Condition.get(), "x", "<", "y"))
        << "Failed Testing";

    ASSERT_EQ(exp->Consequence->Statements.size(), 1)
        << "consequence is not 1 statements. got=" << exp->Consequence->Statements.size();

    auto consequence = dynamic_cast<ExpressionStatement*>(exp->Consequence->Statements[0].get());
    ASSERT_TRUE(consequence != nullptr)
        << "Statements[0] is not ExpressionStatement";

    EXPECT_TRUE(testIdentifier(consequence->ExpressionPtr.get(), "x"))
        << "Failed Testing";

    EXPECT_EQ(exp->Alternative, nullptr)
        << "exp.Alternative.Statements was not nil. got=" << exp->Alternative.get();
}

TEST(ParserTest, TestIfElseExpression) {
    std::string input = R"(
        if (x < y) { x } else { y }
    )";

    auto l = std::make_unique<Lexer>(input);
    Parser p(std::move(l));
    auto program = p.ParseProgram();
    checkParserErrors(&p);

    ASSERT_EQ(program->Statements.size(), 1)
        << "program.Body does not contain 1 statements. got=" << program->Statements.size();

    auto stmt = dynamic_cast<ExpressionStatement*>(program->Statements[0].get());
    ASSERT_TRUE(stmt != nullptr)
        << "program.Statements[0] is not ExpressionStatement";

    auto exp = dynamic_cast<IfExpression*>(stmt->ExpressionPtr.get());
    ASSERT_TRUE(exp != nullptr)
        << "stmt.Expression is not IfExpression.";

    EXPECT_TRUE(testInfixExpression(exp->Condition.get(), "x", "<", "y"))
        << "Failed Testing";

    ASSERT_EQ(exp->Consequence->Statements.size(), 1)
        << "consequence is not 1 statements. got=" << exp->Consequence->Statements.size();

    auto consequence = dynamic_cast<ExpressionStatement*>(exp->Consequence->Statements[0].get());
    ASSERT_TRUE(consequence != nullptr)
        << "Statements[0] is not ExpressionStatement";

    EXPECT_TRUE(testIdentifier(consequence->ExpressionPtr.get(), "x"))
        << "Failed Testing";

    ASSERT_TRUE(exp->Alternative != nullptr)
        << "exp.Alternative is null";

    ASSERT_EQ(exp->Alternative->Statements.size(), 1)
        << "exp.Alternative.Statements does not contain 1 statements";

    auto alternative = dynamic_cast<ExpressionStatement*>(exp->Alternative->Statements[0].get());
    ASSERT_TRUE(alternative != nullptr)
        << "Statements[0] is not ExpressionStatement";

    EXPECT_TRUE(testIdentifier(alternative->ExpressionPtr.get(), "y"))
        << "Failed Testing";
}

TEST(ParserTest, TestFunctionLiteralParsing) {
    std::string input = "fn(x, y) { x + y; }";

    auto l = std::make_unique<Lexer>(input);
    Parser p(std::move(l));
    auto program = p.ParseProgram();
    checkParserErrors(&p);

    ASSERT_EQ(program->Statements.size(), 1)
        << "program.Body does not contain 1 statements. got=" << program->Statements.size();

    auto stmt = dynamic_cast<ExpressionStatement*>(program->Statements[0].get());
    ASSERT_NE(stmt, nullptr)
        << "program.Statements[0] is not ExpressionStatement.";

    auto function = dynamic_cast<FunctionLiteral*>(stmt->ExpressionPtr.get());
    ASSERT_NE(function, nullptr)
        << "stmt.Expression is not FunctionLiteral.";

    ASSERT_EQ(function->Parameters.size(), 2)
        << "function literal parameters wrong. want 2, got=" << function->Parameters.size();

    testLiteralExpression(function->Parameters[0].get(), "x");
    testLiteralExpression(function->Parameters[1].get(), "y");

    ASSERT_EQ(function->Body->Statements.size(), 1)
        << "function.Body.Statements has not 1 statements. got="
        << function->Body->Statements.size();

    auto bodyStmt = dynamic_cast<ExpressionStatement*>(function->Body->Statements[0].get());
    ASSERT_NE(bodyStmt, nullptr)
        << "function body stmt is not ExpressionStatement.";

    testInfixExpression(bodyStmt->ExpressionPtr.get(), "x", "+", "y");
}

TEST(ParserTest, TestFunctionParameterParsing) {
    struct TestCase {
        std::string input;
        std::vector<std::string> expectedParams;
    };

    std::vector<TestCase> tests = {
        {"fn() {};", {}},
        {"fn(x) {};", {"x"}},
        {"fn(x, y, z) {};", {"x", "y", "z"}}
    };

    for (const auto& tt : tests) {
        auto lexer = std::make_unique<Lexer>(tt.input);
        Parser p(std::move(lexer));
        auto program = p.ParseProgram();

        checkParserErrors(&p);

        auto stmt = dynamic_cast<ExpressionStatement*>(program->Statements[0].get());
        ASSERT_NE(stmt, nullptr);

        auto function = dynamic_cast<FunctionLiteral*>(stmt->ExpressionPtr.get());
        ASSERT_NE(function, nullptr);

        EXPECT_EQ(function->Parameters.size(), tt.expectedParams.size())
            << "Parameter count mismatch for input: " << tt.input;

        for (size_t i = 0; i < tt.expectedParams.size(); ++i) {
            testLiteralExpression(function->Parameters[i].get(), tt.expectedParams[i]);
        }
    }
}

TEST(ParserTest, TestCallExpressionParsing) {
    std::string input = "add(1, 2 * 3, 4 + 5);";

    auto lexer = std::make_unique<Lexer>(input);
    Parser p(std::move(lexer));
    auto program = p.ParseProgram();

    checkParserErrors(&p);

    ASSERT_EQ(program->Statements.size(), 1)
        << "program.statements does not contain 1 statement. got=" << program->Statements.size();

    auto stmt = dynamic_cast<ExpressionStatement*>(program->Statements[0].get());
    ASSERT_NE(stmt, nullptr)
        << "stmt is not ExpressionStatement.";

    auto exp = dynamic_cast<CallExpression*>(stmt->ExpressionPtr.get());
    ASSERT_NE(exp, nullptr)
        << "stmt.expression is not CallExpression.";

    if (!testIdentifier(exp->Function.get(), "add")) {
        return;
    }

    ASSERT_EQ(exp->Arguments.size(), 3)
        << "wrong length of arguments. got=" << exp->Arguments.size();

    testLiteralExpression(exp->Arguments[0].get(), 1);
    testInfixExpression(exp->Arguments[1].get(), 2, "*", 3);
    testInfixExpression(exp->Arguments[2].get(), 4, "+", 5);
}

TEST(ParserTest, TestCallExpressionParameterParsing) {
    struct TestCase {
        std::string input;
        std::string expectedIdent;
        std::vector<std::string> expectedArgs;
    };

    std::vector<TestCase> tests = {
        {
            "add();",
            "add",
            {}
        },
        {
            "add(1);",
            "add",
            {"1"}
        },
        {
            "add(1, 2 * 3, 4 + 5);",
            "add",
            {"1", "(2 * 3)", "(4 + 5)"}
        }
    };

    for (const auto& tt : tests) {
        auto lexer = std::make_unique<Lexer>(tt.input);
        Parser p(std::move(lexer));
        auto program = p.ParseProgram();

        checkParserErrors(&p);

        auto stmt = dynamic_cast<ExpressionStatement*>(program->Statements[0].get());
        ASSERT_NE(stmt, nullptr);

        auto exp = dynamic_cast<CallExpression*>(stmt->ExpressionPtr.get());
        ASSERT_NE(exp, nullptr)
            << "stmt.expression is not CallExpression.";

        if (!testIdentifier(exp->Function.get(), tt.expectedIdent)) {
            return;
        }

        ASSERT_EQ(exp->Arguments.size(), tt.expectedArgs.size())
            << "wrong number of arguments. want=" << tt.expectedArgs.size()
            << ", got=" << exp->Arguments.size();

        for (size_t i = 0; i < tt.expectedArgs.size(); ++i) {
            EXPECT_EQ(exp->Arguments[i]->String(), tt.expectedArgs[i])
                << "argument " << i << " wrong. want=\"" << tt.expectedArgs[i]
                << "\", got=\"" << exp->Arguments[i]->String() << "\"";
        }
    }
}

TEST(ParserTest, TestStringLiteralExpression) {
    std::string input = R"("hello world";)";

    auto l = std::make_unique<Lexer>(input);
    Parser p(std::move(l));
    auto program = p.ParseProgram();

    checkParserErrors(&p);

    ASSERT_EQ(program->Statements.size(), 1)
        << "program has not enough statements. got=" << program->Statements.size();

    auto stmt = dynamic_cast<ExpressionStatement*>(program->Statements[0].get());
    ASSERT_NE(stmt, nullptr)
        << "program.Statements[0] is not ExpressionStatement.";

    auto literal = dynamic_cast<StringLiteral*>(stmt->ExpressionPtr.get());
    ASSERT_NE(literal, nullptr)
        << "exp not StringLiteral. got=" << typeid(*stmt->ExpressionPtr.get()).name();

    EXPECT_EQ(literal->Value, "hello world")
        << "literal.Value not 'hello world'. got=" << literal->Value;
}

TEST(ParserTest, TestParsingArrayLiterals) {
    std::string input = "[1, 2 * 2, 3 + 3]";

    auto l = std::make_unique<Lexer>(input);
    Parser p(std::move(l));
    auto program = p.ParseProgram();
    checkParserErrors(&p);

    ASSERT_EQ(program->Statements.size(), 1)
        << "program has not enough statements. got=" << program->Statements.size();

    auto stmt = dynamic_cast<ExpressionStatement*>(program->Statements[0].get());
    ASSERT_NE(stmt, nullptr)
        << "program.Statements[0] is not ExpressionStatement.";

    auto array = dynamic_cast<ArrayLiteral*>(stmt->ExpressionPtr.get());
    ASSERT_NE(array, nullptr)
        << "exp not ArrayLiteral. got=" << typeid(*stmt->ExpressionPtr.get()).name();

    ASSERT_EQ(array->Elements.size(), 3)
        << "len(array.Elements) not 3. got=" << array->Elements.size();

    EXPECT_TRUE(testIntegerLiteral(array->Elements[0].get(), 1));
    EXPECT_TRUE(testInfixExpression(array->Elements[1].get(), 2, "*", 2));
    EXPECT_TRUE(testInfixExpression(array->Elements[2].get(), 3, "+", 3));
}

TEST(ParserTest, TestParsingIndexExpressions) {
    std::string input = "myArray[1 + 1]";

    auto l = std::make_unique<Lexer>(input);
    Parser p(std::move(l));
    auto program = p.ParseProgram();
    checkParserErrors(&p);

    ASSERT_EQ(program->Statements.size(), 1)
        << "program has not enough statements. got=" << program->Statements.size();

    auto stmt = dynamic_cast<ExpressionStatement*>(program->Statements[0].get());
    ASSERT_NE(stmt, nullptr)
        << "program.Statements[0] is not ExpressionStatement.";

    auto indexExp = dynamic_cast<IndexExpression*>(stmt->ExpressionPtr.get());
    ASSERT_NE(indexExp, nullptr)
        << "exp not IndexExpression. got=" << typeid(*stmt->ExpressionPtr.get()).name();

    EXPECT_TRUE(testIdentifier(indexExp->Left.get(), "myArray"));
    EXPECT_TRUE(testInfixExpression(indexExp->Index.get(), 1, "+", 1));
}

TEST(ParserTest, TestParsingHashLiteralsStringKeys) {
    std::string input = R"({"one": 1, "two": 2, "three": 3})";

    auto l = std::make_unique<Lexer>(input);
    Parser p(std::move(l));
    auto program = p.ParseProgram();
    checkParserErrors(&p);

    ASSERT_EQ(program->Statements.size(), 1);

    auto stmt = dynamic_cast<ExpressionStatement*>(program->Statements[0].get());
    ASSERT_NE(stmt, nullptr);

    auto hash = dynamic_cast<HashLiteral*>(stmt->ExpressionPtr.get());
    ASSERT_NE(hash, nullptr)
        << "exp is not HashLiteral. got=" << typeid(*stmt->ExpressionPtr.get()).name();

    ASSERT_EQ(hash->Pairs.size(), 3)
        << "hash.Pairs has wrong length. got=" << hash->Pairs.size();

    std::unordered_map<std::string, int64_t> expected = {
        {"one", 1},
        {"two", 2},
        {"three", 3}
    };

    for (const auto& pair : hash->Pairs) {
        auto literal = dynamic_cast<StringLiteral*>(pair.first);
        ASSERT_NE(literal, nullptr)
            << "key is not StringLiteral. got=" << typeid(*pair.first).name();

        int64_t expectedValue = expected[literal->String()];
        EXPECT_TRUE(testIntegerLiteral(pair.second.get(), expectedValue));
    }
}

TEST(ParserTest, TestParsingEmptyHashLiteral) {
    std::string input = "{}";

    auto l = std::make_unique<Lexer>(input);
    Parser p(std::move(l));
    auto program = p.ParseProgram();
    checkParserErrors(&p);

    auto stmt = dynamic_cast<ExpressionStatement*>(program->Statements[0].get());
    ASSERT_NE(stmt, nullptr);

    auto hash = dynamic_cast<HashLiteral*>(stmt->ExpressionPtr.get());
    ASSERT_NE(hash, nullptr)
        << "exp is not HashLiteral. got=" << typeid(*stmt->ExpressionPtr.get()).name();

    EXPECT_EQ(hash->Pairs.size(), 0)
        << "hash.Pairs has wrong length. got=" << hash->Pairs.size();
}

TEST(ParserTest, TestParsingHashLiteralsWithExpressions) {
    std::string input = R"({"one": 0 + 1, "two": 10 - 8, "three": 15 / 5})";

    auto l = std::make_unique<Lexer>(input);
    Parser p(std::move(l));
    auto program = p.ParseProgram();
    checkParserErrors(&p);

    auto stmt = dynamic_cast<ExpressionStatement*>(program->Statements[0].get());
    ASSERT_NE(stmt, nullptr);

    auto hash = dynamic_cast<HashLiteral*>(stmt->ExpressionPtr.get());
    ASSERT_NE(hash, nullptr);

    ASSERT_EQ(hash->Pairs.size(), 3);

    std::unordered_map<std::string, std::function<void(Expression*)>> tests = {
        {"one", [](Expression* e) {
            EXPECT_TRUE(testInfixExpression(e, int64_t(0), "+", int64_t(1)));
        }},

        {"two", [](Expression* e) {
            EXPECT_TRUE(testInfixExpression(e, int64_t(10), "-", int64_t(8)));
        }},

        {"three", [](Expression* e) {
            EXPECT_TRUE(testInfixExpression(e, int64_t(15), "/", int64_t(5)));
        }}
    };

    for (const auto& pair : hash->Pairs) {
        auto literal = dynamic_cast<StringLiteral*>(pair.first);
        ASSERT_NE(literal, nullptr);

        auto testFunc = tests[literal->String()];
        testFunc(pair.second.get());
    }
}

