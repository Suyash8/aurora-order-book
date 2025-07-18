#include "processing/MessageParser.h"
#include <iostream>

namespace aurora {
namespace processing {

std::optional<core::Order> MessageParser::parse(const std::string &raw_message) {
  try {
    // Parse the JSON message
    nlohmann::json json_message = nlohmann::json::parse(raw_message);
    
    // Check if this is an order message
    if (!json_message.contains("type") || json_message["type"] != "ORDER") {
      return std::nullopt;
    }
    
    // Extract and validate required fields
    if (!json_message.contains("order_id") ||
        !json_message.contains("instrument_id") ||
        !json_message.contains("order_type") ||
        !json_message.contains("side")) {
      return std::nullopt;
    }
    
    // Parse order type and side
    auto order_type = parse_order_type(json_message["order_type"]);
    auto order_side = parse_order_side(json_message["side"]);
    
    if (!order_type || !order_side) {
      return std::nullopt;
    }
    
    // Create the order object
    core::Order order;
    order.order_id = json_message["order_id"];
    order.instrument_id = json_message["instrument_id"];
    order.type = *order_type;
    order.side = *order_side;
    
    // Parse optional fields with defaults
    order.price = json_message.value("price", 0.0);
    order.quantity = json_message.value("quantity", 0);
    order.remaining_quantity = order.quantity;
    
    // Set timestamp to current time if not provided
    order.timestamp = std::chrono::high_resolution_clock::now();
    
    return order;
  } catch (const std::exception &e) {
    std::cerr << "Error parsing message: " << e.what() << std::endl;
    return std::nullopt;
  }
}

std::optional<core::OrderType> MessageParser::parse_order_type(const std::string &type_str) {
  if (type_str == "LIMIT") {
    return core::OrderType::LIMIT;
  } else if (type_str == "MARKET") {
    return core::OrderType::MARKET;
  } else if (type_str == "CANCEL") {
    return core::OrderType::CANCEL;
  } else {
    return std::nullopt;
  }
}

std::optional<core::Side> MessageParser::parse_order_side(const std::string &side_str) {
  if (side_str == "BUY") {
    return core::Side::BUY;
  } else if (side_str == "SELL") {
    return core::Side::SELL;
  } else {
    return std::nullopt;
  }
}

} // namespace processing
} // namespace aurora