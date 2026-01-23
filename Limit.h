#pragma once

#include <list>
#include "Order.h"

class Limit {
    double price;
    double totalVolume;
    std::list<Order> orders;

public:
    Limit(double limit_price) : price(limit_price), totalVolume(0) {}
    double getPrice() const { return price; }
    double getTotalVolume() const { return totalVolume; }
    std::list<Order>& getOrders() { return orders; }
    bool isEmpty() const { return orders.empty(); }

    std::list<Order>::iterator addOrder(const Order& order);
    bool deleteOrder(int orderId);
};