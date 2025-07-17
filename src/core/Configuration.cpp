#include "Configuration.h"

#include <cstdint>

using aurora::core::Configuration;

Configuration &Configuration::instance() {
  static Configuration instance;
  return instance;
}

uint16_t Configuration::get_port() const { return port_; }
size_t Configuration::get_io_threads() const { return io_threads_; }
size_t Configuration::get_worker_threads() const { return worker_threads_; }
bool Configuration::is_persistence_enabled() const {
  return persistence_enabled_;
}