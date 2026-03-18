#include "order_book.h"
#include <iostream>

int main() {
    {
        OrderBook book(200000);

        // ─── Test 1: Single bid ───────────────────────────────────────────
        book.add_order(1001, 15000, 100, true);
        std::cout << "[T1] best_bid price: " << book.getBestBid()->price
                << " (expected 15000)" << std::endl;

        // ─── Test 2: Higher bid replaces best ─────────────────────────────
        book.add_order(1002, 15100, 200, true);
        std::cout << "[T2] best_bid price: " << book.getBestBid()->price
                << " (expected 15100)" << std::endl;

        // ─── Test 3: Lower bid does NOT replace best ──────────────────────
        book.add_order(1003, 14900, 150, true);
        std::cout << "[T3] best_bid price: " << book.getBestBid()->price
                << " (expected 15100 still)" << std::endl;

        // ─── Test 4: Single ask ───────────────────────────────────────────
        book.add_order(2001, 15200, 300, false);
        std::cout << "[T4] best_ask price: " << book.getBestAsk()->price 
                << " (expected 15200)" << std::endl;

        // ─── Test 5: Lower ask replaces best ──────────────────────────────
        book.add_order(2002, 15150, 250, false);
        std::cout << "[T5] best_ask price: " << book.getBestAsk()->price
                << " (expected 15150)" << std::endl;

        // ─── Test 6: Multiple orders at same price level ──────────────────
        book.add_order(1004, 15100, 400, true);
        std::cout << "[T6] level volume at 15100: " << book.getBidLevel(15100)->total_volume
                << " (expected 600)" << std::endl;
        std::cout << "[T6] order count at 15100: " << book.getBidLevel(15100)->order_count
                << " (expected 2)" << std::endl;

        // ─── Test 7: order_map registered correctly ───────────────────────
        std::cout << "[T7] order_map size: " << book.getOrderMapSize()
                << " (expected 6)" << std::endl;
    }

    // ─── cancel_order() tests ─────────────────────────────────────────
    {   
        OrderBook book(200000);
        // Setup — add orders at different prices and positions
        book.add_order(3001, 15000, 100, true);   // level with one order
        book.add_order(3002, 15050, 200, true);   // head of level
        book.add_order(3003, 15050, 300, true);   // middle of level
        book.add_order(3004, 15050, 400, true);   // tail of level
        book.add_order(3005, 15100, 500, true);   // best bid level

        // Test 8 — cancel only order in level (Case 1)
        book.cancel_order(3001);
        std::cout << "[T8]  bids[15000] null: " << (book.getBidLevel(15000) == nullptr)
                << " (expected 1)" << std::endl;

        // Test 9 — cancel head (Case 2)
        book.cancel_order(3002);
        std::cout << "[T9]  new head qty at 15050: " << book.getBidLevel(15050)->head->quantity
                << " (expected 300)" << std::endl;

        // Test 10 — cancel tail (Case 3)
        book.cancel_order(3004);
        std::cout << "[T10] new tail qty at 15050: " << book.getBidLevel(15050)->tail->quantity
                << " (expected 300)" << std::endl;

        // Test 11 — cancel middle / only remaining (Case 1 again)
        book.cancel_order(3003);
        std::cout << "[T11] bids[15050] null: " << (book.getBidLevel(15050) == nullptr)
                << " (expected 1)" << std::endl;

        // Test 12 — best_bid survives unrelated cancel
        book.cancel_order(3001);  // already deleted — this should crash or be handle
        std::cout << "[T12] best_bid price: " << book.getBestBid()->price
                << " (expected 15100)" << std::endl;

        // Test 13 — cancel best_bid level, best_bid goes nullptr
        book.cancel_order(3005);
        std::cout << "[T13] best_bid null: " << (book.getBestBid() == nullptr)
                << " (expected 1)" << std::endl;
    }
    // ─── execute_order() tests ────────────────────────────────────────
    {
        OrderBook book(200000);

        book.add_order(4001, 15000, 500, true);
        book.add_order(4002, 15000, 300, true);
        book.add_order(4003, 15100, 200, true);

        // Test 15 — partial fill
        book.execute_order(4001, 200);
        std::cout << "[T15] order 4001 qty: " << book.getBidLevel(15000)->head->quantity
                << " (expected 300)" << std::endl;
        std::cout << "[T15] level volume:   " << book.getBidLevel(15000)->total_volume
                << " (expected 600)" << std::endl;

        // Test 16 — full fill removes order
        book.execute_order(4001, 300);
        std::cout << "[T16] new head qty:   " << book.getBidLevel(15000)->head->quantity
                << " (expected 300)" << std::endl;
        std::cout << "[T16] order_map size: " << book.getOrderMapSize()
                << " (expected 2)" << std::endl;

        // Test 17 — full fill empties level
        book.execute_order(4002, 300);
        std::cout << "[T17] bids[15000] null: " << (book.getBidLevel(15000) == nullptr)
                << " (expected 1)" << std::endl;

        // Test 18 — overfill guard
        book.execute_order(4003, 999);
        std::cout << "[T18] order 4003 still exists: " << (book.orderExists(4003))
                << " (expected 1)" << std::endl;

        // Test 19 — invalid order guard
        book.execute_order(9999, 100);
        std::cout << "[T19] should print WARN above" << std::endl;
   }
}