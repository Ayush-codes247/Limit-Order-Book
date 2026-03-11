#include "order_book.h"
#include <iostream>

int main() {
    OrderBook book(200000);

    // ─── Test 1: Single bid ───────────────────────────────────────────
    book.add_order(1001, 15000, 100, true);
    std::cout << "[T1] best_bid price: " << book.best_bid->price 
              << " (expected 15000)" << std::endl;

    // ─── Test 2: Higher bid replaces best ─────────────────────────────
    book.add_order(1002, 15100, 200, true);
    std::cout << "[T2] best_bid price: " << book.best_bid->price 
              << " (expected 15100)" << std::endl;

    // ─── Test 3: Lower bid does NOT replace best ──────────────────────
    book.add_order(1003, 14900, 150, true);
    std::cout << "[T3] best_bid price: " << book.best_bid->price 
              << " (expected 15100 still)" << std::endl;

    // ─── Test 4: Single ask ───────────────────────────────────────────
    book.add_order(2001, 15200, 300, false);
    std::cout << "[T4] best_ask price: " << book.best_ask->price 
              << " (expected 15200)" << std::endl;

    // ─── Test 5: Lower ask replaces best ──────────────────────────────
    book.add_order(2002, 15150, 250, false);
    std::cout << "[T5] best_ask price: " << book.best_ask->price 
              << " (expected 15150)" << std::endl;

    // ─── Test 6: Multiple orders at same price level ──────────────────
    book.add_order(1004, 15100, 400, true);
    std::cout << "[T6] level volume at 15100: " << book.bids[15100]->total_volume
              << " (expected 600)" << std::endl;
    std::cout << "[T6] order count at 15100: " << book.bids[15100]->order_count
              << " (expected 2)" << std::endl;

    // ─── Test 7: order_map registered correctly ───────────────────────
    std::cout << "[T7] order_map size: " << book.getOrderMapSize()
              << " (expected 6)" << std::endl;

    return 0;
}