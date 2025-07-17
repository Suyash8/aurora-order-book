#pragma once

#include "DataTypes.h"

namespace aurora {
namespace core {

/**
 * @brief Interface for the matching engine component
 *
 * The matching engine is responsible for processing incoming orders,
 * routing them to appropriate order books, and coordinating the overall
 * order processing pipeline. It serves as the central orchestrator for
 * all trading operations in the system.
 */
class IMatchingEngine {
public:
  /**
   * @brief Process an incoming order through the matching engine
   *
   * This method handles the complete order processing pipeline including
   * validation, routing to the appropriate instrument's order book,
   * matching against existing orders, and generating trade reports.
   *
   * @param order The order to be processed, containing all necessary
   *              information including instrument ID, type, side, price,
   *              and quantity
   *
   * @note This method should be thread-safe as it may be called
   *       concurrently from multiple threads
   * @note The implementation should handle all order types (LIMIT,
   *       MARKET, CANCEL) appropriately
   */
  virtual void process_order(const Order &order) = 0;

  /**
   * @brief Virtual destructor for proper cleanup of derived classes
   */
  virtual ~IMatchingEngine() = default;
};

} // namespace core
} // namespace aurora