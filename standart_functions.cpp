#include "standart_functions.h"
#include <algorithm>
#include <functional>
#include <iterator>
#include "error.h"
#include "object.h"

Object* Quote::InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope>) {
    // assumming we were given list of length 1 and we return first element
    return cell_ptr->GetFirst();
}

Object* IsBoolean::InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) {
    return scope->CreateServiceObject<Boolean>(
        IsBooleanConstant(EvaluateObject(cell_ptr->GetFirst(), scope)));
}

Object* NotFunction::InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) {
    Object* arg = EvaluateObject(cell_ptr->GetFirst(), scope);
    return scope->CreateServiceObject<Boolean>(GetRepr(arg) == "#f");
}

Object* AndFunction::InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) {
    auto objects = ListToVector(cell_ptr);
    Object* last_res = nullptr;
    for (Object* obj_ptr : objects) {
        last_res = EvaluateObject(obj_ptr, scope);
        if (GetRepr(last_res) == "#f") {
            break;
        }
    }
    if (!last_res) {
        last_res = scope->CreateServiceObject<Boolean>(true);
    }
    return last_res;
}

Object* OrFunction::InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) {
    auto objects = ListToVector(cell_ptr);
    Object* last_res = nullptr;
    for (Object* obj_ptr : objects) {
        last_res = EvaluateObject(obj_ptr, scope);
        if (GetRepr(last_res) != "#f") {
            break;
        }
    }
    if (!last_res) {
        last_res = scope->CreateServiceObject<Boolean>(false);
    }
    return last_res;
}

Object* IsNumber::InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) {
    return scope->CreateServiceObject<Boolean>(
        Is<Number>(EvaluateObject(cell_ptr->GetFirst(), scope)));
}

Object* Equal::InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) {
    return CheckOrderList(cell_ptr, scope, [&](Object* lhs, Object* rhs) {
        if (!Is<Number>(lhs) || !Is<Number>(rhs)) {
            throw RuntimeError("Cannot compare: `" + GetRepr(lhs) + "` and `" + GetRepr(rhs) + "`");
        }
        return As<Number>(lhs)->GetValue() == As<Number>(rhs)->GetValue();
    });
}

Object* Less::InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) {
    return CheckOrderList(cell_ptr, scope, [&](Object* lhs, Object* rhs) {
        if (!Is<Number>(lhs) || !Is<Number>(rhs)) {
            throw RuntimeError("Cannot compare: `" + GetRepr(lhs) + "` and `" + GetRepr(rhs) + "`");
        }
        return As<Number>(lhs)->GetValue() < As<Number>(rhs)->GetValue();
    });
}

Object* Greater::InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) {
    return CheckOrderList(cell_ptr, scope, [&](Object* lhs, Object* rhs) {
        if (!Is<Number>(lhs) || !Is<Number>(rhs)) {
            throw RuntimeError("Cannot compare: `" + GetRepr(lhs) + "` and `" + GetRepr(rhs) + "`");
        }
        return As<Number>(lhs)->GetValue() > As<Number>(rhs)->GetValue();
    });
}

Object* LessEqual::InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) {
    return CheckOrderList(cell_ptr, scope, [&](Object* lhs, Object* rhs) {
        if (!Is<Number>(lhs) || !Is<Number>(rhs)) {
            throw RuntimeError("Cannot compare: `" + GetRepr(lhs) + "` and `" + GetRepr(rhs) + "`");
        }
        return As<Number>(lhs)->GetValue() <= As<Number>(rhs)->GetValue();
    });
}

Object* GreaterEqual::InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) {
    return CheckOrderList(cell_ptr, scope, [&](Object* lhs, Object* rhs) {
        if (!Is<Number>(lhs) || !Is<Number>(rhs)) {
            throw RuntimeError("Cannot compare: `" + GetRepr(lhs) + "` and `" + GetRepr(rhs) + "`");
        }
        return As<Number>(lhs)->GetValue() >= As<Number>(rhs)->GetValue();
    });
}

Object* Addition::InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) {
    int64_t result = 0;
    ApplyToList(cell_ptr, [&](Object* obj_ptr) {
        obj_ptr = EvaluateObject(obj_ptr, scope);
        if (!Is<Number>(obj_ptr)) {
            throw RuntimeError("`" + GetRepr(obj_ptr) + "` is not a number");
        }
        result += As<Number>(obj_ptr)->GetValue();
    });
    return scope->CreateServiceObject<Number>(result);
}

Object* Subtraction::InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) {
    std::optional<int64_t> result;
    ApplyToList(cell_ptr, [&](Object* obj_ptr) {
        obj_ptr = EvaluateObject(obj_ptr, scope);
        if (!Is<Number>(obj_ptr)) {
            throw RuntimeError("`" + GetRepr(obj_ptr) + "` is not a number");
        }
        if (!result) {
            result = As<Number>(obj_ptr)->GetValue();
        } else {
            *result -= As<Number>(obj_ptr)->GetValue();
        }
    });
    if (!result) {
        throw RuntimeError("No arguments for `-` operation");
    }

    return scope->CreateServiceObject<Number>(*result);
}

Object* Multiplication::InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) {
    int64_t result = 1;
    ApplyToList(cell_ptr, [&](Object* obj_ptr) {
        obj_ptr = EvaluateObject(obj_ptr, scope);
        if (!Is<Number>(obj_ptr)) {
            throw RuntimeError("`" + GetRepr(obj_ptr) + "` is not a number");
        }
        result *= As<Number>(obj_ptr)->GetValue();
    });
    return scope->CreateServiceObject<Number>(result);
}

Object* Division::InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) {
    std::optional<int64_t> result;
    ApplyToList(cell_ptr, [&](Object* obj_ptr) {
        obj_ptr = EvaluateObject(obj_ptr, scope);
        if (!Is<Number>(obj_ptr)) {
            throw RuntimeError("`" + GetRepr(obj_ptr) + "` is not a number");
        }
        if (!result) {
            result = As<Number>(obj_ptr)->GetValue();
        } else {
            *result /= As<Number>(obj_ptr)->GetValue();
        }
    });
    if (!result) {
        throw RuntimeError("No arguments for `/` operation");
    }

    return scope->CreateServiceObject<Number>(*result);
}

Object* Minimum::InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) {
    std::optional<int64_t> result;
    ApplyToList(cell_ptr, [&](Object* obj_ptr) {
        obj_ptr = EvaluateObject(obj_ptr, scope);
        if (!Is<Number>(obj_ptr)) {
            throw RuntimeError("`" + GetRepr(obj_ptr) + "` is not a number");
        }
        if (!result) {
            result = As<Number>(obj_ptr)->GetValue();
        } else {
            result = std::min(*result, As<Number>(obj_ptr)->GetValue());
        }
    });
    if (!result) {
        throw RuntimeError("No arguments for `min` operation");
    }

    return scope->CreateServiceObject<Number>(*result);
}

Object* Maximum::InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) {
    std::optional<int64_t> result;
    ApplyToList(cell_ptr, [&](Object* obj_ptr) {
        obj_ptr = EvaluateObject(obj_ptr, scope);
        if (!Is<Number>(obj_ptr)) {
            throw RuntimeError("`" + GetRepr(obj_ptr) + "` is not a number");
        }
        if (!result) {
            result = As<Number>(obj_ptr)->GetValue();
        } else {
            result = std::max(*result, As<Number>(obj_ptr)->GetValue());
        }
    });
    if (!result) {
        throw RuntimeError("No arguments for `max` operation");
    }

    return scope->CreateServiceObject<Number>(*result);
}

Object* AbsoluteValue::InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) {
    Object* obj_ptr = EvaluateObject(cell_ptr->GetFirst(), scope);
    if (!Is<Number>(obj_ptr)) {
        throw RuntimeError("`" + GetRepr(obj_ptr) + "` is not a number");
    }
    return scope->CreateServiceObject<Number>(abs(As<Number>(obj_ptr)->GetValue()));
}

Object* IsPair::InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) {
    Object* obj_ptr = EvaluateObject(cell_ptr->GetFirst(), scope);
    if (obj_ptr && !Is<Cell>(obj_ptr)) {
        return scope->CreateServiceObject<Boolean>(false);
    }
    auto objects = ListToVector(As<Cell>(obj_ptr));
    return scope->CreateServiceObject<Boolean>(objects.size() == 2);
}

Object* IsNull::InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) {
    Object* obj_ptr = EvaluateObject(cell_ptr->GetFirst(), scope);
    if (obj_ptr && !Is<Cell>(obj_ptr)) {
        return scope->CreateServiceObject<Boolean>(false);
    }
    auto objects = ListToVector(As<Cell>(obj_ptr));
    return scope->CreateServiceObject<Boolean>(objects.size() == 0);
}

bool CheckProperList(Cell* cell_ptr) {
    while (cell_ptr) {
        if (Is<Cell>(cell_ptr->GetSecond())) {
            cell_ptr = As<Cell>(cell_ptr->GetSecond());
        } else {
            if (cell_ptr->GetSecond()) {
                return false;
            } else {
                return true;
            }
        }
    }
    return true;
}

Object* IsList::InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) {
    Object* obj_ptr = EvaluateObject(cell_ptr->GetFirst(), scope);
    if (obj_ptr && !Is<Cell>(obj_ptr)) {
        return scope->CreateServiceObject<Boolean>(false);
    }
    return scope->CreateServiceObject<Boolean>(CheckProperList(As<Cell>(obj_ptr)));
}

Object* ConsOperation::InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) {
    std::vector<Object*> evaluated_objects;
    ApplyToList(cell_ptr, [&](Object* obj_ptr) {
        evaluated_objects.push_back(EvaluateObject(obj_ptr, scope));
    });
    return VectorToImproperList(evaluated_objects, scope);
}

Object* CarOperation::InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) {
    Object* evaluated_obj = EvaluateObject(cell_ptr->GetFirst(), scope);
    if (!Is<Cell>(evaluated_obj)) {
        throw RuntimeError("`car` argument should be pair, not: `" + GetRepr(evaluated_obj) + "`");
    }
    return As<Cell>(evaluated_obj)->GetFirst();
}

Object* CdrOperation::InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) {
    Object* evaluated_obj = EvaluateObject(cell_ptr->GetFirst(), scope);
    if (!Is<Cell>(evaluated_obj)) {
        throw RuntimeError("`cdr` argument should be pair, not: `" + GetRepr(evaluated_obj) + "`");
    }
    return As<Cell>(evaluated_obj)->GetSecond();
}

Object* ListMaker::InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) {
    std::vector<Object*> evaluated_objects;
    ApplyToList(cell_ptr, [&](Object* obj_ptr) {
        evaluated_objects.push_back(EvaluateObject(obj_ptr, scope));
    });
    return VectorToProperList(evaluated_objects, scope);
}

Object* ListRef::InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) {
    std::vector<Object*> evaluated_objects;
    ApplyToList(cell_ptr, [&](Object* obj_ptr) {
        evaluated_objects.push_back(EvaluateObject(obj_ptr, scope));
    });
    if (!Is<Number>(evaluated_objects[1])) {
        throw RuntimeError("Second argument must be a number, but it is: `" +
                           GetRepr(evaluated_objects[1]) + "`");
    }
    if (evaluated_objects[0] && !Is<Cell>(evaluated_objects[0])) {
        throw RuntimeError("First argument must be a list, but it is: `" +
                           GetRepr(evaluated_objects[0]) + "`");
    }
    int64_t index = As<Number>(evaluated_objects[1])->GetValue();
    auto objects = ListToVector(As<Cell>(evaluated_objects[0]));
    if (index < 0 || index >= objects.size()) {
        throw RuntimeError("Index is out of range: `" + GetRepr(evaluated_objects[1]) +
                           "`, size: `" + std::to_string(objects.size()) + "`");
    }
    return objects[index];
}

Cell* FindKthNodeInList(Cell* cell_ptr, size_t k) {
    // assumming that the list has at least k elements
    // TODO: can be done with apply
    while (cell_ptr && k) {
        if (!As<Cell>(cell_ptr->GetSecond())) {
            throw RuntimeError("List must be proper!");
        }
        --k;
        cell_ptr = As<Cell>(cell_ptr->GetSecond());
    }
    return cell_ptr;
}

Object* ListTail::InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) {
    std::vector<Object*> evaluated_objects;
    ApplyToList(cell_ptr, [&](Object* obj_ptr) {
        evaluated_objects.push_back(EvaluateObject(obj_ptr, scope));
    });
    if (!Is<Number>(evaluated_objects[1])) {
        throw RuntimeError("Second argument must be a number, but it is: `" +
                           GetRepr(evaluated_objects[1]) + "`");
    }
    if (evaluated_objects[0] && !Is<Cell>(evaluated_objects[0])) {
        throw RuntimeError("First argument must be a list, but it is: `" +
                           GetRepr(evaluated_objects[0]) + "`");
    }
    int64_t index = As<Number>(evaluated_objects[1])->GetValue();
    auto objects = ListToVector(As<Cell>(evaluated_objects[0]));
    if (index < 0 || index > objects.size()) {
        throw RuntimeError("Index is out of range: `" + GetRepr(evaluated_objects[1]) +
                           "`, size: `" + std::to_string(objects.size()) + "`");
    }
    if (index == objects.size()) {
        return nullptr;
    }
    return FindKthNodeInList(As<Cell>(evaluated_objects[0]), index);
}

Object* IfStatement::InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) {
    // arguments quantity control is on InvokeImpl now, because we want to throw SyntaxError
    // instead of RuntimeError in this function
    std::vector<Object*> objects = ListToVector(cell_ptr);
    if (objects.size() < 2 || objects.size() > 3) {
        throw SyntaxError("Incorrect number of parameters for if statement");
    }

    Object* statement = EvaluateObject(objects[0], scope);
    if (!IsBooleanConstant(statement)) {
        throw RuntimeError("Statement value should be bool");
    }

    if (GetRepr(statement) == "#t") {
        return EvaluateObject(objects[1], scope);
    } else {
        return (objects.size() == 3) ? EvaluateObject(objects[2], scope) : nullptr;
    }
}

Object* CreateLambda(const std::vector<Object*>& arguments, const std::vector<Object*>& commands,
                     std::shared_ptr<Scope> scope) {
    if (commands.empty()) {
        throw SyntaxError("Function should do something");
    }

    std::unordered_set<std::string> declared_inside_lambda;
    std::vector<std::string> argument_names;
    for (Object* obj_ptr : arguments) {
        Symbol* symbol_ptr = As<Symbol>(obj_ptr);
        if (!symbol_ptr) {
            throw SyntaxError("Argument must be a symbol");
        }
        declared_inside_lambda.insert(symbol_ptr->GetName());
        argument_names.push_back(symbol_ptr->GetName());
    }

    std::unordered_map<std::string, Object*> capture;

    for (Object* expression : commands) {
        bool define_token = false;
        size_t index_obj = 0;

        ApplyToAllObjects(expression, [&](Object* obj_ptr) {
            Symbol* symbol_ptr = As<Symbol>(obj_ptr);
            ++index_obj;
            if (symbol_ptr && !IsBooleanConstant(symbol_ptr) &&
                capture.find(symbol_ptr->GetName()) == capture.end()) {
                if (symbol_ptr->GetName() == "define" && index_obj == 1) {
                    define_token = true;
                    return;
                }

                if (define_token && index_obj == 2) {
                    // declaring new object inside lambda
                    declared_inside_lambda.insert(symbol_ptr->GetName());
                    define_token = false;
                    return;
                }

                auto previous_scope_object = scope->GetObjectInAncestorScope(symbol_ptr->GetName());
                if (declared_inside_lambda.find(symbol_ptr->GetName()) ==
                        declared_inside_lambda.end() &&
                    previous_scope_object && !Is<StandartFunction>(*previous_scope_object)) {
                    capture[symbol_ptr->GetName()] = *previous_scope_object;
                }
            }
        });
    }

    return scope->CreateServiceObject<ScopedFunction>(argument_names, commands, capture);
}

Object* DefineFunction(std::vector<Object*>& objects, std::shared_ptr<Scope> scope) {
    Cell* cell_ptr = As<Cell>(objects[0]);
    Symbol* name_ptr = As<Symbol>(cell_ptr->GetFirst());
    if (!name_ptr) {
        throw SyntaxError("No function name");
    }

    std::vector<Object*> argnames;
    if (cell_ptr->GetSecond() && !Is<Cell>(cell_ptr->GetSecond())) {
        argnames.push_back(cell_ptr->GetSecond());
    } else if (Cell* arg_list = As<Cell>(cell_ptr->GetSecond())) {
        argnames = ListToVector(arg_list);
    }

    std::vector<Object*> commands;
    for (size_t i = 1; i < objects.size(); ++i) {
        commands.push_back(objects[i]);
    }

    return scope->NameObject(CreateLambda(argnames, commands, scope), name_ptr->GetName());
}

Object* DefineVariable(std::vector<Object*>& objects, std::shared_ptr<Scope> scope) {
    if (objects.size() != 2) {
        throw SyntaxError("Definition of variable must have 2 parameters");
    }
    Symbol* symbol_ptr = As<Symbol>(objects[0]);

    auto res = scope->NameObject(CopyObject(EvaluateObject(objects[1], scope), scope),
                                 symbol_ptr->GetName());
    return res;
}

Object* Definition::InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) {
    // arguments quantity control is on InvokeImpl now, because we want to throw SyntaxError
    // instead of RuntimeError in this function
    std::vector<Object*> objects = ListToVector(cell_ptr);
    if (objects.empty()) {
        throw SyntaxError("Zero parameters for definition");
    }

    if (Is<Cell>(objects[0])) {
        DefineFunction(objects, scope);
    } else if (Is<Symbol>(objects[0])) {
        DefineVariable(objects, scope);
    } else {
        throw SyntaxError("First parameter in the definition must be list or symbol");
    }

    return nullptr;
}

Object* MakeLambda::InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) {
    std::vector<Object*> objects = ListToVector(cell_ptr);
    if (objects.empty() || (objects[0] && !Is<Cell>(objects[0]))) {
        throw SyntaxError("No argument list for lambda");
    }

    std::vector<Object*> argnames = ListToVector(As<Cell>(objects[0]));
    std::vector<Object*> commands;

    for (size_t i = 1; i < objects.size(); ++i) {
        commands.push_back(objects[i]);
    }

    return CreateLambda(argnames, commands, scope);
}

Object* SetVariable::InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) {
    // arguments quantity control is on InvokeImpl now, because we want to throw SyntaxError
    // instead of RuntimeError in this function
    std::vector<Object*> objects = ListToVector(cell_ptr);
    if (objects.size() != 2) {
        throw SyntaxError("Wrong number of parameters for `set!`");
    }

    Symbol* symbol_ptr = As<Symbol>(objects[0]);
    if (!symbol_ptr) {
        throw SyntaxError("Name of variable is not a symbol");
    }

    if (!scope->GetObjectInAncestorScope(symbol_ptr->GetName())) {
        throw NameError("No such variable: `" + symbol_ptr->GetName() + "`");
    }

    return scope->NameObject(CopyObject(EvaluateObject(objects[1], scope), scope),
                             symbol_ptr->GetName());
}

std::pair<Cell*, Object*> SetCarCdrBody(Cell* cell_ptr, std::shared_ptr<Scope> scope) {
    std::vector<Object*> objects = ListToVector(cell_ptr);
    if (objects.size() != 2) {
        throw SyntaxError("Wrong number of parameters for setting tail or head");
    }

    Symbol* symbol_ptr = As<Symbol>(objects[0]);
    if (!symbol_ptr) {
        throw SyntaxError("Name of pair is not a symbol");
    }

    Cell* pair_ptr = As<Cell>(EvaluateObject(symbol_ptr, scope));

    if (!pair_ptr) {
        throw RuntimeError("Attempting to set tail or head on empty list");
    }

    return {pair_ptr, CopyObject(EvaluateObject(objects[1], scope), scope)};
}

Object* SetCar::InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) {
    auto [pair_ptr, obj_ptr] = SetCarCdrBody(cell_ptr, scope);
    pair_ptr->GetFirst() = obj_ptr;
    return pair_ptr;
}

Object* SetCdr::InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) {
    auto [pair_ptr, obj_ptr] = SetCarCdrBody(cell_ptr, scope);
    pair_ptr->GetSecond() = obj_ptr;
    return pair_ptr;
}

Object* IsSymbol::InvokeImpl(Cell* cell_ptr, std::shared_ptr<Scope> scope) {
    Object* evaluated = EvaluateObject(cell_ptr->GetFirst(), scope);
    return scope->CreateServiceObject<Boolean>(Is<Symbol>(evaluated) &&
                                               !IsBooleanConstant(evaluated));
}
