#pragma once

#include <vector>
#include "Order.h"

class Limit {
    double price;
    double totalVolume;
    std::vector<Order> orders;

public:
    Limit(double limit_price) : price(limit_price), totalVolume(0) {}
    double getPrice() const { return price; }
    double getTotalVolume() const { return totalVolume; }

    void addOrder(Order& order);
};