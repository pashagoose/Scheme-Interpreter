#pragma once

#include "tokens.h"

#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <vector>

class StateMachine {  // DFA for extracting tokens
public:
    enum class Terminals {
        Root = 0,
        OpenParTerminal = 1,
        CloseParTerminal = 2,
        DotTerminal = 3,
        QuoteTerminal = 4,
        PlusTerminal = 5,
        MinusTerminal = 6,
        SymbolTerminal = 7,
        ConstantTerminal = 8
    };

    constexpr static inline size_t kStatesCount = 9;

public:
    StateMachine();

    bool Move(char);
    /*
        Returns true if need to flush,
    */

    bool IsEmptyToken() const;

    Token Flush();

private:
    struct Node {
        std::map<char, Node*> next;
        std::unique_ptr<ITokenProducer> producer;
    };

private:
    std::string token_str_;
    Node node_arr_[kStatesCount];
    Node* current_node_ = nullptr;
};

class Tokenizer {
public:
    Tokenizer(std::istream* in);

    bool IsEnd();

    void Next();

    Token GetToken();

private:
    StateMachine token_dfa_;
    std::istream* input_;
    std::optional<Token> last_token_;
    bool reached_end_ = false;
};