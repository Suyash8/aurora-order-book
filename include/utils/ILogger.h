#pragma once

#include <string_view>

namespace aurora {
namespace utils {

/**
 * @brief Interface for logging services
 *
 * Defines the contract for logging implementations that handle
 * application messages at different severity levels. Implementations
 * should manage log formatting, storage, and any configured outputs
 * such as console, files, or external logging services.
 */
class ILogger {
public:
  /**
   * @brief Virtual destructor for proper cleanup
   *
   * Ensures proper cleanup of derived classes when the interface is
   * destroyed through a base class pointer.
   */
  virtual ~ILogger() = default;

  /**
   * @brief Log an informational message
   *
   * Records a message at the INFO level, typically used for general
   * operational information about the application's normal functioning.
   *
   * @param message The information message to log
   */
  virtual void info(std::string_view message) = 0;

  /**
   * @brief Log a warning message
   *
   * Records a message at the WARNING level, typically used for
   * non-critical issues that might require attention but don't
   * affect the core functionality of the application.
   *
   * @param message The warning message to log
   */
  virtual void warn(std::string_view message) = 0;

  /**
   * @brief Log an error message
   *
   * Records a message at the ERROR level, typically used for
   * significant problems that affect the application's functionality
   * but don't require immediate termination.
   *
   * @param message The error message to log
   */
  virtual void error(std::string_view message) = 0;
};

} // namespace utils
} // namespace aurora