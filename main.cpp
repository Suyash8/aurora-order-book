#include <iostream>
#include "OrderBook.h"

int main() {
    OrderBook book;
    
    book.addOrder(Order{1, 100, 100, OrderType::SELL});
    book.addOrder(Order{2, 105, 100, OrderType::BUY});
    book.addOrder(Order{3, 95, 100, OrderType::BUY});

    std::cout << std::endl;

    book.printInfo();

    book.match();
    return 0;
}