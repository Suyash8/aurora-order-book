#include "core/DataTypes.h"

#include <catch2/catch_test_macros.hpp>
#include <chrono>

using aurora::core::Order;
using aurora::core::OrderType;
using aurora::core::Side;

TEST_CASE("Order struct initialization") {
  Order order;
  order.order_id = 1;
  order.instrument_id = "AAPL";
  order.type = OrderType::LIMIT;
  order.side = Side::BUY;
  order.price = 150.0;
  order.quantity = 100;
  order.remaining_quantity = 100;
  auto now = std::chrono::high_resolution_clock::now();
  order.timestamp = now;

  REQUIRE(order.order_id == 1);
  REQUIRE(order.instrument_id == "AAPL");
  REQUIRE(order.type == OrderType::LIMIT);
  REQUIRE(order.side == Side::BUY);
  REQUIRE(order.price == 150.0);
  REQUIRE(order.quantity == 100);
  REQUIRE(order.remaining_quantity == 100);
  REQUIRE(order.timestamp == now);
}