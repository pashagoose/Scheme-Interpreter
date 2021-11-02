#pragma once

#include "abstract_object.h"

#include <iostream>
#include <vector>
#include <set>

namespace garbage_collector {

class GarbageCollector {  // singleton
public:
    GarbageCollector() = default;

    template <class T, class... Args>
    T* RegisterObject(Args&&... args) {
        T* new_ptr = new T(std::forward<Args>(args)...);
        objects_.push_back(new_ptr);
        return new_ptr;
    }

    void CollectExcept(std::set<Object*> leave_ptrs) {
        std::vector<Object*> new_objects;
        for (auto obj_ptr : objects_) {
            if (!leave_ptrs.contains(obj_ptr)) {
                delete obj_ptr;
            } else {
                new_objects.push_back(obj_ptr);
            }
        }
        objects_ = new_objects;
    }

    ~GarbageCollector() {
        for (auto obj_ptr : objects_) {
            delete obj_ptr;
        }
    }

private:
    std::vector<Object*> objects_;
};

static inline GarbageCollector& Instance() {
    static GarbageCollector gc;
    return gc;
}

}  // namespace garbage_collector