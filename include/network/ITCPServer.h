#pragma once

#include <cstdint>
#include <functional>
#include <memory>

namespace aurora {
namespace network {

class ISession;

/**
 * @brief Interface for TCP server implementations
 *
 * Defines the contract for TCP server implementations that handle network
 * communication for the order book system. Implementations should manage
 * client connections, handle incoming requests, and provide proper lifecycle
 * management.
 */
class ITCPServer {
public:
  /**
   * @brief Start the TCP server on the specified port
   *
   * Initializes the server and begins listening for incoming connections
   * on the specified port. The server should be ready to accept client
   * connections after this method returns successfully.
   *
   * @param port The port number on which to start the server
   * @throws std::runtime_error if the server cannot be started (e.g., port
   * already in use)
   */
  virtual void start(uint16_t port) = 0;

  /**
   * @brief Stop the TCP server
   *
   * Gracefully shuts down the server, closing all active connections and
   * releasing any resources. After this method returns, the server should
   * no longer accept new connections or process existing ones.
   */
  virtual void stop() = 0;

  /**
   * @brief Virtual destructor for proper cleanup
   *
   * Ensures proper cleanup of derived classes when the interface is
   * destroyed through a base class pointer.
   */
  virtual ~ITCPServer() = default;

  /**
   * @brief Type definition for message handler function
   *
   * Defines the signature for callback functions that process incoming
   * messages from clients. The handler receives the client session and
   * the message content.
   */
  using MessageHandler =
      std::function<void(std::shared_ptr<ISession>, const std::string &)>;

  /**
   * @brief Set the message handler for incoming client messages
   *
   * Registers a callback function that will be invoked when a message
   * is received from any client. This handler is responsible for processing
   * the message and generating appropriate responses.
   *
   * @param handler The function to be called when a message is received
   */
  virtual void set_message_handler(const MessageHandler &handler) = 0;
};

} // namespace network
} // namespace aurora