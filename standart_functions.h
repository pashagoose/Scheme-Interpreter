#pragma once

#include "object.h"

#include <vector>
#include <unordered_set>

class Quote final : public StandartFunction {
public:
    Quote(std::optional<size_t> args_count = std::nullopt) : StandartFunction(args_count){};

    Object* InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope>) override;
};

class IsBoolean final : public StandartFunction {
public:
    IsBoolean(std::optional<size_t> args_count = std::nullopt) : StandartFunction(args_count){};

    Object* InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) override;
};

class NotFunction final : public StandartFunction {
public:
    NotFunction(std::optional<size_t> args_count = std::nullopt) : StandartFunction(args_count){};

    Object* InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) override;
};

class AndFunction final : public StandartFunction {
public:
    AndFunction(std::optional<size_t> args_count = std::nullopt) : StandartFunction(args_count){};

    Object* InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) override;
};

class OrFunction final : public StandartFunction {
public:
    OrFunction(std::optional<size_t> args_count = std::nullopt) : StandartFunction(args_count){};

    Object* InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) override;
};

class IsNumber final : public StandartFunction {
public:
    IsNumber(std::optional<size_t> args_count = std::nullopt) : StandartFunction(args_count){};

    Object* InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) override;
};

template <class Predicate>
Object* CheckOrderList(Cell* cell_ptr, std::shared_ptr<Scope> scope, Predicate&& pred) {
    auto objects = ListToVector(cell_ptr);
    Boolean* ans_ptr = scope->CreateServiceObject<Boolean>(true);
    Object* prev_obj = nullptr;
    for (size_t i = 1; i < objects.size(); ++i) {
        if (i == 1) {
            prev_obj = EvaluateObject(objects[i - 1], scope);
        }
        Object* current_obj = EvaluateObject(objects[i], scope);
        if (!pred(prev_obj, current_obj)) {
            ans_ptr->Set(false);
            break;
        }
        prev_obj = current_obj;
    }
    return ans_ptr;
}

class Equal final : public StandartFunction {
public:
    Equal(std::optional<size_t> args_count = std::nullopt) : StandartFunction(args_count){};

    Object* InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) override;
};

class Less final : public StandartFunction {
public:
    Less(std::optional<size_t> args_count = std::nullopt) : StandartFunction(args_count){};

    Object* InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) override;
};

class Greater final : public StandartFunction {
public:
    Greater(std::optional<size_t> args_count = std::nullopt) : StandartFunction(args_count){};

    Object* InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) override;
};

class LessEqual final : public StandartFunction {
public:
    LessEqual(std::optional<size_t> args_count = std::nullopt) : StandartFunction(args_count){};

    Object* InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) override;
};

class GreaterEqual final : public StandartFunction {
public:
    GreaterEqual(std::optional<size_t> args_count = std::nullopt) : StandartFunction(args_count){};

    Object* InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) override;
};

class Addition final : public StandartFunction {
public:
    Addition(std::optional<size_t> args_count = std::nullopt) : StandartFunction(args_count){};

    Object* InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) override;
};

class Subtraction final : public StandartFunction {
public:
    Subtraction(std::optional<size_t> args_count = std::nullopt) : StandartFunction(args_count){};

    Object* InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) override;
};

class Multiplication final : public StandartFunction {
public:
    Multiplication(std::optional<size_t> args_count = std::nullopt)
        : StandartFunction(args_count){};

    Object* InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) override;
};

class Division final : public StandartFunction {
public:
    Division(std::optional<size_t> args_count = std::nullopt) : StandartFunction(args_count){};

    Object* InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) override;
};

class Minimum final : public StandartFunction {
public:
    Minimum(std::optional<size_t> args_count = std::nullopt) : StandartFunction(args_count){};

    Object* InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) override;
};

class Maximum final : public StandartFunction {
public:
    Maximum(std::optional<size_t> args_count = std::nullopt) : StandartFunction(args_count){};

    Object* InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) override;
};

class AbsoluteValue final : public StandartFunction {
public:
    AbsoluteValue(std::optional<size_t> args_count = std::nullopt) : StandartFunction(args_count){};

    Object* InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) override;
};

class IsPair final : public StandartFunction {
public:
    IsPair(std::optional<size_t> args_count = std::nullopt) : StandartFunction(args_count){};

    Object* InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) override;
};

class IsNull final : public StandartFunction {
public:
    IsNull(std::optional<size_t> args_count = std::nullopt) : StandartFunction(args_count){};

    Object* InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) override;
};

bool CheckProperList(Cell* cell_ptr);

class IsList final : public StandartFunction {  // returns true if proper list
public:
    IsList(std::optional<size_t> args_count = std::nullopt) : StandartFunction(args_count){};

    Object* InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) override;
};

class ConsOperation final : public StandartFunction {  // returns true if proper list
public:
    ConsOperation(std::optional<size_t> args_count = std::nullopt) : StandartFunction(args_count){};

    Object* InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) override;
};

class CarOperation final : public StandartFunction {  // returns true if proper list
public:
    CarOperation(std::optional<size_t> args_count = std::nullopt) : StandartFunction(args_count){};

    Object* InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) override;
};

class CdrOperation final : public StandartFunction {  // returns true if proper list
public:
    CdrOperation(std::optional<size_t> args_count = std::nullopt) : StandartFunction(args_count){};

    Object* InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) override;
};

class ListMaker final : public StandartFunction {  // returns true if proper list
public:
    ListMaker(std::optional<size_t> args_count = std::nullopt) : StandartFunction(args_count){};

    Object* InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) override;
};

Cell* FindKthNodeInList(Cell* cell_ptr, size_t k);

class ListRef final : public StandartFunction {  // returns true if proper list
public:
    ListRef(std::optional<size_t> args_count = std::nullopt) : StandartFunction(args_count){};

    Object* InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) override;
};

class ListTail final : public StandartFunction {  // returns true if proper list
public:
    ListTail(std::optional<size_t> args_count = std::nullopt) : StandartFunction(args_count){};

    Object* InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) override;
};

class IfStatement final : public StandartFunction {  // returns true if proper list
public:
    IfStatement(std::optional<size_t> args_count = std::nullopt) : StandartFunction(args_count){};

    Object* InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) override;
};

class Definition final : public StandartFunction {  // returns true if proper list
public:
    Definition(std::optional<size_t> args_count = std::nullopt) : StandartFunction(args_count){};

    Object* InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) override;
};

Object* CreateLambda(const std::vector<Object*>& arguments, std::vector<Cell*>& commands,
                     std::shared_ptr<Scope> scope);

Object* DefineFunction(std::vector<Object*>& objects, std::shared_ptr<Scope> scope);

Object* DefineVariable(std::vector<Object*>& objects, std::shared_ptr<Scope> scope);

class SetVariable final : public StandartFunction {  // returns true if proper list
public:
    SetVariable(std::optional<size_t> args_count = std::nullopt) : StandartFunction(args_count){};

    Object* InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) override;
};

std::pair<Cell*, Object*> SetCarCdrBody(Cell* cell_ptr, std::shared_ptr<Scope> scope);

class SetCar final : public StandartFunction {  // returns true if proper list
public:
    SetCar(std::optional<size_t> args_count = std::nullopt) : StandartFunction(args_count){};

    Object* InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) override;
};

class SetCdr final : public StandartFunction {  // returns true if proper list
public:
    SetCdr(std::optional<size_t> args_count = std::nullopt) : StandartFunction(args_count){};

    Object* InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) override;
};

class IsSymbol final : public StandartFunction {  // returns true if proper list
public:
    IsSymbol(std::optional<size_t> args_count = std::nullopt) : StandartFunction(args_count){};

    Object* InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) override;
};

class MakeLambda final : public StandartFunction {  // returns true if proper list
public:
    MakeLambda(std::optional<size_t> args_count = std::nullopt) : StandartFunction(args_count){};

    Object* InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) override;
};
