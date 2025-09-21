#ifndef EVALUATOR_HPP
#define EVALUATOR_HPP

#include<memory>
#include "ast.hpp"
#include "object.hpp"

std::unique_ptr<Object> Eval(Node* node);

#endif //EVALUATOR_HPP

