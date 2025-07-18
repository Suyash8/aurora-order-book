#include "core/Configuration.h"
#include "yaml-cpp/exceptions.h"
#include "yaml-cpp/node/parse.h"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <fstream>
#include <iostream>
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

void Configuration::load_from_file(const std::string &filename) {
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