cmake_minimum_required(VERSION 3.16)
project(MonkeyInterpreterTests)
set(CMAKE_CXX_STANDARD 17)

include_directories(src)

add_library(monkey_interpreter
    src/lexer.cpp
    src/parser.cpp
    src/evaluator.cpp
    src/object.cpp
    src/ast.cpp
    src/repl.cpp
)

add_executable(monkey src/main.cpp)
target_link_libraries(monkey monkey_interpreter)

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(evaluator_test tests/evaluator_test.cpp)

target_link_libraries(evaluator_test
    monkey_interpreter
    GTest::gtest_main
    GTest::gtest
)
