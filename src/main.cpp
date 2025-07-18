#include "core/Configuration.h"
#include "network/TCPServer.h"

#include <atomic>
#include <chrono>
#include <csignal>
#include <iostream>
#include <memory>
#include <signal.h>
#include <termios.h>
#include <thread>
#include <unistd.h>

// Global signal flags
std::atomic<bool> g_shutdown_requested{false};
std::atomic<bool> g_reload_config_requested{false};

// Signal handler for graceful shutdown
void signal_handler(int signum) {
  if (signum == SIGINT || signum == SIGTERM) {
    g_shutdown_requested = true;
  } else if (signum == SIGHUP) {
    g_reload_config_requested = true;
  }
}

// Raw terminal helper RAII
#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
struct TerminalManager {
  termios original_termios;
  bool is_valid = false;

  TerminalManager() {
    if (tcgetattr(STDIN_FILENO, &original_termios) == 0) {
      termios raw = original_termios;
      raw.c_lflag &= ~(ICANON | ECHO);
      tcsetattr(STDIN_FILENO, TCSANOW, &raw);
      is_valid = true;
    }
  }

  ~TerminalManager() {
    if (is_valid) {
      tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
    }
  }
};
#endif

// Keyboard listener thread function
void keyboard_listener_thread() {
#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
  TerminalManager tm;
  if (!tm.is_valid)
    return;

  char c;
  while (!g_shutdown_requested) {
    ssize_t bytes_read = read(STDIN_FILENO, &c, 1);
    if (bytes_read > 0) {
      switch (c) {
      case 3: // Ctrl+C
      case 4: // Ctrl+D
        g_shutdown_requested = true;
        break;
      case 18: // Ctrl+R (Reload)
        g_reload_config_requested = true;
        break;
      }
    } else if (bytes_read == 0) {
      g_shutdown_requested = true;
    } else if (errno == EINTR) {
      continue;
    } else {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }
#else
  std::cout
      << "Interactive keyboard shortcuts are not supported on this platform."
      << std::endl;
#endif
}

int main(int argc, char *argv[]) {
  std::cout << "Aurora LOB initializing..." << std::endl;

  // Set up signal handling
  struct sigaction action;
  action.sa_handler = signal_handler;
  sigemptyset(&action.sa_mask);
  action.sa_flags = 0;

  sigaction(SIGINT, &action, NULL);
  sigaction(SIGTERM, &action, NULL);
  sigaction(SIGHUP, &action, NULL);

  std::thread keyboard_thread(keyboard_listener_thread);

  std::cout << "\nInteractive Controls:\n"
               "  Ctrl+C / Ctrl+D: Shutdown Gracefully\n"
               "  Ctrl+R:          Reload Configuration\n\n";

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
    while (!g_shutdown_requested) {
      if (g_reload_config_requested) {
        g_reload_config_requested = false;
        std::cout << "SIGHUP received, reloading configuration..." << std::endl;
        config.reload();
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Graceful shutdown
    std::cout << "Shutting down Aurora LOB..." << std::endl;
    server.stop();

    if (keyboard_thread.joinable()) {
#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
      pthread_kill(keyboard_thread.native_handle(), SIGCONT);
#endif
      keyboard_thread.join();
    }

  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  std::cout << "Aurora LOB shutdown complete" << std::endl;
  return 0;
}