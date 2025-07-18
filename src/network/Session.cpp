#include "network/Session.h"

#include <iostream>
#include <nlohmann/json.hpp>

namespace aurora {
namespace network {

Session::Session(boost::asio::io_context &io_context,
                 const ITCPServer::MessageHandler &message_handler)
    : socket_(io_context), message_handler_(message_handler),
      is_closed_(false) {}

void Session::start() { start_read(); }

void Session::send_response(const core::Response &response) {
  if (is_closed_) {
    throw std::runtime_error("Cannot send response on closed session");
  }

  // Convert response to JSON string
  write_buffer_ = response_to_json(response);
  write_buffer_ += "\n"; // Add newline for message framing

  // Send the response asynchronously
  boost::asio::async_write(
      socket_, boost::asio::buffer(write_buffer_),
      [self = shared_from_this()](const boost::system::error_code &error,
                                  size_t /*bytes_transferred*/) {
        self->handle_write(error);
      });
}

void Session::close() {
  if (!is_closed_) {
    is_closed_ = true;

    boost::system::error_code ec;
    if (socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec)) {
      std::cerr << "Socket shutdown error: " << ec.message() << std::endl;
    }
    if (socket_.close(ec)) {
      std::cerr << "Socket close error: " << ec.message() << std::endl;
    }
  }
}

void Session::start_read() {
  if (is_closed_) {
    return;
  }

  socket_.async_read_some(
      boost::asio::buffer(read_buffer_),
      [self = shared_from_this()](const boost::system::error_code &error,
                                  size_t bytes_transferred) {
        self->handle_read(error, bytes_transferred);
      });
}

void Session::handle_read(const boost::system::error_code &error,
                          size_t bytes_transferred) {
  if (!error) {
    // Process the received data
    std::string message(read_buffer_.data(), bytes_transferred);

    // Call the message handler if set
    if (message_handler_) {
      message_handler_(shared_from_this(), message);
    }

    // Continue reading
    start_read();
  } else if (error != boost::asio::error::operation_aborted) {
    // Connection closed or error occurred
    std::cerr << "Read error: " << error.message() << std::endl;
    close();
  }
}

void Session::handle_write(const boost::system::error_code &error) {
  if (error) {
    std::cerr << "Write error: " << error.message() << std::endl;
    close();
  }
}

std::string Session::response_to_json(const core::Response &response) {
  nlohmann::json json_response;

  // Convert OrderStatus enum to string
  std::string status_str;
  switch (response.status) {
  case core::OrderStatus::PENDING:
    status_str = "PENDING";
    break;
  case core::OrderStatus::ACCEPTED:
    status_str = "ACCEPTED";
    break;
  case core::OrderStatus::PARTIAL_FILL:
    status_str = "PARTIAL_FILL";
    break;
  case core::OrderStatus::FILLED:
    status_str = "FILLED";
    break;
  case core::OrderStatus::CANCELLED:
    status_str = "CANCELLED";
    break;
  case core::OrderStatus::REJECTED:
    status_str = "REJECTED";
    break;
  }

  // Populate JSON response
  json_response["order_id"] = response.order_id;
  json_response["status"] = status_str;
  json_response["filled_quantity"] = response.filled_quantity;
  json_response["remaining_quantity"] = response.remaining_quantity;
  json_response["fill_price"] = response.fill_price;
  json_response["message"] = response.message;

  return json_response.dump();
}

} // namespace network
} // namespace aurora