#ifndef EVALUATOR_HPP
#define EVALUATOR_HPP

#include<memory>
#include "ast.hpp"
#include "object.hpp"
#include "environment.hpp"

std::unique_ptr<Object> Eval(Node* node, Environment* env);

#endif //EVALUATOR_HPP

