#pragma once

#include "Enums.h"

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

namespace aurora {
namespace core {

/**
 * @brief Represents an order in the trading system
 *
 * Contains all information related to a single order including its
 * identification, pricing, quantity, and timing details.
 */
struct Order {
  /** @brief Unique identifier for the order */
  uint64_t order_id;
  /** @brief Identifier for the financial instrument being traded */
  std::string instrument_id;
  /** @brief Type of the order (LIMIT, MARKET, CANCEL) */
  OrderType type;
  /** @brief Direction of the order (BUY or SELL) */
  Side side;
  /** @brief Price at which the order is placed */
  double price;
  /** @brief Total quantity of the order */
  uint64_t quantity;
  /** @brief Quantity that has not yet been filled */
  uint64_t remaining_quantity;
  /** @brief Timestamp when the order was created */
  std::chrono::high_resolution_clock::time_point timestamp;
};

/**
 * @brief Represents a completed trade between two orders
 *
 * Contains information about a transaction that occurred when a buy order
 * matched with a sell order in the trading system.
 */
struct Trade {
  /** @brief Unique identifier for the trade */
  uint64_t trade_id;
  /** @brief Identifier of the buy order involved in this trade */
  uint64_t buy_order_id;
  /** @brief Identifier of the sell order involved in this trade */
  uint64_t sell_order_id;
  /** @brief Identifier for the financial instrument that was traded */
  std::string instrument_id;
  /** @brief Price at which the trade was executed */
  double price;
  /** @brief Quantity of the instrument that was traded */
  uint64_t quantity;
  /** @brief Timestamp when the trade occurred */
  std::chrono::high_resolution_clock::time_point timestamp;
};

/**
 * @brief Contains statistical information about the market for a specific
 * instrument
 *
 * Aggregates key market metrics including price information, depth, volume, and
 * activity.
 */
struct MarketStats {
  /** @brief Identifier for the financial instrument */
  std::string instrument_id;
  /** @brief Highest price a buyer is willing to pay */
  double best_bid;
  /** @brief Lowest price a seller is willing to accept */
  double best_ask;
  /** @brief Difference between best ask and best bid prices */
  double spread;
  /** @brief Total quantity available at all bid price levels */
  uint64_t bid_depth;
  /** @brief Total quantity available at all ask price levels */
  uint64_t ask_depth;
  /** @brief Total volume traded for this instrument */
  uint64_t total_volume;
  /** @brief Number of trades executed for this instrument */
  uint64_t trade_count;
  /** @brief Timestamp of when these statistics were last updated */
  std::chrono::high_resolution_clock::time_point last_update;
};

/**
 * @brief Represents a price level in the order book
 *
 * Contains aggregated information about all orders at a specific price point,
 * including the total quantity and number of orders at this level.
 */
struct PriceLevel {
  /** @brief The price point for this level */
  double price;
  /** @brief Total quantity of all orders at this price level */
  uint64_t aggregate_quantity;
  /** @brief Number of individual orders at this price level */
  uint64_t order_count;
  /** @brief Collection of all orders at this price level */
  std::vector<Order> orders;
};

/**
 * @brief Represents a complete snapshot of the order book at a specific moment
 *
 * Contains all bid and ask price levels for a specific instrument at a given
 * point in time, providing a comprehensive view of the market depth.
 */
struct BookSnapshot {
  /** @brief Identifier for the financial instrument */
  std::string instrument_id;
  /** @brief Timestamp when this snapshot was taken */
  std::chrono::high_resolution_clock::time_point timestamp;
  /** @brief Collection of all bid price levels, typically sorted in descending
   * order by price */
  std::vector<PriceLevel> bids;
  /** @brief Collection of all ask price levels, typically sorted in ascending
   * order by price */
  std::vector<PriceLevel> asks;
};

} // namespace core
} // namespace aurora