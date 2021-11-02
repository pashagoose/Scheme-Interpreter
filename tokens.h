#pragma once

#include <cstdint>
#include <iostream>
#include <string>
#include <string_view>
#include <variant>

struct SymbolToken {
    std::string name;

    bool operator==(const SymbolToken& other) const {
        return name == other.name;
    }
};

struct QuoteToken {
    bool operator==(const QuoteToken&) const {
        return true;
    }
};

struct DotToken {
    bool operator==(const DotToken&) const {
        return true;
    }
};

enum class BracketToken { OPEN, CLOSE };

struct ConstantToken {
    int64_t value;

    bool operator==(const ConstantToken& other) const {
        return value == other.value;
    }
};

using Token = std::variant<ConstantToken, BracketToken, SymbolToken, QuoteToken, DotToken>;

class ITokenProducer {
public:
    ITokenProducer() = default;
    virtual ~ITokenProducer() = default;
    virtual Token operator()(const std::string&) const = 0;
};

class OpenParenthesisProducer final : public ITokenProducer {
public:
    OpenParenthesisProducer() = default;

    Token operator()(const std::string&) const override {
        return BracketToken::OPEN;
    }
};

class CloseParenthesisProducer final : public ITokenProducer {
public:
    CloseParenthesisProducer() = default;

    Token operator()(const std::string&) const override {
        return BracketToken::CLOSE;
    }
};

class DotProducer final : public ITokenProducer {
public:
    DotProducer() = default;

    Token operator()(const std::string&) const override {
        return DotToken{};
    }
};

class QuoteProducer final : public ITokenProducer {
public:
    QuoteProducer() = default;

    Token operator()(const std::string&) const override {
        return QuoteToken{};
    }
};

class ConstantProducer final : public ITokenProducer {
public:
    ConstantProducer() = default;

    Token operator()(const std::string& token) const override {
        return ConstantToken{std::atoll(token.data())};
    }
};

class SymbolProducer final : public ITokenProducer {
public:
    SymbolProducer() = default;

    Token operator()(const std::string& token) const override {
        return SymbolToken{token};
    }
};