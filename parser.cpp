#include "parser.h"
#include "error.h"
#include "object.h"
#include "tokenizer.h"
#include "garbage_collector.h"
#include "tokens.h"

#include <iostream>

Object* Read(Tokenizer* tokenizer) {
    if (tokenizer->IsEnd()) {
        return nullptr;
    }

    auto token = tokenizer->GetToken();

    // PrintToken(tokenizer->GetToken());

    if (ConstantToken* constant = std::get_if<ConstantToken>(&token)) {
        tokenizer->Next();
        return garbage_collector::Instance().RegisterObject<Number>(constant->value);
    } else if (SymbolToken* symbol = std::get_if<SymbolToken>(&token)) {
        tokenizer->Next();
        return garbage_collector::Instance().RegisterObject<Symbol>(symbol->name);
    } else if (std::get_if<QuoteToken>(&token)) {
        return ReadQuoted(tokenizer);
    } else {
        return ReadList(tokenizer);
    }
}

bool IsOpeningParToken(const Token& token) {
    const BracketToken* bracket = std::get_if<BracketToken>(&token);
    return (bracket && *bracket == BracketToken::OPEN);
}

bool IsClosingParToken(const Token& token) {
    const BracketToken* bracket = std::get_if<BracketToken>(&token);
    return (bracket && *bracket == BracketToken::CLOSE);
}

bool IsDotToken(const Token& token) {
    return std::get_if<DotToken>(&token);
}

bool IsQuoteToken(const Token& token) {
    return std::get_if<QuoteToken>(&token);
}

void PrintToken(Token token) {
    std::cerr << "Token: ";
    if (IsDotToken(token)) {
        std::cerr << "`.`";
    } else if (IsClosingParToken(token)) {
        std::cerr << "T`)`";
    } else if (IsOpeningParToken(token)) {
        std::cerr << "`(`";
    } else if (IsQuoteToken(token)) {
        std::cerr << "`\'`";
    } else if (SymbolToken* symbol = std::get_if<SymbolToken>(&token)) {
        std::cerr << '`' << symbol->name << '`';
    } else {
        ConstantToken* constant = std::get_if<ConstantToken>(&token);
        std::cerr << "`" << constant->value << "`";
    }
    std::cerr << "\n";
}

Object* ReadQuoted(Tokenizer* tokenizer) {
    Cell* first_cell_ptr = garbage_collector::Instance().RegisterObject<Cell>();
    first_cell_ptr->GetFirst() = garbage_collector::Instance().RegisterObject<Symbol>("quote");
    tokenizer->Next();
    auto obj_ptr = Read(tokenizer);
    auto new_cell_ptr = garbage_collector::Instance().RegisterObject<Cell>();
    first_cell_ptr->GetSecond() = new_cell_ptr;
    new_cell_ptr->GetFirst() = obj_ptr;
    return first_cell_ptr;
}

Object* ReadList(Tokenizer* tokenizer) {
    if (!IsOpeningParToken(tokenizer->GetToken())) {
        throw SyntaxError("Expected opening bracket");
    }

    bool found_dot = false;
    bool set_after_dot = false;
    Cell* current_cell_ptr = nullptr;
    Cell* first_cell_ptr = nullptr;

    auto current_cell = [&]() -> Cell*& {
        if (current_cell_ptr) {
            return current_cell_ptr;
        } else {
            return current_cell_ptr = first_cell_ptr =
                       garbage_collector::Instance().RegisterObject<Cell>();
        }
    };

    auto add_element_in_list = [&](Object* obj_ptr) {
        if (first_cell_ptr) {
            if (found_dot) {
                current_cell()->GetSecond() = obj_ptr;
                set_after_dot = true;
            } else {
                Cell* new_cell_ptr = garbage_collector::Instance().RegisterObject<Cell>();
                current_cell()->GetSecond() = new_cell_ptr;
                current_cell() = new_cell_ptr;
                current_cell()->GetFirst() = obj_ptr;
            }
        } else {
            if (found_dot) {
                throw SyntaxError("Dot cannot be first in list");
            }
            current_cell()->GetFirst() = obj_ptr;
        }
    };

    // PrintToken(tokenizer->GetToken());

    tokenizer->Next();
    while (true) {
        if (tokenizer->IsEnd()) {
            throw SyntaxError("Expected ending of the list, no more tokens");
        }

        auto token = tokenizer->GetToken();

        // PrintToken(token);

        if (set_after_dot && !IsClosingParToken(token)) {
            throw SyntaxError("Expected closing bracket");
        }

        if (IsDotToken(token)) {
            if (found_dot) {
                throw SyntaxError("Too much dots in list");
            }
            found_dot = true;
            tokenizer->Next();
        } else if (IsClosingParToken(token)) {
            if (found_dot && !set_after_dot) {
                throw SyntaxError("No arguments after `.` in list");
            }
            tokenizer->Next();
            return first_cell_ptr;
        } else {
            auto obj_ptr = Read(tokenizer);
            add_element_in_list(obj_ptr);
        }
    }
}