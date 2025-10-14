#ifndef REPL_HPP
#define REPL_HPP

#include<iostream>
#include<vector>
#include<string>

void Start(std::istream& in, std::ostream& out);
void printParserErrors(std::ostream& out, const std::vector<std::string>& errors);

#endif // REPL_HPP

