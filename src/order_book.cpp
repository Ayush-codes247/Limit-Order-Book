#include "order_book.h"
#include "order_pool.h"  
#include <chrono>
#include <iostream>

OrderBook::OrderBook(uint32_t max_price_ticks, uint32_t pool_size)
    // constructor logic
    : bids(max_price_ticks, nullptr)
    , asks(max_price_ticks, nullptr)
    , best_bid(nullptr)
    , best_ask(nullptr)
    , pool(pool_size)
{}

void OrderBook::add_order(uint64_t order_id, uint64_t price, uint64_t quantity, bool is_bid) {
    
    auto& side = is_bid ? bids : asks;          // select side

    if (side[price] == nullptr) {
        side[price] = new PriceLevel(price, 0, 0, nullptr, nullptr);
    }

    PriceLevel* level = side[price];

    Order* new_order = pool.acquire();
    if (new_order == nullptr) {
        std::cout << "[ERR] order pool exhausted" << std::endl;
        return;
    }
    new_order->order_id     = order_id;
    new_order->quantity     = quantity;
    new_order->is_bid       = is_bid;
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

void OrderBook::cancel_order(uint64_t order_id) {
    if (order_map.find(order_id) == order_map.end()) {
        std::cout << "[WARN] order " << order_id << " not found" << std::endl;
        return;
    }
    Order*      order = order_map[order_id];
    PriceLevel* level = order->level;

    // Case 1 — only order in list
    if (order->prev == nullptr && order->next == nullptr) {
        level->head = nullptr;
        level->tail = nullptr;
    }
    // Case 2 — order is head
    else if (order->prev == nullptr) {
        Order* tmp    = order->next;
        tmp->prev     = nullptr;
        order->next   = nullptr;
        level->head   = tmp;
    }
    // Case 3 — order is tail
    else if (order->next == nullptr) {
        Order* tmp    = order->prev;
        tmp->next     = nullptr;
        order->prev   = nullptr;
        level->tail   = tmp;
    }
    // Case 4 — middle
    else {
        order->prev->next = order->next;
        order->next->prev = order->prev;
    }
    // - update level->total_volume and order_count
    // - if level is empty, null out the array slot + update best_bid/ask
    // - delete order
    // - erase from order_map
    level->total_volume -= order->quantity;
    level->order_count -= 1;
    if (level->order_count == 0) {
        if (order->is_bid) {
            bids[level->price] = nullptr;
            if (best_bid == level)
                best_bid = nullptr;   // proper scan comes later
        } else {
            asks[level->price] = nullptr;
            if(best_ask == level)
                best_ask = nullptr;
        }
        delete level;
    }
    pool.release(order);
    order_map.erase(order_id);
}

void OrderBook::execute_order(uint64_t order_id, uint64_t quantity){
    // Case-1 : order_id not in book.
    if(order_map.find(order_id) == order_map.end()){
        std::cout << "[WARN] order " << order_id << " not found" << std::endl;
        return;
    }// Case-2 : quantity > order->quantity.
    Order* order = order_map[order_id];
    PriceLevel* level = order->level;
    if(quantity > order->quantity){
        std::cout << "[WARN] order " << order_id << " doesn't have enough quantity." << std::endl;
        return;
    }// Case-3 : quantity < order->quantity.
    else if(quantity < order->quantity){
        order->quantity -= quantity;
        level->total_volume -= quantity;
    }// Case-4 : quantity = order->quantity.
    else{
        cancel_order(order_id);
    }
}

OrderBook::~OrderBook() {
    for (auto& side : {bids, asks}) {        // iterate both sides
        for (PriceLevel* level : side) {     // each slot
            if (level == nullptr) continue;  // skip empty slots
            // walk the linked list and delete each Order
            delete level;
        }
    }
}




