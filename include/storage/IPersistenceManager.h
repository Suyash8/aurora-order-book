#pragma once

namespace aurora {
namespace core {
class IMatchingEngine;
}

namespace storage {

/**
 * @brief Interface for persistence management services
 *
 * Defines the contract for components that handle the persistence and
 * recovery of trading system state. Implementations should manage the
 * creation of system snapshots and restoration of system state from
 * previously saved snapshots to ensure data durability across system
 * restarts or failures.
 */
class IPersistenceManager {
public:
  /**
   * @brief Virtual destructor for proper cleanup
   *
   * Ensures proper cleanup of derived classes when the interface is
   * destroyed through a base class pointer.
   */
  virtual ~IPersistenceManager() = default;

  /**
   * @brief Create a snapshot of the current matching engine state
   *
   * Captures the current state of the matching engine including all active
   * orders, order books, and other relevant trading data, and persists it
   * to a storage medium. This snapshot can later be used to restore the
   * system to this point in time.
   *
   * @param engine The matching engine whose state should be captured
   * @throws std::runtime_error if the snapshot creation fails
   */
  virtual void create_snapshot(const core::IMatchingEngine &engine) = 0;

  /**
   * @brief Restore matching engine state from a previously saved snapshot
   *
   * Loads a previously created snapshot from storage and uses it to
   * reconstruct the matching engine state, including order books and
   * all active orders.
   *
   * @param engine The matching engine to restore state into
   * @return bool True if restoration was successful, false otherwise
   * @throws std::runtime_error if the restoration process encounters a critical
   * error
   */
  virtual bool restore_from_snapshot(core::IMatchingEngine &engine) = 0;
};

} // namespace storage
} // namespace aurora