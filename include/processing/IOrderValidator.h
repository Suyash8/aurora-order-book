#pragma once

#include "core/DataTypes.h"

#include <string>

namespace aurora {
namespace processing {

/**
 * @brief Interface for order validation services
 *
 * Defines the contract for components that validate orders before they are
 * processed by the matching engine. Implementations should check for valid
 * price ranges, quantities, instrument IDs, and other business rules to
 * ensure orders meet system requirements.
 */
class IOrderValidator {
public:
  /**
   * @brief Virtual destructor for proper cleanup
   *
   * Ensures proper cleanup of derived classes when the interface is
   * destroyed through a base class pointer.
   */
  virtual ~IOrderValidator() = default;

  /**
   * @brief Validate an order against system rules
   *
   * Checks if an order meets all business and technical requirements before
   * being accepted into the system. This may include price range validation,
   * minimum/maximum quantity checks, instrument availability, and other
   * business-specific rules.
   *
   * @param order The order to validate
   * @param error_message Output parameter to store validation error details
   * @return bool True if the order is valid, false otherwise with error_message
   * set
   */
  virtual bool validate(const core::Order &order,
                        std::string &error_message) = 0;
};

} // namespace processing
} // namespace aurora