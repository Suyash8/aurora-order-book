#pragma once

#include "network/ISession.h"
#include "network/ITCPServer.h"

#include <boost/asio.hpp>
#include <memory>
#include <string>

namespace aurora {
namespace network {

/**
 * @brief Implementation of the client session using Boost.Asio
 *
 * This class provides a concrete implementation of the ISession interface
 * using Boost.Asio for asynchronous network I/O. It manages a single client
 * connection, handles message reading and writing, and provides proper
 * lifecycle management.
 */
class Session : public ISession, public std::enable_shared_from_this<Session> {
public:
  /**
   * @brief Constructor for Session
   *
   * Initializes a new session with the specified IO context and message
   * handler.
   *
   * @param io_context The Boost.Asio IO context to use for this session
   * @param message_handler The function to call when a message is received
   */
  Session(boost::asio::io_context &io_context,
          const ITCPServer::MessageHandler &message_handler);

  /**
   * @brief Start the session
   *
   * Begins reading messages from the client connection.
   */
  void start() override;

  /**
   * @brief Send a response message to the client
   *
   * @param response The response to send to the client
   */
  void send_response(const core::Response &response) override;

  /**
   * @brief Close the session
   *
   * Gracefully closes the client connection.
   */
  void close() override;

  /**
   * @brief Get the socket for this session
   *
   * @return Reference to the socket used by this session
   */
  boost::asio::ip::tcp::socket &socket() { return socket_; }

private:
  /**
   * @brief Start reading data from the client
   *
   * Sets up an asynchronous read operation to receive data from the client.
   */
  void start_read();

  /**
   * @brief Handle completion of a read operation
   *
   * Called when data has been read from the client.
   *
   * @param error Any error that occurred during the read operation
   * @param bytes_transferred The number of bytes read
   */
  void handle_read(const boost::system::error_code &error,
                   size_t bytes_transferred);

  /**
   * @brief Handle completion of a write operation
   *
   * Called when data has been written to the client.
   *
   * @param error Any error that occurred during the write operation
   */
  void handle_write(const boost::system::error_code &error);

  /**
   * @brief Convert a Response object to a JSON string
   *
   * @param response The Response object to convert
   * @return std::string The JSON representation of the response
   */
  std::string response_to_json(const core::Response &response);

  /** @brief Socket for the client connection */
  boost::asio::ip::tcp::socket socket_;

  /** @brief Buffer for reading incoming data from the client */
  std::array<char, 8192> read_buffer_;

  /** @brief Buffer for storing outgoing data to the client */
  std::string write_buffer_;

  /** @brief Handler function called when a message is received */
  ITCPServer::MessageHandler message_handler_;

  /** @brief Flag indicating whether the session has been closed */
  bool is_closed_;
};

} // namespace network
} // namespace aurora