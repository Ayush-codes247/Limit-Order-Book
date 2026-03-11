# O(1) Limit Order Book — C++ Implementation

> A high-performance Limit Order Book (LOB) built for ultra-low latency trading systems.
> Achieves **O(1) amortized complexity** for add, cancel, and execute operations using a
> price-indexed array mapped to doubly-linked order queues.

---

## Table of Contents

- [Motivation](#motivation)
- [Architecture](#architecture)
- [Data Structures](#data-structures)
- [Operation Complexity](#operation-complexity)
- [Project Structure](#project-structure)
- [Build Instructions](#build-instructions)
- [Benchmarks](#benchmarks)
- [Design Tradeoffs](#design-tradeoffs)
- [References](#references)

---

## Motivation

Standard LOB implementations using `std::map<price, queue>` incur **O(log n)** per operation
due to tree rebalancing. In HFT environments where systems process millions of order events
per second, this is a critical bottleneck.

This implementation eliminates that overhead by replacing the sorted map with a
**direct-address array** — trading memory for time, achieving true O(1) on the hot path.

---

## Architecture

The design is composed of three cooperating structures:

```
┌─────────────────────────────────────────────────────────────────┐
│                        ORDER BOOK                               │
│                                                                 │
│   bids[]   asks[]                                               │
│  ┌──────┐  ┌──────┐     ← Direct-address arrays (price → level)│
│  │ null │  │ null │                                             │
│  │  *───┼──┼─►PL  │     PriceLevel (PL):                       │
│  │ null │  │ null │       head ──► [A] ⇄ [B] ⇄ [C] ◄── tail   │
│  │  *───┼──┼─►PL  │                                            │
│  └──────┘  └──────┘                                             │
│                                                                 │
│   order_map: { id → Order* }   ← O(1) cancel teleport          │
│   best_bid*  best_ask*         ← Running best-price pointers    │
└─────────────────────────────────────────────────────────────────┘
```

### Layer 1 — Price Array

A flat array where the **index encodes the price** (scaled by tick size).

```
index:   0     1     2    ...  15000  15001  15002  ...
        [—]   [—]   [—]        [—]    [PL*]  [PL*]
                                        ↑      ↑
                                       BID    ASK
```

- Accessing a price level is `array[price_ticks]` — a single pointer dereference
- No hashing, no tree traversal, no comparisons

### Layer 2 — Price Level (Doubly-Linked FIFO Queue)

This is also very similar to a direct-addressing hashing method.
Each active price level maintains a **doubly-linked list** of resting orders in
arrival order (price-time priority).

```
PriceLevel @ $150.00
┌────────────────────────────────────────────────────────┐
│  price:        15000  (ticks)                          │
│  total_volume: 3500                                    │
│  order_count:  3                                       │
│                                                        │
│  head ──► [Order A] ⇄ [Order B] ⇄ [Order C] ◄── tail  │
└────────────────────────────────────────────────────────┘
```

Each `Order` node stores a **back-pointer to its parent PriceLevel** — the key
that enables O(1) cancel without any upward search.

### Layer 3 — Order Map

```
unordered_map<uint64_t order_id, Order*>
```

A hash map from order ID to a raw pointer directly into the linked list.
This is what makes cancel O(1) — you teleport to the node, splice it out,
and update the parent level, all without touching any other structure.

---

## Data Structures

### Order

```cpp
struct Order {
    uint64_t    order_id;
    uint64_t    quantity;
    uint64_t    timestamp_ns;   // nanosecond epoch
    Order*      prev;
    Order*      next;
    PriceLevel* level;          // back-pointer to parent level
};
```

### PriceLevel

```cpp
struct PriceLevel {
    uint64_t price;
    uint64_t total_volume;
    uint32_t order_count;
    Order*   head;
    Order*   tail;
};
```

### OrderBook

```cpp
class OrderBook {
    std::vector<PriceLevel*>              bids;
    std::vector<PriceLevel*>              asks;
    std::unordered_map<uint64_t, Order*>  order_map;
    PriceLevel*                           best_bid;
    PriceLevel*                           best_ask;
};
```

---

## Operation Complexity

| Operation      | Complexity | Notes                                          |
|----------------|------------|------------------------------------------------|
| Add Order      | O(1)       | Array lookup + list tail append                |
| Cancel Order   | O(1)       | Hash map lookup + doubly-linked list splice    |
| Execute (fill) | O(1)       | Pop from list head + volume update             |
| Best Bid/Ask   | O(1)       | Read running pointer                           |
| Level Volume   | O(1)       | Stored in PriceLevel.total_volume              |
| New Best Price | O(n)*      | Linear scan on level depletion — see tradeoffs |

> *The new-best-price scan is the only non-O(1) case. It is rare (only on full level
> exhaustion) and bounded by the tick range, not the number of orders.

---

## Project Structure

```
lob/
├── CMakeLists.txt
├── README.md
├── include/
│   ├── order.h              # Order node definition
│   ├── price_level.h        # PriceLevel definition
│   └── order_book.h         # OrderBook class interface
├── src/
│   ├── order_book.cpp       # Core LOB logic
│   └── main.cpp             # Driver / integration tests
└── bench/
    └── bench_lob.cpp        # Latency benchmarks (nanosecond timing)
```

---

## Build Instructions

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
./lob_bench
```

Recommended compile flags for benchmarking:

```
-O3 -march=native -std=c++17 -Wall -Wextra
```

---

## Benchmarks

> Results on: [your CPU], [RAM speed], Linux [version], GCC [version]
> Methodology: 10M operations, warm cache, `CLOCK_MONOTONIC` nanosecond timing

| Operation | Mean Latency | P99 Latency |
|-----------|-------------|-------------|
| Add       | — ns        | — ns        |
| Cancel    | — ns        | — ns        |
| Execute   | — ns        | — ns        |

*Fill in after running `./lob_bench`*

---

## Design Tradeoffs

### Memory vs. Speed
The price array pre-allocates slots for every possible tick. For a $0.01 tick
size and a $0–$1000 range, that's 100,000 slots × 8 bytes = ~800KB per side.
Acceptable for equities; consider a hash map fallback for options chains with
sparse, wide strike ranges.

### The Best-Price Update Problem
When a price level is fully consumed and `best_bid` / `best_ask` must move,
a linear scan is required to find the next occupied slot. Mitigations:

- **Acceptable as-is** — level depletion is rare vs. add/cancel volume
- **Fenwick tree** overlay for O(log n) next-best lookup
- **Doubly-linked level list** — link active PriceLevels together for O(1) traversal

### Memory Allocator
`new` / `delete` for Order nodes will cause allocator contention under load.
Production systems use a **slab allocator** or **object pool** — pre-allocate
a block of Order nodes and recycle them via a free list.

---

## References

- Voyiadjis & Zhu — *"How to Build a Fast Limit Order Book"*
- Jenkin, C. — *"Limit Order Book"* (2010 SSRN paper)
- [LMAX Disruptor](https://lmax-exchange.github.io/disruptor/) — lock-free ring buffer pattern used in order routing