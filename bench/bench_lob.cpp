#include "order_book.h"
#include <chrono>
#include <iostream>
#include <vector>
#include <random>

// helper — returns nanoseconds elapsed
static uint64_t now_ns() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch()
    ).count();
}

int main() {
    const uint32_t NUM_ORDERS    = 1'000'000;   // 1M ops
    const uint32_t MAX_PRICE     = 200000;

    // ── Pre-generate random data (outside timed loop) ──
    std::mt19937 rng(42);
    std::uniform_int_distribution<uint64_t> price_dist(1000, 199000);
    std::uniform_int_distribution<uint64_t> qty_dist(1, 1000);
    std::uniform_int_distribution<int>      side_dist(0, 1);

    std::vector<uint64_t> prices(NUM_ORDERS);
    std::vector<uint64_t> quantities(NUM_ORDERS);
    std::vector<bool>     sides(NUM_ORDERS);

    for (uint32_t i = 0; i < NUM_ORDERS; i++) {
        prices[i]     = price_dist(rng);
        quantities[i] = qty_dist(rng);
        sides[i]      = side_dist(rng);
    }

    // ── Benchmark add_order() ──
    {
        OrderBook book(MAX_PRICE);
        uint64_t start = now_ns();

        for (uint32_t i = 0; i < NUM_ORDERS; i++) {
            book.add_order(i + 1, prices[i], quantities[i], sides[i]);
        }

        uint64_t elapsed = now_ns() - start;
        std::cout << "add_order()    — "
                  << elapsed / NUM_ORDERS << " ns/op  ("
                  << NUM_ORDERS << " ops)" << std::endl;
    }

    // ── Benchmark cancel_order() ──
    {
        OrderBook book(MAX_PRICE);

        // pre-populate
        for (uint32_t i = 0; i < NUM_ORDERS; i++) {
            book.add_order(i + 1, prices[i], quantities[i], sides[i]);
        }

        uint64_t start = now_ns();

        for (uint32_t i = 0; i < NUM_ORDERS; i++) {
            book.cancel_order(i + 1);
        }

        uint64_t elapsed = now_ns() - start;
        std::cout << "cancel_order() — "
                  << elapsed / NUM_ORDERS << " ns/op  ("
                  << NUM_ORDERS << " ops)" << std::endl;
    }

    // ── Benchmark execute_order() (full fill) ──
    {
        OrderBook book(MAX_PRICE);

        for (uint32_t i = 0; i < NUM_ORDERS; i++) {
            book.add_order(i + 1, prices[i], quantities[i], sides[i]);
        }

        uint64_t start = now_ns();

        for (uint32_t i = 0; i < NUM_ORDERS; i++) {
            book.execute_order(i + 1, quantities[i]);
        }

        uint64_t elapsed = now_ns() - start;
        std::cout << "execute_order()— "
                  << elapsed / NUM_ORDERS << " ns/op  ("
                  << NUM_ORDERS << " ops)" << std::endl;
    }

    return 0;
}