#include <iostream>
#include "Order.h"

int main() {
    Order myOrder = {1, 100.50, 10, OrderType::BUY};
    std::cout << "Order Initialized" << std::endl << "ID: " << myOrder.id << ", Price: $" << myOrder.price << std::endl;
    return 0;
}