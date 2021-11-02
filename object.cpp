#include "object.h"
#include "error.h"

Cell* FormList(Object* obj_ptr, std::shared_ptr<Scope> scope) {
    if (!obj_ptr) {
        return nullptr;
    }
    if (Is<Cell>(obj_ptr)) {
        return As<Cell>(obj_ptr);
    } else {
        Cell* cell_ptr = scope->CreateServiceObject<Cell>();
        cell_ptr->GetFirst() = obj_ptr;
        return cell_ptr;
    }
}

std::shared_ptr<Scope> ScopedFunction::Setup(Cell* cell_ptr, std::shared_ptr<Scope> scope) {
    // copy variables to local scope
    size_t index = 0;
    std::shared_ptr<Scope> new_scope = std::make_shared<Scope>(scope.get());

    ApplyToList(cell_ptr, [&](Object* obj_ptr) {
        if (index >= argnames_.size()) {
            throw RuntimeError("Too many arguments for lambda");
        }
        Object* new_obj = CopyObject(EvaluateObject(obj_ptr, scope), new_scope);
        new_scope->NameObject(new_obj, argnames_[index]);
        ++index;
    });

    if (index != argnames_.size()) {
        throw RuntimeError("Not enough arguments for lambda");
    }

    // registered capture clause
    for (auto& [name, obj_ptr] : captured_variables_) {
        new_scope->NameObject(obj_ptr, name);
    }

    return new_scope;
}

Object* ScopedFunction::InvokeImpl(Cell*, std::shared_ptr<Scope> scope) {

    Object* result = nullptr;
    for (Object* expression : commands_) {
        result = EvaluateObject(expression, scope);
    }
    Teardown(scope);
    return result;
}

void ScopedFunction::Teardown(std::shared_ptr<Scope> scope) {
    // grab capture clause back
    for (auto& [name, obj_ptr] : captured_variables_) {
        obj_ptr = *scope->GetObjectInThisScope(name);
    }
}

Object* Function::Call(Cell* cell_ptr, std::shared_ptr<Scope> scope) {
    if (!CorrectArgumentsQuantity(cell_ptr)) {
        throw RuntimeError("Wrong number of arguments");
    }
    std::shared_ptr<Scope> new_scope = Setup(cell_ptr, scope);
    Object* result = InvokeImpl(cell_ptr, new_scope);
    return result;
}

Object* Symbol::Evaluate(std::shared_ptr<Scope> scope) {
    if (name_ == "#t" || name_ == "#f") {
        return scope->CreateServiceObject<Boolean>(name_);
    } else {
        // TODO: here may be variable
        auto obj = scope->GetObjectInAncestorScope(name_);
        if (!obj) {
            throw NameError("Cannot evaluate this symbol: `" + name_ + "`");
        } else {
            return *obj;
        }
    }
}

std::string Cell::Repr() const {
    std::string result = "(";
    const Cell* cell_ptr = this;

    while (true) {
        result += GetRepr(cell_ptr->GetFirst());
        if (!cell_ptr->GetSecond()) {
            break;
        }

        result += ' ';

        if (Is<Cell>(cell_ptr->GetSecond())) {
            cell_ptr = As<Cell>(cell_ptr->GetSecond());
        } else {
            result += ". ";
            result += GetRepr(cell_ptr->GetSecond());
            break;
        }
    }

    result += ")";
    return result;
}

Object* Cell::Evaluate(std::shared_ptr<Scope> scope) {
    Function* func = nullptr;

    if (!Is<Symbol>(GetFirst())) {
        Object* obj_ptr = EvaluateObject(GetFirst(), scope);
        if (!Is<Function>(obj_ptr)) {
            throw RuntimeError("Cannot call `" + GetRepr(obj_ptr) + "`");
        }
        func = As<Function>(obj_ptr);
    } else {
        Symbol* symbol_ptr = As<Symbol>(GetFirst());

        auto func_opt = scope->GetObjectInAncestorScope(symbol_ptr->GetName());
        if (!func_opt || !Is<Function>(*func_opt)) {
            throw RuntimeError("No such function `" + symbol_ptr->GetName() + "`");
        }

        func = As<Function>(*func_opt);
    }

    Cell* next_cell = As<Cell>(GetSecond());
    if (!next_cell) {
        next_cell = FormList(GetSecond(), scope);
    }

    return func->Call(next_cell, scope);
}

Object* Cell::Copy(std::shared_ptr<Scope> scope) const {
    Cell* cell_ptr = scope->CreateServiceObject<Cell>();
    cell_ptr->GetFirst() = CopyObject(GetFirst(), scope);
    cell_ptr->GetSecond() = CopyObject(GetSecond(), scope);
    return cell_ptr;
}

Object* ScopedFunction::Copy(std::shared_ptr<Scope> scope) const {
    return scope->CreateServiceObject<ScopedFunction>(argnames_, commands_, captured_variables_,
                                                      ExpectedArgumentsCounter());
}

std::vector<Object*> ListToVector(Cell* cell_ptr) {
    std::vector<Object*> res;
    ApplyToList(cell_ptr, [&](Object* obj_ptr) { res.push_back(obj_ptr); });
    return res;
}

Cell* VectorToProperList(const std::vector<Object*>& objects, std::shared_ptr<Scope> scope) {
    Cell* start = nullptr;
    Cell* ending = nullptr;
    for (Object* obj_ptr : objects) {
        if (!start) {
            start = ending = scope->CreateServiceObject<Cell>();
        } else {
            ending->GetSecond() = scope->CreateServiceObject<Cell>();
            ending = As<Cell>(ending->GetSecond());
        }
        ending->GetFirst() = obj_ptr;
    }
    return start;
}

Cell* VectorToImproperList(const std::vector<Object*>& objects, std::shared_ptr<Scope> scope) {
    Cell* start = nullptr;
    Cell* ending = nullptr;
    for (size_t i = 0; i < objects.size(); ++i) {
        if (!start) {
            start = ending = scope->CreateServiceObject<Cell>();
            ending->GetFirst() = objects[i];
            continue;
        } else if (i + 1 != objects.size()) {
            ending->GetSecond() = scope->CreateServiceObject<Cell>();
            ending = As<Cell>(ending->GetSecond());
        }
        if (i + 1 == objects.size()) {
            ending->GetSecond() = objects[i];
            break;
        } else {
            ending->GetFirst() = objects[i];
        }
    }
    return start;
}

bool Function::CorrectArgumentsQuantity(Cell* cell_ptr) const {
    if (args_count_) {
        auto vec = ListToVector(cell_ptr);
        return (vec.size() == *args_count_);
    } else {
        return true;
    }
}

std::string GetRepr(Object* obj) {
    return obj ? obj->Repr() : "()";
}

Object* EvaluateObject(Object* obj, std::shared_ptr<Scope> scope) {
    if (obj) {
        return obj->Evaluate(scope);
    } else {
        throw RuntimeError("Cannot evaluate this object: `" + GetRepr(obj) + "`");
    }
}

bool IsBooleanConstant(Object* obj_ptr) {
    auto repr = GetRepr(obj_ptr);
    return (repr == "#t" || repr == "#f");
}

Object* CopyObject(Object* obj_ptr, std::shared_ptr<Scope> scope) {
    return obj_ptr ? obj_ptr->Copy(scope) : nullptr;
}

void Cell::GatherSubobjects(std::set<Object*>& save) {
	save.insert(this);
	if (GetFirst()) {
		GetFirst()->GatherSubobjects(save);
	}
	if (GetSecond()) {
		GetSecond()->GatherSubobjects(save);
	}
}

void ScopedFunction::GatherSubobjects(std::set<Object*>& save) {
	save.insert(this);
	for (auto [name, obj] : captured_variables_) {
		save.insert(obj);
	}
	for (auto obj : commands_) {
		save.insert(obj);
	}
}