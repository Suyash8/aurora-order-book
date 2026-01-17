#include <algorithm>
#include <iostream>

#include "OrderBook.h"
#include "Limit.h"

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
    while (true) {
        if (bids.empty() || asks.empty()) break;

        auto& bestBidLimit = bids.begin()->second;
        auto& bestAskLimit = asks.begin()->second;

        if (bids.begin()->first < asks.begin()->first) break;

        Order& bestBid = bestBidLimit.getOrders().front();
        Order& bestAsk = bestAskLimit.getOrders().front();

        int quantity = std::min(bestBid.quantity, bestAsk.quantity);

        double executionPrice;
        if (bestBid.id < bestAsk.id)
            executionPrice = bestBid.price;
        else
            executionPrice = bestAsk.price;

        std::cout << "Execute " << quantity << " shares @ $" << executionPrice << std::endl;

        bestBid.quantity -= quantity;
        bestAsk.quantity -= quantity;

        if (bestBid.quantity == 0) bestBidLimit.getOrders().erase(bestBidLimit.getOrders().begin());
        if (bestAsk.quantity == 0) bestAskLimit.getOrders().erase(bestAskLimit.getOrders().begin());

        if (bestBidLimit.isEmpty()) bids.erase(bids.begin());
        if (bestAskLimit.isEmpty()) asks.erase(asks.begin());
    }
}

template<typename Comparator>
bool cancelOrderFromLimitMap(std::map<double, Limit, Comparator>& limitMap, int orderId) {
    for (auto& [price, limit] : limitMap) {
        if (limit.deleteOrder(orderId)) {
            if (limit.isEmpty()) limitMap.erase(price);
            return true;
        }
    }
    return false;
}

bool OrderBook::cancelOrder(int orderId) {
    if (cancelOrderFromLimitMap(bids, orderId)) return true;
    if (cancelOrderFromLimitMap(asks, orderId)) return true;
    return false;
}