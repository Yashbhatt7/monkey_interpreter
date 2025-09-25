#include<iostream>
// #include<memory>


// class statement { };
//
// class exprStmt : public statement { };
//
// class Parser {
// public:
//     // The return type from func parseExprStmt will get converted to statement which is like child
//     // type to base class and here smartpointers up casts to statement type which is implicit
//     // so basically implicit up cast happens exprStmt -> statement
//     std::unique_ptr<statement> parseExpr() {
//         return parseExprStmt();
//     }
//
//     // This will return the type exprStmt
//     std::unique_ptr<exprStmt> parseExprStmt() {
//         auto stmt = std::make_unique<exprStmt>();
//         return stmt;
//     }
//
// };
//
//
//
// class Lexer {
// public:
//     std::unique_ptr<Lexer> a;
//     char ch;
// public:
//     // void call(Lexer* obj) {
//     //     obj->ch = 'a';
//     // }
// };

// checking how enum will work with abstract class
enum class NodeType {
    DERIVED,
    DERIVED1,
    DERIVED2,
};

class abstract {
public:
    virtual NodeType mustImplement() const = 0;
};

class derived : public abstract {
public:
    NodeType mustImplement() const override {
        std::cout << "only derived\n";
        return NodeType::DERIVED;
    }
};

class derived1 : public abstract {
public:
    NodeType mustImplement() const override {
        std::cout << "derived1\n";
        return NodeType::DERIVED1;
    }
};

class derived2 : public abstract {
public:
    NodeType mustImplement() const override {
        std::cout << "derived2\n";
        return NodeType::DERIVED2;
    }
};

int main() {
    // Lexer lexer;
    // lexer.call(&lexer);
    int chch = 13;

    derived1 p;
    p.mustImplement();

    abstract* a = &p;
    a->mustImplement();

    NodeType letSee = a->mustImplement();

    switch (letSee) {
        case NodeType::DERIVED: std::cout << "okay its only DERIVED\n"; break;
        case NodeType::DERIVED1: std::cout << "okay its DERIVED1\n"; break;
        case NodeType::DERIVED2: std::cout << "okay its DERIVED2\n"; break;
        default: std::cout << "no way we got some issues\n";
    };

    // std::unique_ptr<Lexer> b;
    // // std::unique_ptr<Lexer> a = std::make_unique<Lexer>(ch, std::move(b));
    // std::unique_ptr<Lexer> d = std::make_unique<Lexer>(std::move(b), chch);

    // std::cout << "ch: " << d->ch << std::endl;

    std::cin.get();
}
