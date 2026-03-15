#pragma once
#include <vector>
#include <unordered_map>
#include "price_level.h"

struct OrderBook {
public:
    // Constructors
    OrderBook(uint32_t max_price_ticks);

    // Core methods
    void add_order(uint64_t order_id, uint64_t price, uint64_t quantity, bool is_bid);
    void cancel_order(uint64_t order_id);
    void execute_order(uint64_t order_id, uint64_t quantity);

    // Getters
    size_t getBidsSize()     const { return bids.size(); }
    size_t getAsksSize()     const { return asks.size(); }
    size_t getOrderMapSize() const { return order_map.size(); }
    
    // Destructor
    ~OrderBook();
// private:   ← uncomment this when done testing
    std::vector<PriceLevel*>             bids;
    std::vector<PriceLevel*>             asks;
    std::unordered_map<uint64_t, Order*> order_map;
    PriceLevel*                          best_bid;
    PriceLevel*                          best_ask;
};