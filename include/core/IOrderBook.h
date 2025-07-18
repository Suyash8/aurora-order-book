#pragma once

#include "DataTypes.h"

#include <cstdint>
#include <vector>

namespace aurora {
namespace core {

/**
 * @brief Interface for order book management and matching operations
 *
 * The order book maintains separate bid and ask sides with price-time priority,
 * handles order insertion, cancellation, and matching operations. It provides
 * the core functionality for maintaining market depth and executing trades
 * when orders can be matched.
 */
class IOrderBook {
public:
  /**
   * @brief Add a new order to the appropriate side of the order book
   *
   * Inserts the order into the bid or ask side based on the order's side,
   * maintaining price-time priority. Orders at the same price level are
   * ordered by arrival time (FIFO).
   *
   * @param order The order to be added to the book, must contain valid
   *              price, quantity, side, and timing information
   *
   * @note This method should maintain price-time priority ordering
   * @note Thread safety must be ensured for concurrent access
   */
  virtual void add_order(const Order &order) = 0;

  /**
   * @brief Cancel an existing order from the order book
   *
   * Removes the specified order from the book if it exists. The order
   * is identified by its unique order ID and removed from whichever
   * side and price level it currently occupies.
   *
   * @param order_id Unique identifier of the order to be cancelled
   *
   * @note If the order ID does not exist, this operation should be
   *       handled gracefully without throwing exceptions
   * @note Partial fills should be considered when cancelling orders
   */
  virtual void cancel_order(uint64_t order_id) = 0;

  /**
   * @brief Attempt to match an incoming order against the opposite book
   *
   * For buy orders, matches against the ask side; for sell orders, matches
   * against the bid side. Returns all trades that result from the matching
   * process. The order may be fully filled, partially filled, or not filled.
   *
   * @param order The incoming order to be matched against existing orders
   * @return Vector of Trade objects representing all executions that occurred
   *
   * @note Market orders should match immediately at available prices
   * @note Limit orders only match at their specified price or better
   * @note Partial fills should update the remaining quantity appropriately
   */
  virtual std::vector<Trade> match_order(const Order &order) = 0;

  /**
   * @brief Get a complete snapshot of the current order book state
   *
   * Returns a comprehensive view of all price levels on both bid and ask
   * sides, including aggregate quantities and individual orders at each level.
   * The snapshot represents the book state at the time of the call.
   *
   * @return BookSnapshot containing all current bid and ask price levels
   *         with their associated orders and aggregate information
   *
   * @note The snapshot should be consistent and represent a single point in
   * time
   * @note Price levels should be sorted appropriately (bids descending, asks
   * ascending)
   */
  virtual BookSnapshot get_snapshot() const = 0;

  /**
   * @brief Virtual destructor for proper cleanup of derived classes
   */
  virtual ~IOrderBook() = default;
};

} // namespace core
} // namespace aurora