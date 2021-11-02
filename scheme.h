#pragma once

#include "scope.h"
#include "tokenizer.h"
#include "parser.h"
#include "standart_functions.h"

#include <string>

class Interpreter {
public:
    Interpreter();

    std::string Run(const std::string& command);

private:
    std::shared_ptr<Scope> global_scope_;
};