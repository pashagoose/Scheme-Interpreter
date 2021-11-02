#pragma once

#include "error.h"

#include <set>
#include <string>
#include <memory>


class Scope;

class Object {
public:
    Object() = default;

    virtual Object* Evaluate(
        std::shared_ptr<Scope> scope) = 0;  // throws if object does not evaluate

    virtual std::string Repr() const = 0;

    virtual Object* Copy(std::shared_ptr<Scope> scope) const = 0;

    virtual void GatherSubobjects(std::set<Object*>& save) {
        save.insert(this);
    }

    virtual ~Object() = default;
};
