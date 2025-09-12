#include<iostream>
#include<string>
#include<unistd.h>
#include<pwd.h>
#include "repl.hpp"

// clang++ -std=c++17 -o monkey main.cpp repl.cpp lexer.cpp -Wall -Wextra

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
