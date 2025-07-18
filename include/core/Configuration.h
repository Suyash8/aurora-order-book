#pragma once

#include "core/IConfigurationObserver.hpp"
#include <cstdint>
#include <shared_mutex>
#include <string>
#include <vector>

namespace aurora {
namespace core {

/**
 * @brief Configuration management for the Aurora LOB system
 *
 * This class implements a singleton pattern for global access to configuration
 * settings. It supports loading from JSON/YAML files and command-line
 * arguments.
 */
class Configuration {
public:
  /**
   * @brief Get the singleton instance of the Configuration class
   *
   * @return Reference to the singleton Configuration instance
   */
  static Configuration &instance();

  /**
   * @brief Load configuration from a file
   *
   * @param filename Path to the configuration file (JSON or YAML)
   * @throws std::runtime_error if the file cannot be loaded or parsed
   */
  void load_from_file(const std::string &filename);

  /**
   * @brief Load configuration from command-line arguments
   *
   * @param argc Argument count from main
   * @param argv Argument values from main
   */
  void load_from_args(int argc, char *argv[]);

  /**
   * @brief Get the TCP server port
   *
   * @return The configured port number
   */
  uint16_t get_port() const {
    std::shared_lock<std::shared_mutex> lock(config_mutex_);
    return port_;
  }

  /**
   * @brief Get the number of I/O threads
   *
   * @return The configured number of I/O threads
   */
  size_t get_io_threads() const {
    std::shared_lock<std::shared_mutex> lock(config_mutex_);
    return io_threads_;
  }

  /**
   * @brief Get the number of worker threads
   *
   * @return The configured number of worker threads
   */
  size_t get_worker_threads() const {
    std::shared_lock<std::shared_mutex> lock(config_mutex_);
    return worker_threads_;
  }

  /**
   * @brief Check if persistence is enabled
   *
   * @return True if persistence is enabled, false otherwise
   */
  bool is_persistence_enabled() const {
    std::shared_lock<std::shared_mutex> lock(config_mutex_);
    return enable_persistence_;
  }

  /**
   * @brief Get the list of configured instruments
   *
   * @return Vector of instrument IDs
   */
  const std::vector<std::string> &get_instruments() const {
    std::shared_lock<std::shared_mutex> lock(config_mutex_);
    return instruments_;
  }

  void reload();
  void register_observer(IConfigurationObserver *observer);

private:
  /**
   * @brief Private constructor for singleton pattern
   */
  Configuration();

  /**
   * @brief Deleted copy constructor
   */
  Configuration(const Configuration &) = delete;

  /**
   * @brief Deleted assignment operator
   */
  Configuration &operator=(const Configuration &) = delete;

  /**
   * @brief Helper method to load configuration from a file with appropriate
   * format
   *
   * Determines the file format based on extension and delegates to the
   * appropriate specialized loading method (JSON or YAML). This method assumes
   * the config_mutex_ is already locked by the caller.
   *
   * @param filename Path to the configuration file
   * @throws std::runtime_error if the file has an unsupported extension or
   * cannot be loaded
   */
  void load_from_file_(const std::string &filename);

  /**
   * @brief Load configuration from a JSON file
   *
   * Parses the specified JSON file and updates configuration settings
   * based on its contents. Supports all configurable parameters including
   * network settings, threading options, and instrument definitions.
   *
   * @param filename Path to the JSON configuration file
   * @throws std::runtime_error if the file cannot be read or contains invalid
   * JSON
   */
  void load_from_json(const std::string &filename);

  /**
   * @brief Load configuration from a YAML file
   *
   * Parses the specified YAML file and updates configuration settings
   * based on its contents. Supports all configurable parameters including
   * network settings, threading options, and instrument definitions.
   *
   * @param filename Path to the YAML configuration file
   * @throws std::runtime_error if the file cannot be read or contains invalid
   * YAML
   */
  void load_from_yaml(const std::string &filename);

  /**
   * @brief Validate the current configuration
   *
   * Performs a comprehensive validation of all configuration parameters to
   * ensure they meet system requirements and operational constraints. Includes
   * checks for:
   * - Port number is in valid range (1025-65535, avoiding privileged ports)
   * - Thread counts are reasonable (non-zero for worker threads if used)
   * - Persistence directory exists and is writable if persistence is enabled
   * - At least one instrument is configured if required by the application
   * - IO thread count is appropriate for the expected load
   * - Any configured file paths exist and have appropriate permissions
   *
   * @return true if all validation checks pass, false otherwise
   * @note Failed validations are logged with specific error messages
   */
  bool validate() const;

  /** @brief TCP server port number for client connections */
  uint16_t port_ = 8080;

  /** @brief Number of I/O threads for handling network operations */
  size_t io_threads_ = 4;

  /** @brief Number of worker threads for processing orders and trades */
  size_t worker_threads_ = 0; // Default to hardware concurrency in constructor

  /** @brief Flag indicating whether state persistence is enabled */
  bool enable_persistence_ = false;

  /** @brief Path to the configuration file that was loaded, if any */
  std::string config_file_;

  /** @brief List of financial instruments supported by the system */
  std::vector<std::string> instruments_;

  /** @brief Mutex for thread-safe access to configuration data
   *
   * Shared mutex allows multiple concurrent readers but exclusive writers,
   * optimizing for the common case of configuration being read frequently
   * but updated rarely.
   */
  mutable std::shared_mutex config_mutex_;

  std::vector<IConfigurationObserver *> observers_;
  std::string config_file_path_;
};

} // namespace core
} // namespace aurora