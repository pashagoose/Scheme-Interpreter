#pragma once

#include "abstract_object.h"
#include "scope.h"

#include <string>
#include <memory>

class Number final : public Object {
public:
    Number() = default;
    Number(int64_t value) : number_(value) {
    }

    int64_t GetValue() const {
        return number_;
    }

    Object* Evaluate(std::shared_ptr<Scope>) override {
        return this;
    }

    std::string Repr() const override {
        return std::to_string(number_);
    }

    void Set(int64_t value) {
        number_ = value;
    }

    Object* Copy(std::shared_ptr<Scope> scope) const override {
        return scope->CreateServiceObject<Number>(number_);
    }

private:
    int64_t number_;
};

class Symbol final : public Object {
public:
    Symbol() = default;
    Symbol(const std::string& name) : name_(name) {
    }

    const std::string& GetName() const {
        return name_;
    }

    Object* Evaluate(std::shared_ptr<Scope> scope) override;

    std::string Repr() const override {
        return name_;
    }

    Object* Copy(std::shared_ptr<Scope> scope) const override {
        return scope->CreateServiceObject<Symbol>(name_);
    }

private:
    std::string name_;
};

class Cell : public Object {
public:
    Cell() = default;

    Object* GetFirst() const {
        return first_obj_;
    }

    Object* GetSecond() const {
        return second_obj_;
    }

    Object*& GetFirst() {
        return first_obj_;
    }

    Object*& GetSecond() {
        return second_obj_;
    }

    Object* Evaluate(std::shared_ptr<Scope> scope) override;

    std::string Repr() const override;

    Object* Copy(std::shared_ptr<Scope> scope) const override;

    void GatherSubobjects(std::set<Object*>& save) override;

private:
    Object* first_obj_ = nullptr;
    Object* second_obj_ = nullptr;
};

class Function : public Object {
public:
    Function(std::optional<size_t> args_count = std::nullopt) : args_count_(args_count) {
    }

    virtual std::shared_ptr<Scope> Setup(Cell* cell_ptr, std::shared_ptr<Scope> scope) = 0;

    virtual void Teardown(std::shared_ptr<Scope> scope) = 0;

    virtual Object* Call(Cell* cell_ptr, std::shared_ptr<Scope> scope);
    /*
        Accepts a list of arguments which gives opportunity to do short circuit evaluation for "or",
       "and"
    */

    virtual Object* InvokeImpl(
        Cell* cell_ptr,
        std::shared_ptr<Scope> scope) = 0;  // this must be overriden by lambda or standart function

    bool CorrectArgumentsQuantity(Cell* cell_ptr) const;

    virtual std::optional<size_t> ExpectedArgumentsCounter() const {
        return args_count_;
    }

    Object* Evaluate(std::shared_ptr<Scope>) override {
        return this;
    };

    std::string Repr() const override {
        return "function";
    }

private:
    std::optional<size_t> args_count_;
};

class ScopedFunction : public Function {
public:
    ScopedFunction(const std::vector<std::string>& names, const std::vector<Object*>& commands,
                   const std::unordered_map<std::string, Object*>& capture,
                   std::optional<size_t> args_count = std::nullopt)
        : Function(args_count),
          argnames_(names),
          commands_(commands),
          captured_variables_(capture) {
    }

    std::shared_ptr<Scope> Setup(Cell* cell_ptr, std::shared_ptr<Scope> scope) override;

    void Teardown(std::shared_ptr<Scope> scope) override;

    Object* InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) override;

    Object* Copy(std::shared_ptr<Scope> scope) const override;

    void GatherSubobjects(std::set<Object*>& save) override;

private:
    std::vector<std::string> argnames_;
    std::vector<Object*>
        commands_;  // those objects are never deleted - they are part of syntax tree
    std::unordered_map<std::string, Object*> captured_variables_;
};

class StandartFunction
    : public Function {  // this function is efficiently implemented without creating scopes
public:
    StandartFunction(std::optional<size_t> args_count = std::nullopt) : Function(args_count) {
    }

    std::shared_ptr<Scope> Setup(Cell* cell_ptr, std::shared_ptr<Scope> scope) override {
        return scope;
    }

    void Teardown(std::shared_ptr<Scope> scope) override {
    }

    Object* Copy(std::shared_ptr<Scope>) const override {
        // this should never happen
        throw RuntimeError("Trying to copy standart function");
    }
};

class Boolean : public Object {
public:
    Boolean(const std::string& name) : value_(name == "#t") {
    }

    Boolean(bool val) : value_(val) {
    }

    std::string Repr() const override {
        return value_ ? "#t" : "#f";
    }

    Object* Evaluate(std::shared_ptr<Scope>) override {
        return this;
    }

    void Set(bool value) {
        value_ = value;
    }

    Object* Copy(std::shared_ptr<Scope> scope) const override {
        return scope->CreateServiceObject<Boolean>(value_);
    }

private:
    bool value_;
};

template <class T>
T* As(Object* obj_ptr) {
    return dynamic_cast<T*>(obj_ptr);
}

template <class T>
bool Is(Object* obj_ptr) {
    return dynamic_cast<T*>(obj_ptr);
}

Cell* FormList(Object* obj_ptr, std::shared_ptr<Scope> scope);

std::vector<Object*> ListToVector(Cell* cell_ptr);

Cell* VectorToProperList(const std::vector<Object*>& objects, std::shared_ptr<Scope> scope);

Cell* VectorToImproperList(const std::vector<Object*>& objects, std::shared_ptr<Scope> scope);

std::string GetRepr(Object* obj);

Object* EvaluateObject(Object* obj, std::shared_ptr<Scope> scope);

Object* CopyObject(Object* obj, std::shared_ptr<Scope> scope);

bool IsBooleanConstant(Object* obj_ptr);

template <class F>
void ApplyToAllObjects(Object* obj_ptr, F&& function) {
    if (!obj_ptr) {
        return;
    }

    if (!Is<Cell>(obj_ptr)) {
        function(obj_ptr);
        return;
    }

    ApplyToAllObjects(As<Cell>(obj_ptr)->GetFirst(), function);
    ApplyToAllObjects(As<Cell>(obj_ptr)->GetSecond(), function);
}

template <class F>
void ApplyToList(const Cell* cell_ptr, F&& function) {
    while (cell_ptr) {
        function(cell_ptr->GetFirst());
        if (Is<Cell>(cell_ptr->GetSecond())) {
            cell_ptr = As<Cell>(cell_ptr->GetSecond());
        } else {
            if (cell_ptr->GetSecond()) {
                function(cell_ptr->GetSecond());
            }
            break;
        }
    }
}