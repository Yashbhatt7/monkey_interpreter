#include<iostream>

class Lexer {
public:
    char ch;
public:
    void call(Lexer* obj) {
        obj->ch = 'a';
    }
};

int main() {
    Lexer lexer;
    lexer.call(&lexer);

    std::cout << "ch: " << lexer.ch << std::endl;

    std::cin.get();
}
