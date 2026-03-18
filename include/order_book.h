#pragma once
#include <vector>
#include "order_pool.h"
#include <unordered_map>
#include "price_level.h"

struct OrderBook {
public:
    // Constructors
    OrderBook(uint32_t max_price_ticks, uint32_t pool_size = 1000000);

    // Core methods
    void add_order(uint64_t order_id, uint64_t price, uint64_t quantity, bool is_bid);
    void cancel_order(uint64_t order_id);
    void execute_order(uint64_t order_id, uint64_t quantity);

    // Getters
    size_t getBidsSize()     const { return bids.size(); }
    size_t getAsksSize()     const { return asks.size(); }
    size_t getOrderMapSize() const { return order_map.size(); }
    PriceLevel* getBestBid()             const { return best_bid; }
    PriceLevel* getBestAsk()             const { return best_ask; }
    PriceLevel* getBidLevel(uint64_t price) const { return bids[price]; }
    PriceLevel* getAskLevel(uint64_t price) const { return asks[price]; }
    bool        orderExists(uint64_t id)    const { return order_map.find(id) != order_map.end(); }
    // Destructor
    ~OrderBook();
private:
    OrderPool pool;
    std::vector<PriceLevel*>             bids;
    std::vector<PriceLevel*>             asks;
    std::unordered_map<uint64_t, Order*> order_map;
    PriceLevel*                          best_bid;
    PriceLevel*                          best_ask;
};