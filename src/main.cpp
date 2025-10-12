#include<iostream>
#include<string>
#include<unistd.h>
#include<pwd.h>
#include "repl.hpp"

std::string getCurrentUsername() {
    struct passwd *pw = getpwuid(getuid());
    if (pw) {
        return std::string(pw->pw_name);
    }

    return "User";
}

int main() {
    try {
        std::string username = getCurrentUsername();

        std::cout << "Hello " << username << "! This is the Monkey programming language!" << "\n";
        std::cout << "Feel free to type in commands" << "\n";

        Start(std::cin, std::cout);
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return -1;
    }

    std::cin.get();
}

// For File Based Execution
// #include<iostream>
// #include<fstream>
// #include<sstream>
// #include<string>
// #include<unistd.h>
// #include<pwd.h>
// #include "repl.hpp"
// #include "lexer.hpp"
// #include "parser.hpp"
// #include "evaluator.hpp"
// #include "environment.hpp"
//
// std::string getCurrentUsername() {
//     struct passwd *pw = getpwuid(getuid());
//     if (pw) {
//         return std::string(pw->pw_name);
//     }
//     return "User";
// }
//
// void runFile(const std::string& filename) {
//     std::ifstream file(filename);
//     if (!file.is_open()) {
//         std::cerr << "Error: Could not open file '" << filename << "'\n";
//         return;
//     }
//
//     std::stringstream buffer;
//     buffer << file.rdbuf();
//     std::string input = buffer.str();
//     file.close();
//
//     auto lexer = std::make_unique<Lexer>(input);
//     Parser parser(std::move(lexer));
//     auto program = parser.ParseProgram();
//
//     if (!parser.Errors().empty()) {
//         std::cerr << "Parser errors:\n";
//         for (const auto& error : parser.Errors()) {
//             std::cerr << "\t" << error << "\n";
//         }
//         return;
//     }
//
//     auto env = NewEnvironment();
//     auto result = Eval(program.get(), env);
//
//     if (result) {
//         if (auto errorObj = dynamic_cast<Error*>(result.get())) {
//             std::cerr << errorObj->Inspect() << "\n";
//         } else if (!dynamic_cast<Null*>(result.get())) {
//             std::cout << result->Inspect() << "\n";
//         }
//     }
// }
//
// int main(int argc, char* argv[]) {
//     try {
//         if (argc == 2) {
//             runFile(argv[1]);
//             return 0;
//         }
//
//         if (argc > 2) {
//             std::cerr << "Usage: " << argv[0] << " [script.monkey]\n";
//             std::cerr << "  With no arguments: Start REPL\n";
//             std::cerr << "  With filename: Execute the script\n";
//             return 1;
//         }
//
//         std::string username = getCurrentUsername();
//         std::cout << "Hello " << username << "! This is the Monkey programming language!\n";
//         std::cout << "Feel free to type in commands\n";
//         Start(std::cin, std::cout);
//     }
//     catch (const std::exception& e) {
//         std::cerr << "Error: " << e.what() << "\n";
//         return 1;
//     }
//
//     return 0;
// }

