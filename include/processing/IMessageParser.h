#pragma once

#include "DataTypes.h"

#include <optional>
#include <string>

namespace aurora {
namespace processing {

/**
 * @brief Interface for message parsing services
 *
 * Defines the contract for components that parse raw message strings into
 * structured order objects. Implementations should handle the transformation
 * of various message formats (e.g., JSON, FIX, binary) into the system's
 * internal order representation.
 */
class IMessageParser {
public:
  /**
   * @brief Virtual destructor for proper cleanup
   *
   * Ensures proper cleanup of derived classes when the interface is
   * destroyed through a base class pointer.
   */
  virtual ~IMessageParser() = default;

  /**
   * @brief Parse a raw message into an order object
   *
   * Converts a raw message string into the system's structured Order
   * representation. Returns an empty optional if the message cannot be parsed
   * successfully.
   *
   * @param raw_message The raw message string to parse
   * @return std::optional<core::Order> The parsed order if successful, or empty
   * optional if parsing failed
   */
  virtual std::optional<core::Order> parse(const std::string &raw_message) = 0;
};

} // namespace processing
} // namespace aurora