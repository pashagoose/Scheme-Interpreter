#include "tokenizer.h"
#include "error.h"
#include "tokens.h"

StateMachine::StateMachine() {
    current_node_ = node_arr_;

    auto check_symbol_start = [](char ch) {
        return std::isalpha(ch) || ch == '*' || ch == '#' || ch == '<' || ch == '>' || ch == '=' ||
               ch == '/';
    };

    auto check_symbol_mid = [](char ch) {
        return std::isalnum(ch) || ch == '*' || ch == '#' || ch == '<' || ch == '>' || ch == '=' ||
               ch == '?' || ch == '!' || ch == '-' || ch == '/';
    };

    // simple token ('\'', ")", "(", ".") transitions
    current_node_->next['('] = node_arr_ + int(Terminals::OpenParTerminal);
    current_node_->next[')'] = node_arr_ + int(Terminals::CloseParTerminal);
    current_node_->next['.'] = node_arr_ + int(Terminals::DotTerminal);
    current_node_->next['\''] = node_arr_ + int(Terminals::QuoteTerminal);

    node_arr_[int(Terminals::OpenParTerminal)].producer =
        std::make_unique<OpenParenthesisProducer>();
    node_arr_[int(Terminals::CloseParTerminal)].producer =
        std::make_unique<CloseParenthesisProducer>();
    node_arr_[int(Terminals::DotTerminal)].producer = std::make_unique<DotProducer>();
    node_arr_[int(Terminals::QuoteTerminal)].producer = std::make_unique<QuoteProducer>();

    // constant and symbolic tokens transitions

    node_arr_[int(Terminals::PlusTerminal)].producer = std::make_unique<SymbolProducer>();
    node_arr_[int(Terminals::MinusTerminal)].producer = std::make_unique<SymbolProducer>();
    node_arr_[int(Terminals::SymbolTerminal)].producer = std::make_unique<SymbolProducer>();
    node_arr_[int(Terminals::ConstantTerminal)].producer = std::make_unique<ConstantProducer>();

    for (char digit = '0'; digit <= '9'; ++digit) {
        node_arr_[int(Terminals::PlusTerminal)].next[digit] =
            node_arr_ + int(Terminals::ConstantTerminal);
        node_arr_[int(Terminals::MinusTerminal)].next[digit] =
            node_arr_ + int(Terminals::ConstantTerminal);
        node_arr_[int(Terminals::Root)].next[digit] = node_arr_ + int(Terminals::ConstantTerminal);
        node_arr_[int(Terminals::ConstantTerminal)].next[digit] =
            node_arr_ + int(Terminals::ConstantTerminal);
    }

    for (char symbol = char(33); symbol <= char(126); ++symbol) {
        if (check_symbol_start(symbol)) {
            node_arr_[int(Terminals::Root)].next[symbol] =
                node_arr_ + int(Terminals::SymbolTerminal);
        }
        if (check_symbol_mid(symbol)) {
            node_arr_[int(Terminals::SymbolTerminal)].next[symbol] =
                node_arr_ + int(Terminals::SymbolTerminal);
        }
    }

    node_arr_[int(Terminals::Root)].next['+'] = node_arr_ + int(Terminals::PlusTerminal);
    node_arr_[int(Terminals::Root)].next['-'] = node_arr_ + int(Terminals::MinusTerminal);
}

bool StateMachine::Move(char ch) {
    auto it = current_node_->next.find(ch);
    if (it != current_node_->next.end()) {
        current_node_ = it->second;
        token_str_ += ch;
        return false;
    } else {
        return true;
    }
}

bool StateMachine::IsEmptyToken() const {
    return token_str_.empty();
}

Token StateMachine::Flush() {
    if (IsEmptyToken()) {
        throw SyntaxError("Cannot flush empty token");
    } else {
        auto res = (*(current_node_->producer))(token_str_);
        token_str_ = "";
        current_node_ = node_arr_;
        return res;
    }
}

Tokenizer::Tokenizer(std::istream* in) : input_(in) {
    Next();
}

bool Tokenizer::IsEnd() {
    return reached_end_;
}

void Tokenizer::Next() {
    if (reached_end_) {
        return;
    }

    while (input_->peek() != EOF) {
        char ch = input_->peek();
        if (std::isspace(ch)) {
            if (!token_dfa_.IsEmptyToken()) {
                last_token_ = token_dfa_.Flush();
                break;
            }
        } else {
            if (token_dfa_.Move(ch)) {
                last_token_ = token_dfa_.Flush();
                break;
            }
        }
        input_->ignore();
    }

    if (input_->peek() == EOF) {
        if (!token_dfa_.IsEmptyToken()) {
            last_token_ = token_dfa_.Flush();
        } else {
            reached_end_ = true;
        }
    }
}

Token Tokenizer::GetToken() {
    if (last_token_) {
        return *last_token_;
    } else {
        throw SyntaxError("No token extracted");
    }
}