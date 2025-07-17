#pragma once

#include "DataTypes.h"

#include <string>

namespace aurora {
namespace network {

/**
 * @brief Interface for client session management
 *
 * Defines the contract for handling individual client sessions in the TCP
 * server. Each session represents a connection with a single client and
 * provides methods for communication lifecycle management including starting
 * the session, sending responses, and proper cleanup.
 */
class ISession {
public:
  /**
   * @brief Start the session
   *
   * Initializes the session and begins processing incoming requests from
   * the connected client. This method should set up any necessary resources
   * and begin listening for incoming data on the connection.
   */
  virtual void start() = 0;

  /**
   * @brief Send a response message to the client
   *
   * Transmits a structured response message back to the connected client.
   * Uses the core::Response type which contains information about order
   * status, execution details, and any relevant messages.
   *
   * @param response_message The structured response to send to the client
   * @throws std::runtime_error if the message cannot be sent (e.g., connection
   * closed)
   */
  virtual void send_response(const core::Response &response_message) = 0;

  /**
   * @brief Close the session
   *
   * Gracefully closes the client connection and releases any resources
   * associated with this session. After this method returns, the session
   * should no longer be used for communication.
   */
  virtual void close() = 0;

  /**
   * @brief Virtual destructor for proper cleanup
   *
   * Ensures proper cleanup of derived classes when the interface is
   * destroyed through a base class pointer. Should automatically close
   * the session if it hasn't been closed already.
   */
  virtual ~ISession() = default;
};

} // namespace network
} // namespace aurora