#pragma once
#include "order.h"

struct OrderPool {
    Order*   pool;        // flat array of pre-allocated nodes
    Order*   free_list;   // stack of available nodes
    uint32_t capacity;

    OrderPool(uint32_t capacity);
    ~OrderPool();

    Order* acquire();
    void   release(Order* order);
};