#include<iostream>
#include<memory>


class statement { };

class exprStmt : public statement { };

class Parser {
public:
    std::unique_ptr<statement> parseExpr() {
        return parseExprStmt();
    }

    std::unique_ptr<exprStmt> parseExprStmt() {
        auto stmt = std::make_unique<exprStmt>();
        return stmt;
    }

};



class Lexer {
public:
    std::unique_ptr<Lexer> a;
    char ch;
public:
    // void call(Lexer* obj) {
    //     obj->ch = 'a';
    // }
};

int main() {
    // Lexer lexer;
    // lexer.call(&lexer);
    int chch = 13;

    std::unique_ptr<Lexer> b;
    // std::unique_ptr<Lexer> a = std::make_unique<Lexer>(ch, std::move(b));
    std::unique_ptr<Lexer> d = std::make_unique<Lexer>(std::move(b), chch);

    std::cout << "ch: " << d->ch << std::endl;

    std::cin.get();
}
