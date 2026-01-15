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