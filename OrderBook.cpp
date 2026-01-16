#include <iostream>

#include "OrderBook.h"

void OrderBook::addOrder(const Order& order) {
    if (order.type == OrderType::BUY) {
        bids.push_back(order);
    } else {
        asks.push_back(order);
    }

    std::cout << "Order " << order.id << " added" << std::endl;
}

void OrderBook::printInfo() {
    std::cout << "Order Book Stats: " << std::endl;
    std::cout << "Bids: " << bids.size() << std::endl;
    std::cout << "Asks: " << asks.size() << std::endl;
}

void OrderBook::match() {
    if (bids.empty() || asks.empty()) return;

    size_t bestBid = 0, bestAsk = 0;
    for (size_t i = 0; i < bids.size(); ++i)
        if (bids[i].price > bids[bestBid].price) bestBid = i;
    
    for (size_t i = 0; i < asks.size(); ++i) 
        if (asks[i].price < asks[bestAsk].price) bestAsk = i;

    if (bids[bestBid].price > asks[bestAsk].price)
        std::cout << "Match: Bid " << bids[bestBid].id << " vs Ask " << asks[bestAsk].id << std::endl;
}