#include<gtest/gtest.h>
#include<vector>
#include<string>
#include<memory>
#include "../src/lexer.hpp"
#include "../src/parser.hpp"
#include "../src/object.hpp"
#include "../src/evaluator.hpp"

struct TestContext {
    std::unique_ptr<Program> program;
    std::shared_ptr<Environment> env;
    std::unique_ptr<Object> result;
};

TestContext testEvalWithContext(const std::string& input) {
    TestContext ctx;
    auto lexer = std::make_unique<Lexer>(input);
    Parser parser(std::move(lexer));
    ctx.program = parser.ParseProgram();
    ctx.env = NewEnvironment();
    ctx.result = Eval(ctx.program.get(), ctx.env);
    return ctx;
}

std::unique_ptr<Object> testEval(const std::string& input) {
    auto ctx = testEvalWithContext(input);
    return std::move(ctx.result);
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

TEST(EvaluatorTest, TestErrorHandling) {
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
        },
        {
            "foobar",
            "identifier not found: foobar",
        },
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

TEST(EvaluatorTest, TestLetStatement) {
    struct TestCase {
        std::string input;
        int64_t expected;
    };

    std::vector<TestCase> tests {
        {"let a = 5; a;", 5},
        {"let a = 5 * 5; a;", 25},
        {"let a = 5; let b = a; b;", 5},
        {"let a = 5; let b = a; let c = a + b + 5; c;", 15},
    };

    for (const auto& tt : tests) {
        auto evaluated = testEval(tt.input);
        ASSERT_TRUE(testIntegerObject(evaluated.get(), tt.expected));
    }
}

TEST(EvaluatorTest, TestFunctionObject) {
    std::string input = "fn(x) { x + 2; };";
    auto ctx = testEvalWithContext(input);

    auto fn = dynamic_cast<Function*>(ctx.result.get());
    ASSERT_TRUE(fn != nullptr) << "object is not Function.";

    ASSERT_EQ(fn->Parameters.size(), 1);
    ASSERT_EQ(fn->Parameters[0], "x");

    std::string expectedBody = "(x + 2)";
    ASSERT_EQ(fn->Body->String(), expectedBody);
}

TEST(EvaluatorTest, TestFunctionApplication) {
    struct TestCase {
        std::string input;
        int64_t expected;
    };

    std::vector<TestCase> tests = {
        {"let identity = fn(x) { x; }; identity(5);", 5},
        {"let identity = fn(x) { return x; }; identity(5);", 5},
        {"let double = fn(x) { x * 2; }; double(5);", 10},
        {"let add = fn(x, y) { x + y; }; add(5, 5);", 10},
        {"let add = fn(x, y) { x + y; }; add(5 + 5, add(5, 5));", 20},
        {"fn(x) { x; }(5)", 5},
    };

    for (const auto& tt : tests) {
        auto ctx = testEvalWithContext(tt.input);
        ASSERT_TRUE(testIntegerObject(ctx.result.get(), tt.expected))
            << "Failed testing input: " << tt.input;
    }
}

TEST(EvaluatorTest, TestClosures) {
    std::string input = R"(
        let newAdder = fn(x) {
        fn(y) { x + y };
        };
        let addTwo = newAdder(2);
        addTwo(2);
    )";

    auto ctx = testEvalWithContext(input);
    ASSERT_TRUE(testIntegerObject(ctx.result.get(), 4))
        << "Failed testing closures";
}
