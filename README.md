# Aurora LOB

A minimal C++ Limit Order Book implementation.

## Features
- Scalable price limit levels using `std::map`.
- $O(1)$ order cancellation via `std::unordered_map` lookup.
- FIFO (Price-Time) priority matching.

## Components
- `Order`: ID, price, quantity, and side (BUY/SELL).
- `Limit`: List of orders at a specific price point.
- `OrderBook`: Core engine for managing bids, asks, and matching.

## Build & Run
```bash
g++ main.cpp OrderBook.cpp Limit.cpp -o lob
./lob
```
