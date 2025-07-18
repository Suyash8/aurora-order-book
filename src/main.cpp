#include "core/Configuration.h"
#include "network/TCPServer.h"

#include <iostream>
#include <memory>
#include <signal.h>

// Global flag for shutdown
volatile sig_atomic_t shutdown_requested = 0;

// Signal handler for graceful shutdown
void signal_handler(int signal) {
  std::cout << "Received signal " << signal << ", initiating shutdown..."
            << std::endl;
  shutdown_requested = 1;
}

int main(int argc, char *argv[]) {
  std::cout << "Aurora LOB initializing..." << std::endl;

  // Set up signal handling for graceful shutdown
  signal(SIGINT, signal_handler);
  signal(SIGTERM, signal_handler);

  try {
    // Load configuration
    auto &config = aurora::core::Configuration::instance();
    config.load_from_args(argc, argv);

    // Create and start the TCP server
    aurora::network::TCPServer server(config.get_io_threads());

    // Set up message handler
    server.set_message_handler(
        [](std::shared_ptr<aurora::network::ISession> session,
           const std::string &message) {
          std::cout << "Received message: " << message << std::endl;

          // Create a simple response for testing
          aurora::core::Response response;
          response.order_id =
              12345; // This would normally come from the parsed message
          response.status = aurora::core::OrderStatus::ACCEPTED;
          response.message = "Order accepted";

          // Send the response back to the client
          session->send_response(response);
        });

    // Start the server
    server.start(config.get_port());

    std::cout << "Aurora LOB started on port " << config.get_port()
              << std::endl;
    std::cout << "Press Ctrl+C to stop the server" << std::endl;

    // Wait for shutdown signal
    while (!shutdown_requested) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Graceful shutdown
    std::cout << "Shutting down Aurora LOB..." << std::endl;
    server.stop();

  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  std::cout << "Aurora LOB shutdown complete" << std::endl;
  return 0;
}