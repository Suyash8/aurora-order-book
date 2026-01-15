#pragma once

#include <vector>
#include "Order.h"

class OrderBook {
    std::vector<Order> orders;

public:
    void addOrder(const Order& order);
};