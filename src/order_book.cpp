#include "order_book.h"  
#include <chrono>

OrderBook::OrderBook(uint32_t max_price_ticks)
    // constructor logic
    : bids(max_price_ticks, nullptr)
    , asks(max_price_ticks, nullptr)
    , best_bid(nullptr)
    , best_ask(nullptr)
{}

void OrderBook::add_order(uint64_t order_id, uint64_t price, uint64_t quantity, bool is_bid) {
    
    auto& side = is_bid ? bids : asks;          // select side

    if (side[price] == nullptr) {
        side[price] = new PriceLevel(price, 0, 0, nullptr, nullptr);
    }

    PriceLevel* level = side[price];

    Order* new_order        = new Order();
    new_order->order_id     = order_id;
    new_order->quantity     = quantity;
    new_order->level        = level;
    new_order->timestamp_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
                                  std::chrono::steady_clock::now().time_since_epoch()
                              ).count();

    if (level->head == nullptr) {
        level->head       = new_order;
        level->tail       = new_order;
        new_order->prev   = nullptr;
        new_order->next   = nullptr;
    } else {
        new_order->prev       = level->tail;
        level->tail->next     = new_order;
        new_order->next       = nullptr;
        level->tail           = new_order;
    }

    level->total_volume += quantity;
    level->order_count  += 1;
    order_map[order_id]  = new_order;

    if (is_bid) {
        if (best_bid == nullptr || best_bid->price < level->price)
            best_bid = level;
    } else {
        if (best_ask == nullptr || best_ask->price > level->price)
            best_ask = level;
    }
}



