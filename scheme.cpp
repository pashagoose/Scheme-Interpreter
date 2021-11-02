#include "scheme.h"

#include <sstream>
#include "garbage_collector.h"
#include "object.h"
#include "standart_functions.h"

Interpreter::Interpreter() : global_scope_(std::make_shared<Scope>()) {
    // quote
    global_scope_->CreateObject<Quote>("quote", 1);

    // booleans
    global_scope_->CreateObject<IsBoolean>("boolean?", 1);
    global_scope_->CreateObject<NotFunction>("not", 1);
    global_scope_->CreateObject<AndFunction>("and", std::nullopt);
    global_scope_->CreateObject<OrFunction>("or", std::nullopt);

    // integers
    global_scope_->CreateObject<IsNumber>("number?", 1);
    global_scope_->CreateObject<Equal>("=", std::nullopt);
    global_scope_->CreateObject<Less>("<", std::nullopt);
    global_scope_->CreateObject<Greater>(">", std::nullopt);
    global_scope_->CreateObject<LessEqual>("<=", std::nullopt);
    global_scope_->CreateObject<GreaterEqual>(">=", std::nullopt);
    global_scope_->CreateObject<Addition>("+", std::nullopt);
    global_scope_->CreateObject<Subtraction>("-", std::nullopt);
    global_scope_->CreateObject<Multiplication>("*", std::nullopt);
    global_scope_->CreateObject<Division>("/", std::nullopt);
    global_scope_->CreateObject<Minimum>("min", std::nullopt);
    global_scope_->CreateObject<Maximum>("max", std::nullopt);
    global_scope_->CreateObject<AbsoluteValue>("abs", 1);

    // lists and pairs
    global_scope_->CreateObject<IsPair>("pair?", 1);
    global_scope_->CreateObject<IsNull>("null?", 1);
    global_scope_->CreateObject<IsList>("list?", 1);
    global_scope_->CreateObject<ConsOperation>("cons", 2);
    global_scope_->CreateObject<CarOperation>("car", 1);
    global_scope_->CreateObject<CdrOperation>("cdr", 1);
    global_scope_->CreateObject<ListMaker>("list", std::nullopt);
    global_scope_->CreateObject<ListRef>("list-ref", 2);
    global_scope_->CreateObject<ListTail>("list-tail", 2);

    // if
    global_scope_->CreateObject<IfStatement>("if", std::nullopt);

    // define
    global_scope_->CreateObject<Definition>("define", std::nullopt);

    // setters
    global_scope_->CreateObject<SetVariable>("set!", std::nullopt);
    global_scope_->CreateObject<SetCar>("set-car!", std::nullopt);
    global_scope_->CreateObject<SetCdr>("set-cdr!", std::nullopt);

    // symbols
    global_scope_->CreateObject<IsSymbol>("symbol?", 1);

    // lambda
    global_scope_->CreateObject<MakeLambda>("lambda", std::nullopt);
}

std::string Interpreter::Run(const std::string& command) {
	garbage_collector::Instance().CollectExcept(global_scope_->GatherReferredObjects());

    std::stringstream s(command);
    Tokenizer tokenizer(&s);
    std::vector<Object*> objects;
    while (!tokenizer.IsEnd()) {
        objects.push_back(Read(&tokenizer));
    }

    std::string result;
    for (Object* obj_ptr : objects) {
    	std::set<Object*> important = global_scope_->GatherReferredObjects();
    	important.insert(obj_ptr);
    	garbage_collector::Instance().CollectExcept(important);

        result = GetRepr(EvaluateObject(obj_ptr, global_scope_));
    }

    return result;
}