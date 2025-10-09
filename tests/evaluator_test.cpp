#include<gtest/gtest.h>
#include<vector>
#include<string>
#include<memory>
#include "../src/lexer.hpp"
#include "../src/parser.hpp"
#include "../src/object.hpp"
#include "../src/evaluator.hpp"

std::unique_ptr<Object> testEval(const std::string& input) {
    auto lexer = std::make_unique<Lexer>(input);
    Parser parser(std::move(lexer));
    auto program = parser.ParseProgram();
    return Eval(program.get());
}

bool testIntegerObject(Object* obj, int64_t expected) {
    auto result = dynamic_cast<Integer*>(obj);
    if (!result) {
        std::cerr << "object is not Integer. got=" << typeid(*obj).name() << std::endl;
        return false;
    }

    if (result->Value != expected) {
        std::cerr << "object has wrong value. got=" << result->Value
            << ", want=" << expected << std::endl;
        return false;
    }

    return true;
}

bool testBooleanObject(Object* obj, bool expected) {
    auto result = dynamic_cast<Boolean*>(obj);

    if (!result) {
        std::cerr << "object is not Boolean. got=" << typeid(*obj).name() << std::endl;
        return false;
    }

    if (result->Value != expected) {
        std::cerr << "object has wrong value. got=" << result->Value
            << ", want= " << expected << "\n";
        return false;
    }
    return true;
}

bool testNullObject(Object* obj) {
    if (!dynamic_cast<Null*>(obj)) {
        std::cerr << "object is not NULL. got=" << typeid(*obj).name() << "\n";
        return false;
    }
    return true;
}

TEST(EvaluatorTest, TestEvalIntegerExpression) {
    struct TestCase {
        std::string input;
        int64_t expected;
    };

    std::vector<TestCase> tests = {
        {"5", 5},
        {"10", 10},
        {"-5", -5},
        {"-10", -10},
        {"5 + 5 + 5 + 5 - 10", 10},
        {"2 * 2 * 2 * 2 * 2", 32},
        {"-50 + 100 + -50", 0},
        {"5 * 2 + 10", 20},
        {"5 + 2 * 10", 25},
        {"20 + 2 * -10", 0},
        {"50 / 2 * 2 + 10", 60},
        {"2 * (5 + 10)", 30},
        {"3 * 3 * 3 + 10", 37},
        {"3 * (3 * 3) + 10", 37},
        {"(5 + 10 * 2 + 15 / 3) * 2 + -10", 50},
    };

    for (const auto& tt : tests) {
        auto evaluated = testEval(tt.input);
        ASSERT_TRUE(testIntegerObject(evaluated.get(), tt.expected));
    }
}

TEST(EvaluatorTest, TestEvalBooleanExpression) {
    struct TestCase {
        std::string input;
        bool expected;
    };

    std::vector<TestCase> tests = {
        {"true", true},
        {"false", false},
        {"1 < 2", true},
        {"1 > 2", false},
        {"1 < 1", false},
        {"1 > 1", false},
        {"1 == 1", true},
        {"1 != 1", false},
        {"1 == 2", false},
        {"1 != 2", true},
        {"true == true", true},
        {"false == false", true},
        {"true == false", false},
        {"true != false", true},
        {"false != true", true},
        {"(1 < 2) == true", true},
        {"(1 < 2) == false", false},
        {"(1 > 2) == true", false},
        {"(1 > 2) == false", true},
    };

    for (const auto& tt : tests) {
        auto evaluated = testEval(tt.input);
        ASSERT_TRUE(testBooleanObject(evaluated.get(), tt.expected));
    }
}

TEST(EvaluatorTest, TestBangOperator) {
    struct TestCase {
        std::string input;
        bool expected;
    };

    std::vector<TestCase> tests = {
        {"!true", false},
        {"!false", true},
        {"!5", false},
        {"!!true", true},
        {"!!false", false},
        {"!!5", true},
    };

    for (const auto& tt : tests) {
        auto evaluated = testEval(tt.input);
        EXPECT_TRUE(testBooleanObject(evaluated.get(), tt.expected))
            << "Failed testing input: " << tt.input;
    }
}

TEST(EvaluatorTest, TestIfElseExpressions) {
    struct TestCase {
        std::string input;
        std::optional<int64_t> expected;
    };

    std::vector<TestCase> tests = {
        {"if (true) { 10 }", 10},
        {"if (false) { 10 }", std::nullopt},
        {"if (1) { 10 }", 10},
        {"if (1 < 2) { 10 }", 10},
        {"if (1 > 2) { 10 }", std::nullopt},
        {"if (1 > 2) { 10 } else { 20 }", 20},
        {"if (1 < 2) { 10 } else { 20 }", 10},
    };

    for (const auto& tt : tests) {
        auto evaluated = testEval(tt.input);
        if (tt.expected.has_value()) {
            ASSERT_TRUE(testIntegerObject(evaluated.get(), tt.expected.value()));
        } else {
            ASSERT_TRUE(testNullObject(evaluated.get()));
        }
    }
}

TEST(EvaluatorTest, TestReturnStatements) {
    struct TestCase {
        std::string input;
        int64_t expected;
    };

    std::vector<TestCase> tests = {
        {"return 10;", 10},
        {"return 10; 9;", 10},
        {"return 2 * 5; 9;", 10},
        {"9; return 2 * 5; 9;", 10},
        { R"(
            if (10 > 1) {
                if (10 > 1) {
                    return 10;
                }
                return 1;
            }
        )", 10},
    };

    for (const auto& tt : tests) {
        auto evaluated = testEval(tt.input);
        EXPECT_TRUE(testIntegerObject(evaluated.get(), tt.expected))
            << "Failed testing input: " << tt.input;
    }
}

TEST(EvaluatorTest, ErrorHandling) {
    struct TestCase {
        std::string input;
        std::string expectedMessage;
    };

    std::vector<TestCase> tests = {
        {
            "5 + true;",
            "type mismatch: INTEGER + BOOLEAN"
        },
        {
            "5 + true; 5;",
            "type mismatch: INTEGER + BOOLEAN"
        },
        {
            "-true",
            "unknown operator: -BOOLEAN"
        },
        {
            "true + false;",
            "unknown operator: BOOLEAN + BOOLEAN"
        },
        {
            "5; true + false; 5",
            "unknown operator: BOOLEAN + BOOLEAN"
        },
        {
            "if (10 > 1) { true + false; }",
            "unknown operator: BOOLEAN + BOOLEAN"
        },
        {
            R"(
            if (10 > 1) {
                if (10 > 1) {
                    return true + false;
                }
                return 1;
            }
        )",
            "unknown operator: BOOLEAN + BOOLEAN"
        }
    };

    for (const auto& tt : tests) {
        auto evaluated = testEval(tt.input);

        auto* errObj = dynamic_cast<Error*>(evaluated.get());
        if (!errObj) {
            FAIL() << "no error object returned. got="
                   << evaluated->Type() << "(" << evaluated->Inspect() << ")";
            continue;
        }

        EXPECT_EQ(errObj->Message, tt.expectedMessage)
            << "wrong error message for input: " << tt.input;
    }
}

