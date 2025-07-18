#include "network/TCPServer.h"
#include "network/Session.h"

#include <iostream>
#include <stdexcept>

namespace aurora {
namespace network {

TCPServer::TCPServer(size_t io_threads)
    : acceptor_(io_context_), running_(false), num_io_threads_(io_threads) {}

TCPServer::~TCPServer() {
  if (running_) {
    stop();
  }
}

void TCPServer::start(uint16_t port) {
  if (running_) {
    throw std::runtime_error("TCP server is already running");
  }

  try {
    // Configure the acceptor
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port);
    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    acceptor_.bind(endpoint);
    acceptor_.listen();

    // Start accepting connections
    start_accept();

    // Start the IO threads
    running_ = true;
    for (size_t i = 0; i < num_io_threads_; ++i) {
      io_threads_.emplace_back([this]() { io_context_.run(); });
    }

    std::cout << "TCP server started on port " << port << std::endl;
  } catch (const std::exception &e) {
    throw std::runtime_error(std::string("Failed to start TCP server: ") +
                             e.what());
  }
}

void TCPServer::stop() {
  if (!running_) {
    return;
  }

  running_ = false;

  // Stop accepting new connections
  acceptor_.close();

  // Stop the IO context to cancel all pending operations
  io_context_.stop();

  // Wait for all IO threads to finish
  for (auto &thread : io_threads_) {
    if (thread.joinable()) {
      thread.join();
    }
  }
  io_threads_.clear();

  std::cout << "TCP server stopped" << std::endl;
}

void TCPServer::set_message_handler(const MessageHandler &handler) {
  message_handler_ = handler;
}

void TCPServer::start_accept() {
  auto new_session = std::make_shared<Session>(io_context_, message_handler_);
  acceptor_.async_accept(
      new_session->socket(),
      [this, new_session](const boost::system::error_code &error) {
        handle_accept(new_session, error);
      });
}

void TCPServer::handle_accept(std::shared_ptr<ISession> session,
                             const boost::system::error_code &error) {
  if (!error) {
    // Start the new session
    session->start();

    // Continue accepting new connections
    start_accept();
  } else {
    std::cerr << "Error accepting connection: " << error.message() << std::endl;
  }
}

} // namespace network
} // namespace aurora