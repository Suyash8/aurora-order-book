#pragma once

#include "processing/IMessageParser.h"

#include <nlohmann/json.hpp>

namespace aurora {
namespace processing {

/**
 * @brief Implementation of the message parser for JSON messages
 *
 * This class provides a concrete implementation of the IMessageParser interface
 * for parsing JSON-formatted messages into Order objects.
 */
class MessageParser : public IMessageParser {
public:
  /**
   * @brief Constructor for MessageParser
   */
  MessageParser() = default;

  /**
   * @brief Parse a raw JSON message into an order object
   *
   * @param raw_message The raw JSON message string to parse
   * @return std::optional<core::Order> The parsed order if successful, or empty
   * optional if parsing failed
   */
  std::optional<core::Order> parse(const std::string &raw_message) override;

private:
  /**
   * @brief Parse order type from string
   *
   * @param type_str The order type as a string
   * @return std::optional<core::OrderType> The parsed order type or empty
   * optional if invalid
   */
  std::optional<core::OrderType> parse_order_type(const std::string &type_str);

  /**
   * @brief Parse order side from string
   *
   * @param side_str The order side as a string
   * @return std::optional<core::Side> The parsed order side or empty optional
   * if invalid
   */
  std::optional<core::Side> parse_order_side(const std::string &side_str);
};

} // namespace processing
} // namespace aurora