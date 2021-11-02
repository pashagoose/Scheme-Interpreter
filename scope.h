#pragma once

#include "garbage_collector.h"
#include "abstract_object.h"
#include "error.h"

#include <list>
#include <memory>
#include <string>
#include <unordered_map>

class Scope {
public:
	friend void GatherImpl(Scope* scope, std::set<Object*>& save);

    Scope() = default;  // constructor for independent scope (for example global one)
    Scope(Scope* scope_parent) : parent_scope_(scope_parent) {
    }

    template <class T, class... Args>
    T* CreateServiceObject(Args&&... args) {  // for syntax tree nodes
        T* ptr = garbage_collector::Instance().RegisterObject<T>(std::forward<Args>(args)...);
        service_objects_.push_back(ptr);
        return ptr;
    }

    template <class T, class... Args>
    T* CreateObject(const std::string& name, Args&&... args) {
        if (objects_.find(name) != objects_.end()) {
            // I guess, we need to erase old object here and insert new one
            throw RuntimeError("Duplicate variable or function names: `" + name + "`");
        }
        T* ptr = garbage_collector::Instance().RegisterObject<T>(std::forward<Args>(args)...);
        objects_[name] = ptr;
        return ptr;
    }

    std::optional<Object*> GetObjectInThisScope(const std::string& name);

    std::optional<Object*> GetObjectInAncestorScope(const std::string& name);

    Object* NameObject(Object* obj_ptr, const std::string& name);

    std::set<Object*> GatherReferredObjects();

    ~Scope();

private:
    std::unordered_map<std::string, Object*> objects_;  // may be functions or variables
    std::list<Object*> service_objects_;

private:
    Scope* parent_scope_ = nullptr;
};