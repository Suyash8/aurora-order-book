#pragma once

/**
 * @brief Enum class for order types
 *
 * This defines the types of orders that can be placed in the order book.
 */
enum class OrderType {
  /** @brief A limit order that specifies a price at which to buy or sell. */
  LIMIT,
  /** @brief A market order that executes immediately at the best available
     price. */
  MARKET,
  /** @brief An order that cancels an existing order in the book. */
  CANCEL
};

/**
 * @brief Enum class for order sides
 *
 * This defines the sides of an order in the order book.
 */
enum class Side {
  /** @brief Buy side of the order book */
  BUY,
  /** @brief Sell side of the order book */
  SELL
};

/**
 * @brief Enum class for order status
 *
 * This defines the possible statuses of an order in the order book.
 */
enum class OrderStatus {
  /** @brief The order is pending and has not yet been processed. */
  PENDING,
  /** @brief The order has been accepted into the book. */
  ACCEPTED,
  /** @brief The order has been partially filled. */
  PARTIAL_FILL,
  /** @brief The order has been completely filled. */
  FILLED,
  /** @brief The order has been cancelled. */
  CANCELLED,
  /** @brief The order was rejected due to some error or condition. */
  REJECTED
};