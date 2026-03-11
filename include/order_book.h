#pragma once
#include <vector>
#include<unordered_map>
#include "price_level.h"

struct OrderBook{
public:
    size_t getBidsSize() const { return bids.size(); }
    size_t getAsksSize() const { return asks.size(); }
    OrderBook(uint32_t max_price_ticks);
    void add_order(uint64_t order_id, uint64_t price, uint64_t quantity, bool is_bid);
    void cancel_order(uint64_t order_id);
// private:
    std::vector<PriceLevel*>             bids;
    std::vector<PriceLevel*>             asks;
    std::unordered_map<uint64_t, Order*> order_map;
    size_t getOrderMapSize() const { return order_map.size(); }
    PriceLevel*                          best_bid;
    PriceLevel*                          best_ask;
};