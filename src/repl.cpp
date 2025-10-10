#include<iostream>
#include<memory>
#include "repl.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "evaluator.hpp"

const std::string Prompt = ">> ";

void Start(std::istream& in, std::ostream& out) {
    std::string line;

    while (true) {
        out << Prompt;
        out.flush();

        if(!std::getline(in, line)) {
            return;
        }

        auto lexer = std::make_unique<Lexer>(line);
        Parser p(std::move(lexer));
        auto program = p.ParseProgram();

        if (!p.Errors().empty()) {
            printParserErrors(out, p.Errors());
            continue;
        }

        auto evaluated = Eval(program.get());
        if (evaluated != nullptr) {
            out << evaluated->Inspect();
            out << "\n";
        }

        // out << program->String() << "\n";
    }
}

void printParserErrors(std::ostream& out, const std::vector<std::string>& errors) {
    out << "Woops! We ran into some monkey business here!\n";
    out << " parser errors:\n";
    for (const auto& msg : errors) {
        out << "\t" << msg << "\n";
    }
}

