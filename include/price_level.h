#pragma once
#include<cstdint>
#include "order.h"

struct PriceLevel{
    uint64_t price;
    uint64_t total_volume;
    uint64_t order_count;
    Order* head;
    Order* tail;
    PriceLevel(uint64_t p, uint64_t vol, uint32_t count, Order* h, Order* t)
        : price(p), total_volume(vol), order_count(count), head(h), tail(t) {}
};