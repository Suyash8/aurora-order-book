#pragma once

#include <map>
#include <list>
#include <unordered_map>
#include <functional>

#include "Order.h"
#include "Limit.h"

struct OrderEntry {
    std::list<Order>::iterator orderIterator;
    Limit* parentLimit;
};

class OrderBook {
    std::map<double, Limit> asks;
    std::map<double, Limit, std::greater<double>> bids;
    std::unordered_map<int, OrderEntry> orderLookup;

public:
    void addOrder(const Order& order);
    void printInfo();
    void match();
    bool cancelOrder(int orderId);
};