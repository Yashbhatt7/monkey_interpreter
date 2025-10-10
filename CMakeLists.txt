cmake_minimum_required(VERSION 3.16)
project(MonkeyInterpreter)
set(CMAKE_CXX_STANDARD 17)

include_directories(src)

find_package(fmt REQUIRED)

add_library(monkey_interpreter
    src/lexer.cpp
    src/parser.cpp
    src/evaluator.cpp
    src/object.cpp
    src/ast.cpp
    src/repl.cpp
    src/environment.cpp
)

target_link_libraries(monkey_interpreter fmt::fmt)

add_executable(monkey src/main.cpp)
target_link_libraries(monkey monkey_interpreter)

find_package(GTest REQUIRED)

add_executable(evaluator_test tests/evaluator_test.cpp)
target_link_libraries(evaluator_test
    monkey_interpreter
    GTest::gtest_main
    GTest::gtest
)

