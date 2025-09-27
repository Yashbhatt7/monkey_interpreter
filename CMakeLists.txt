cmake_minimum_required(VERSION 3.16)
project(MonkeyInterpreterTests)
set(CMAKE_CXX_STANDARD 17)

# Add source directory
include_directories(src)

# Create library with your interpreter code
add_library(monkey_interpreter
    src/lexer.cpp
    src/parser.cpp
    src/evaluator.cpp
    src/object.cpp
    src/ast.cpp
    # Add other source files
)

# Add GoogleTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Test executable
add_executable(evaluator_test tests/evaluator_test.cpp)

# Link with your library and GoogleTest (includes main)
target_link_libraries(evaluator_test
    monkey_interpreter
    GTest::gtest_main  # This provides main()
    GTest::gtest
)
