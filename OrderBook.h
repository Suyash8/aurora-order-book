#pragma once

#include <map>
#include <vector>

#include "Order.h"
#include "Limit.h"

class OrderBook {
    std::map<double, Limit> asks;
    std::map<double, Limit, std::greater<double>> bids;

public:
    void addOrder(const Order& order);
    void printInfo();
    void match();
    bool cancelOrder(int orderId);
};