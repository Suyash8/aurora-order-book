#include <iostream>
#include "OrderBook.h"

int main() {
    OrderBook book;
    
    book.addOrder(Order{1, 100, 10, OrderType::BUY});
    book.addOrder(Order{2, 150, 5, OrderType::SELL});
    book.addOrder(Order{3, 120, 20, OrderType::BUY});

    std::cout << "3 orderes added." << std::endl;
    return 0;
}