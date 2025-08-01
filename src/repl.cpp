#include<iostream>
#include "repl.hpp"
#include "token.hpp"
#include "lexer.hpp"

const std::string Prompt = ">> ";

void Start(std::istream& in, std::ostream& out) {
    std::string line;

    while (true) {
        out << Prompt;
        out.flush();

        if(!std::getline(in, line)) {
            return;
        }

        Lexer lex(line);
        Token tok = lex.NextToken();
        while (tok.type != TokenType::Eof) {
            // std::string s = TokenTypeMap::tokenTypeToString(tok.type);
            out << "Type" << static_cast<int>(tok.type) << ", Literal: " << tok.literal << std::endl;
            tok = lex.NextToken();
        }
    }
}

