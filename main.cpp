#include <iostream>

#include "OrderBook.h"
#include "Limit.h"

int main() {
    OrderBook book;
    
    book.addOrder(Order{1, 100, 100, OrderType::SELL});
    book.addOrder(Order{2, 101, 50, OrderType::SELL});
    book.addOrder(Order{3, 120, 102, OrderType::BUY});
    book.addOrder(Order{4, 119, 150, OrderType::BUY});
    book.addOrder(Order{5, 100, 150, OrderType::BUY});
    book.addOrder(Order{6, 102, 75, OrderType::SELL});

    std::cout << std::endl;

    book.printInfo();

    book.match();
    book.printInfo();
    std::cout << "Order 4 " << (book.cancelOrder(4) ? "" : "not ") << "cancelled" << std::endl;
    book.printInfo();

    Limit lim(100);
    Order ord1{4, 100, 200, OrderType::BUY};
    Order ord2{5, 100, 150, OrderType::SELL};
    lim.addOrder(ord1);
    lim.addOrder(ord2);

    std::cout << "\nLimit 100.0 | Total Volume: " << lim.getTotalVolume() << std::endl;

    return 0;
}