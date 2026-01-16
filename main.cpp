#include <iostream>
#include "OrderBook.h"

int main() {
    OrderBook book;
    
    book.addOrder(Order{1, 100, 100, OrderType::SELL});
    book.addOrder(Order{2, 101, 50, OrderType::SELL});
    book.addOrder(Order{3, 120, 102, OrderType::BUY});

    std::cout << std::endl;

    book.printInfo();
    book.match();
    book.printInfo();
    std::cout << "Order 2 " << (book.cancelOrder(2) ? "" : "not ") << "cancelled" << std::endl;
    book.printInfo();
    return 0;
}