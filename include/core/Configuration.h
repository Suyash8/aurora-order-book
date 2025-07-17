#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <thread>

namespace aurora {
namespace core {

/**
 * @brief Singleton class to manage configuration settings.
 *
 * Provides methods to load configuration from a file or command line arguments,
 * and access various configuration parameters such as port, thread counts, and
 * persistence settings.
 */
class Configuration {
public:
  /**
   * @brief Get the singleton instance of the Configuration class.
   *
   * @return Configuration& Reference to the singleton instance.
   */
  static Configuration &instance();
  /**
   * @brief Load configuration from a file.
   *
   * Reads configuration settings from the specified file and updates the
   * internal state of the Configuration instance.
   *
   * @param filename Path to the configuration file.
   */
  void load_from_file(const std::string &filename);
  /**
   * @brief Load configuration from command line arguments.
   *
   * Parses command line arguments to set configuration parameters such as port,
   * thread counts, and persistence settings.
   *
   * @param argc Argument count.
   * @param argv Argument vector.
   */
  void load_from_args(int argc, char *argv[]);

  /**
   * @brief Get the port number for the application.
   *
   * @return uint16_t Port number.
   */
  uint16_t get_port() const;
  /**
   * @brief Get the number of I/O threads.
   *
   * @return size_t Number of I/O threads.
   */
  size_t get_io_threads() const;
  /**
   * @brief Get the number of worker threads.
   *
   * @return size_t Number of worker threads.
   */
  size_t get_worker_threads() const;
  /**
   * @brief Check if persistence is enabled.
   *
   * @return bool True if persistence is enabled, false otherwise.
   */
  bool is_persistence_enabled() const;

private:
  Configuration() = default;

  /** @brief Port number for the application. */
  uint16_t port_ = 8080;
  /** @brief Number of I/O threads for handling network operations. */
  size_t io_threads_ = 4;
  /** @brief Number of worker threads for processing orders. */
  size_t worker_threads_ = std::thread::hardware_concurrency();
  /** @brief Flag indicating whether persistence is enabled. */
  bool persistence_enabled_ = false;
  /** @brief Path to the configuration file. */
  std::string config_file_;
};

} // namespace core
} // namespace aurora