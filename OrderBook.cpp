#include <iostream>

#include "OrderBook.h"

void OrderBook::addOrder(const Order& order) {
    orders.push_back(order);

    std::cout << "Order " << order.id << " added" << std::endl;
}