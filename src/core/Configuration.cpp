#include "core/Configuration.h"
#include "yaml-cpp/exceptions.h"
#include "yaml-cpp/node/parse.h"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cxxopts.hpp>
#include <exception>
#include <fstream>
#include <iostream>
#include <mutex>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>
#include <yaml-cpp/yaml.h>

namespace aurora {
namespace core {

Configuration::Configuration()
    : worker_threads_(std::thread::hardware_concurrency()) {}

Configuration &Configuration::instance() {
  static Configuration instance;
  return instance;
}

bool has_extensions(const std::string &filename,
                    const std::vector<std::string> &extensions) {
  size_t dot_pos = filename.rfind('.');
  if (dot_pos == std::string::npos || dot_pos == 0) {
    return false;
  }

  std::string extension = filename.substr(dot_pos + 1);
  std::transform(extension.begin(), extension.end(), extension.begin(),
                 [](unsigned char c) { return std::tolower(c); });

  for (const auto &find_extension : extensions) {
    if (extension == find_extension) {
      return true;
    }
  }

  return false;
}

void Configuration::load_from_file_(const std::string &filename) {
  std::vector<std::string> yaml_extensions{"yml", "yaml"};
  std::vector<std::string> json_extensions{"json"};

  if (has_extensions(filename, yaml_extensions)) {
    load_from_yaml(filename);
  } else if (has_extensions(filename, json_extensions)) {
    load_from_json(filename);
  } else {
    std::vector<std::string> all_extensions = yaml_extensions;
    all_extensions.insert(all_extensions.end(), json_extensions.begin(),
                          json_extensions.end());

    std::string extensions_str;
    for (size_t i = 0; i < all_extensions.size(); ++i) {
      if (i > 0)
        extensions_str += ", ";
      extensions_str += all_extensions[i];
    }

    throw std::runtime_error("File not any of accepted extensions: " +
                             extensions_str);
  }
}

void Configuration::load_from_file(const std::string &filename) {
  std::unique_lock<std::shared_mutex> lock(config_mutex_);
  load_from_file_(filename);
}

void Configuration::load_from_json(const std::string &filename) {
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

void Configuration::load_from_yaml(const std::string &filename) {
  try {
    YAML::Node config_yaml = YAML::LoadFile(filename);

    // Parse network configuration
    if (config_yaml["network"]) {
      const auto &network = config_yaml["network"];
      if (network["port"]) {
        port_ = network["port"].as<uint16_t>();
      }
      if (network["io_threads"]) {
        io_threads_ = network["io_threads"].as<size_t>();
      }
    }

    // Parse processing configuration
    if (config_yaml["processing"]) {
      const auto &processing = config_yaml["processing"];
      if (processing["worker_threads"]) {
        worker_threads_ = processing["worker_threads"].as<size_t>();
      }
    }

    // Parse persistence configuration
    if (config_yaml["persistence"]) {
      const auto &persistence = config_yaml["persistence"];
      if (persistence["enabled"]) {
        enable_persistence_ = persistence["enabled"].as<bool>();
      }
    }

    // Parse instruments
    if (config_yaml["instruments"] && config_yaml["instruments"].IsSequence()) {
      instruments_.clear();
      for (const auto &instrument_node : config_yaml["instruments"]) {
        instruments_.push_back(instrument_node.as<std::string>());
      }
    }

    config_file_ = filename;
    std::cout << "Configuration loaded from " << filename << std::endl;
  } catch (const YAML::Exception &e) {
    throw std::runtime_error(std::string("Error loading configuration: ") +
                             e.what());
  }
}

void Configuration::load_from_args(int argc, char *argv[]) {
  try {
    std::unique_lock<std::shared_mutex> lock(config_mutex_);

    cxxopts::Options options(
        "aurora-order-book",
        "Aurora Order Book - High-performance matching engine");

    options.add_options() //
        ("p,port", "TCP server port for client connections",
         cxxopts::value<uint16_t>()) //
        ("i,io-threads", "Number of I/O threads for network operations",
         cxxopts::value<size_t>()) //
        ("w,worker-threads", "Number of worker threads for order processing",
         cxxopts::value<size_t>())                           //
        ("enable-persistence", "Enable state persistence")   //
        ("disable-persistence", "Disable state persistence") //
        ("c,config", "Path to configuration file (JSON or YAML)",
         cxxopts::value<std::string>()) //
        ("instruments", "Comma-separated list of supported instruments",
         cxxopts::value<std::string>()) //
        ("h,help", "Print usage information");

    auto result = options.parse(argc, argv);

    if (result.count("help")) {
      std::cout << options.help() << std::endl;
      return;
    }

    if (result.count("config")) {
      load_from_file_(result["config"].as<std::string>());
    }

    // Override with command line values if specified
    if (result.count("port")) {
      port_ = result["port"].as<uint16_t>();
    }

    if (result.count("io-threads")) {
      io_threads_ = result["io-threads"].as<size_t>();
    }

    if (result.count("worker-threads")) {
      worker_threads_ = result["worker-threads"].as<size_t>();
    }

    if (result.count("enable-persistence")) {
      enable_persistence_ = true;
    }

    if (result.count("disable-persistence")) {
      enable_persistence_ = false;
    }

    if (result.count("instruments")) {
      std::string instruments_str = result["instruments"].as<std::string>();
      instruments_.clear();

      // Parse comma-separated instrument list
      size_t pos = 0;
      while ((pos = instruments_str.find(',')) != std::string::npos) {
        instruments_.push_back(instruments_str.substr(0, pos));
        instruments_str.erase(0, pos + 1);
      }

      // Add the last instrument
      if (!instruments_str.empty()) {
        instruments_.push_back(instruments_str);
      }
    }

    if (!validate()) {
      std::cerr << "Configuration validation failed. "
                << "Please correct the configuration issues and restart the "
                   "applicaiton."
                << std::endl;
      exit(EXIT_FAILURE);
    }
  } catch (const cxxopts::exceptions::exception &e) {
    throw std::runtime_error(
        std::string("Error parsing command line options: ") + e.what());
  }
}

bool Configuration::validate() const {
  bool is_valid = true;

  if (port_ <= 1024 || port_ > 65535) {
    std::cerr
        << "Configuration error: Port number must be between 1025 and 65535"
        << std::endl;
    is_valid = false;
  }

  if (io_threads_ == 0) {
    std::cerr << "Configuration error: At least one IO thread is required"
              << std::endl;
    is_valid = false;
  } else if (io_threads_ > 64) {
    std::cerr << "Configuration warning: Very high IO thread count ("
              << io_threads_ << ") may impact performance" << std::endl;
  }

  if (worker_threads_ == 0) {
    std::cerr << "Configuration warning: No worker threads specified, using "
                 "single-threaded processing"
              << std::endl;
  } else if (worker_threads_ > std::thread::hardware_concurrency() * 2) {
    std::cerr << "Configuration warning: Worker thread count ("
              << worker_threads_
              << ") significantly exceeds hardware concurrency ("
              << std::thread::hardware_concurrency() << ")" << std::endl;
  }

  if (instruments_.empty()) {
    std::cerr << "Configuration error: No instruments specified" << std::endl;
    is_valid = false;
  }

  if (enable_persistence_) {
    std::string persistence_dir = "./data";

    std::error_code ec;
    if (!std::filesystem::exists(persistence_dir, ec) || ec) {
      std::cerr << "Configuration error: Persistence directory does not exist: "
                << persistence_dir << std::endl;
      is_valid = false;
    } else if (!std::filesystem::is_directory(persistence_dir, ec) || ec) {
      std::cerr << "Configuration error: Persistence path is not a directory: "
                << persistence_dir << std::endl;
      is_valid = false;
    } else {
      // Test if directory is writable by creating a test file
      std::string test_file = persistence_dir + "/write_test";
      std::ofstream test_stream(test_file);
      if (!test_stream) {
        std::cerr
            << "Configuration error: Persistence directory is not writable: "
            << persistence_dir << std::endl;
        is_valid = false;
      } else {
        // Clean up test file
        test_stream.close();
        std::filesystem::remove(test_file, ec);
      }
    }
  }

  if (!config_file_.empty()) {
    if (!std::filesystem::exists(config_file_)) {
      std::cerr << "Configuration warning: Config file no longer exists: "
                << config_file_ << std::endl;
    }
  }

  return is_valid;
}

} // namespace core
} // namespace aurora