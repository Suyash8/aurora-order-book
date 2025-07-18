#include "core/Configuration.h"

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <thread>

namespace aurora {
namespace core {

Configuration::Configuration()
    : worker_threads_(std::thread::hardware_concurrency()) {}

Configuration &Configuration::instance() {
  static Configuration instance;
  return instance;
}

void Configuration::load_from_file(const std::string &filename) {
  try {
    std::ifstream file(filename);
    if (!file.is_open()) {
      throw std::runtime_error("Could not open configuration file: " +
                               filename);
    }

    nlohmann::json config_json;
    file >> config_json;

    // Parse network configuration
    if (config_json.contains("network")) {
      const auto &network = config_json["network"];
      if (network.contains("port")) {
        port_ = network["port"];
      }
      if (network.contains("io_threads")) {
        io_threads_ = network["io_threads"];
      }
    }

    // Parse processing configuration
    if (config_json.contains("processing")) {
      const auto &processing = config_json["processing"];
      if (processing.contains("worker_threads")) {
        worker_threads_ = processing["worker_threads"];
      }
    }

    // Parse persistence configuration
    if (config_json.contains("persistence")) {
      const auto &persistence = config_json["persistence"];
      if (persistence.contains("enabled")) {
        enable_persistence_ = persistence["enabled"];
      }
    }

    // Parse instruments
    if (config_json.contains("instruments") &&
        config_json["instruments"].is_array()) {
      instruments_.clear();
      for (const auto &instrument : config_json["instruments"]) {
        instruments_.push_back(instrument);
      }
    }

    config_file_ = filename;
    std::cout << "Configuration loaded from " << filename << std::endl;
  } catch (const std::exception &e) {
    throw std::runtime_error(std::string("Error loading configuration: ") +
                             e.what());
  }
}

void Configuration::load_from_args(int argc, char *argv[]) {
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];

    if (arg == "--port" && i + 1 < argc) {
      port_ = static_cast<uint16_t>(std::stoi(argv[++i]));
    } else if (arg == "--io-threads" && i + 1 < argc) {
      io_threads_ = static_cast<size_t>(std::stoi(argv[++i]));
    } else if (arg == "--worker-threads" && i + 1 < argc) {
      worker_threads_ = static_cast<size_t>(std::stoi(argv[++i]));
    } else if (arg == "--enable-persistence") {
      enable_persistence_ = true;
    } else if (arg == "--disable-persistence") {
      enable_persistence_ = false;
    } else if (arg == "--config" && i + 1 < argc) {
      load_from_file(argv[++i]);
    }
  }
}

} // namespace core
} // namespace aurora