#pragma once
#include<cstdint>

struct PriceLevel;

struct Order{
    uint64_t order_id;
    uint64_t quantity;
    uint64_t timestamp_ns; // ns epoch
    Order* prev;
    Order* next;
    PriceLevel* level;
};