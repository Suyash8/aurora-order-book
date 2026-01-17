#include <vector>
#include <stdexcept>

#include "Order.h"
#include "Limit.h"

void Limit::addOrder(Order& order) {
    if (order.price != price) throw std::invalid_argument("Order price does not match limit price");
    orders.push_back(order);
    totalVolume += order.quantity;
}