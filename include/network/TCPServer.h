#pragma once

#include "network/ISession.h"
#include "network/ITCPServer.h"

#include <boost/asio.hpp>
#include <memory>
#include <thread>
#include <vector>

namespace aurora {
namespace network {

/**
 * @brief Implementation of the TCP server using Boost.Asio
 *
 * This class provides a concrete implementation of the ITCPServer interface
 * using Boost.Asio for asynchronous network I/O. It manages client connections,
 * handles incoming messages, and provides proper lifecycle management.
 */
class TCPServer : public ITCPServer {
public:
  /**
   * @brief Constructor for TCPServer
   *
   * Initializes a new TCP server instance with the specified number of I/O
   * threads.
   *
   * @param io_threads The number of threads to use for I/O operations
   */
  explicit TCPServer(size_t io_threads = 4);

  /**
   * @brief Destructor for TCPServer
   *
   * Ensures the server is properly stopped and resources are released.
   */
  ~TCPServer() override;

  /**
   * @brief Start the TCP server on the specified port
   *
   * @param port The port number on which to start the server
   * @throws std::runtime_error if the server cannot be started
   */
  void start(uint16_t port) override;

  /**
   * @brief Stop the TCP server
   *
   * Gracefully shuts down the server and all active connections.
   */
  void stop() override;

  /**
   * @brief Set the message handler for incoming client messages
   *
   * @param handler The function to be called when a message is received
   */
  void set_message_handler(const MessageHandler &handler) override;

private:
  /**
   * @brief Start accepting incoming connections
   *
   * Sets up the acceptor to listen for and accept new client connections.
   */
  void start_accept();

  /**
   * @brief Handle a new client connection
   *
   * Called when a new client connection is established.
   *
   * @param session The newly created session for the client
   * @param error Any error that occurred during the accept operation
   */
  void handle_accept(std::shared_ptr<ISession> session,
                     const boost::system::error_code &error);

  /** @brief IO context that provides core functionality for asynchronous I/O */
  boost::asio::io_context io_context_;

  /** @brief TCP acceptor that listens for incoming connections */
  boost::asio::ip::tcp::acceptor acceptor_;

  /** @brief Collection of threads that process asynchronous operations */
  std::vector<std::thread> io_threads_;

  /** @brief Handler function called when a message is received from a client */
  MessageHandler message_handler_;

  /** @brief Flag indicating whether the server is currently running */
  bool running_;

  /** @brief Number of I/O threads to use for the server */
  size_t num_io_threads_;
};

} // namespace network
} // namespace aurora