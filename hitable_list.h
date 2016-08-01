//
// Created by Tracy on 8/1/2016.
//

#ifndef RAY_TRACING_HITABLE_LIST_H
#define RAY_TRACING_HITABLE_LIST_H

#include "hitable.h"

#include <memory>

class hitable_list: public hitable {
public:
    hitable_list() {}
    hitable_list(std::unique_ptr<hitable>* l, int n): list(l), list_size(n) {}

    virtual bool hit(const ray &r, float t_min, float t_max, hit_record &rec) const override;

    std::unique_ptr<hitable>* list;
    int list_size;
};

#endif //RAY_TRACING_HITABLE_LIST_H
