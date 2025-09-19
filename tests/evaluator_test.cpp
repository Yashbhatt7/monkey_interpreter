#include<gtest/gtest.h>
#include<vector>
#include<string>
#include<memory>
#include "../src/lexer.hpp"
#include "../src/parser.hpp"
#include "../src/object.hpp"
#include "../src/evaluator.hpp"

std::unique_ptr<Object> testEval(const std::string& input) {
    auto l = std::make_unique<Lexer>(input);
    Parser p(std::move(l));
    auto program = p.ParseProgram();

    return Eval(std::move(program));
}

bool testIntegerObject(Object* obj, int64_t expected) {
    auto result = dynamic_cast<Integer*>(obj);
    if (!result) {
        std::cerr << "object is not Integer.\n";
        return false;
    }

    if (result->Value != expected) {
        std::cerr << "object has wrong value. got" << result->Value << " want " << expected;
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
    };

    for (const auto& tt : tests) {
        auto evaluator = testEval(tt.input);
        EXPECT_TRUE(testIntegerObject(evaluator.get(), tt.expected))
            << "Failed testing input: " << tt.input;
    }
}

