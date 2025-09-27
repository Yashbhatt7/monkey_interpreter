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

TEST(EvaluatorTest, TestEvalIntegerExpression) {
    struct TestCase {
        std::string input;
        int64_t expected;
    };

    std::vector<TestCase> tests = {
        {"5", 5},
        {"10", 10}
    };

    for (const auto& tt : tests) {
        auto evaluated = testEval(tt.input);
        ASSERT_TRUE(testIntegerObject(evaluated.get(), tt.expected));
    }
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

TEST(EvaluatorTest, TestEvalBooleanExpression) {
    struct TestCase {
        std::string input;
        bool expected;
    };

    std::vector<TestCase> tests = {
        {"true", true},
        {"false", false},
    };

    for (const auto& tt : tests) {
        auto evaluated = testEval(tt.input);
        ASSERT_TRUE(testBooleanObject(evaluated.get(), tt.expected));
    }
}

