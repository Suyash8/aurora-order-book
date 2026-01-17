#include <algorithm>
#include <iostream>

#include "OrderBook.h"

template<typename Comparator>
void insertIntoLimitMap(std::map<double, Limit, Comparator>& limitMap, Order order) {
    auto it = limitMap.emplace(order.price, Limit(order.price)).first;
    it->second.addOrder(order);
}

void OrderBook::addOrder(const Order& order) {
    if (order.type == OrderType::BUY)
        insertIntoLimitMap(bids, order);
    else
        insertIntoLimitMap(asks, order);

    std::cout << "Order " << order.id << " added" << std::endl;
}

void OrderBook::printInfo() {
    std::cout << "Order Book Stats: " << std::endl;
    std::cout << "Bids: " << bids.size() << std::endl;
    std::cout << "Asks: " << asks.size() << std::endl;
}

void OrderBook::match() {
    #if 0
    while (true) {
        if (bids.empty() || asks.empty()) return;

        size_t bestBid = 0, bestAsk = 0;
        for (size_t i = 0; i < bids.size(); ++i)
            if (bids[i].price > bids[bestBid].price) bestBid = i;
        
        for (size_t i = 0; i < asks.size(); ++i) 
            if (asks[i].price < asks[bestAsk].price) bestAsk = i;

        if (bids[bestBid].price < asks[bestAsk].price) return;

        std::cout << "Match: Bid " << bids[bestBid].id << " vs Ask " << asks[bestAsk].id << std::endl;

        int quantity = std::min(bids[bestBid].quantity, asks[bestAsk].quantity);

        double executionPrice;
        if (bids[bestBid].id < asks[bestAsk].id)
            executionPrice = bids[bestBid].price;
        else
            executionPrice = asks[bestAsk].price;

        std::cout << "Execute " << quantity << " shares @ $" << executionPrice << std::endl;

        bids[bestBid].quantity -= quantity;
        asks[bestAsk].quantity -= quantity;

        if (bids[bestBid].quantity == 0) bids.erase(bids.begin() + bestBid);
        if (asks[bestAsk].quantity == 0) asks.erase(asks.begin() + bestAsk);
    }
    #endif
}

bool OrderBook::cancelOrder(int orderId) {
    #if 0
    if (bids.empty() && asks.empty()) return false;

    auto it = std::find_if(bids.begin(), bids.end(), [orderId](const auto& order){ return order.id == orderId; });
    if (it != bids.end()) {
        bids.erase(it);
        return true;
    }

    it = std::find_if(asks.begin(), asks.end(), [orderId](const auto& order){ return order.id == orderId; });
    if (it != asks.end()) {
        asks.erase(it);
        return true;
    }
    #endif

    return false;
}