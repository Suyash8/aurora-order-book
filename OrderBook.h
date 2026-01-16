#pragma once

#include <vector>
#include "Order.h"

class OrderBook {
    std::vector<Order> asks;
    std::vector<Order> bids;

public:
    void addOrder(const Order& order);
    void printInfo();
    void match();
    bool cancelOrder(int orderId);
};