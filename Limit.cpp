#include <list>
#include <vector>
#include <stdexcept>

#include "Order.h"
#include "Limit.h"

std::list<Order>::iterator Limit::addOrder(const Order& order) {
    if (order.price != price) throw std::invalid_argument("Order price does not match limit price");
    orders.push_back(order);
    totalVolume += order.quantity;
    return --orders.end();
}

bool Limit::deleteOrder(int orderId) {
    for (auto it = orders.begin(); it != orders.end(); ++it) {
        if (it->id == orderId) {
            totalVolume -= it->quantity;
            orders.erase(it);
            return true;
        }
    }
    return false;
}

void Limit::eraseOrder(std::list<Order>::iterator it) {
    totalVolume -= it->quantity;
    orders.erase(it);
}