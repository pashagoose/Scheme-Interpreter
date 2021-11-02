#include "scope.h"
#include "garbage_collector.h"

std::optional<Object*> Scope::GetObjectInThisScope(const std::string& name) {
    auto it = objects_.find(name);
    if (it == objects_.end()) {
        return std::nullopt;
    }
    return it->second;
}

std::optional<Object*> Scope::GetObjectInAncestorScope(const std::string& name) {
    auto res = GetObjectInThisScope(name);
    return (parent_scope_ && !res) ? parent_scope_->GetObjectInAncestorScope(name) : res;
}

Object* Scope::NameObject(Object* obj_ptr, const std::string& name) {
    // name should not be in scope already, or it may cause "memory leaks" in the interpreter
    // environment
    objects_[name] = obj_ptr;
    return obj_ptr;
}

void GatherImpl(Scope* scope, std::set<Object*>& save) {
    for (auto [name, obj] : scope->objects_) {
        if (obj) {
            obj->GatherSubobjects(save);
        }
    }
    for (auto obj : scope->service_objects_) {
        if (obj) {
            obj->GatherSubobjects(save);
        }
    }
    
}

std::set<Object*> Scope::GatherReferredObjects() {
    std::set<Object*> result;
    GatherImpl(this, result);
    return result;
}

Scope::~Scope() {
    // TODO: write custom destructor for calling `clear` in garbage_collector
}