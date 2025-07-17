#pragma once

#include <functional>

namespace aurora {
namespace utils {

/**
 * @brief Interface for thread pool implementations
 *
 * Defines the contract for thread pool services that manage a collection
 * of worker threads for executing tasks asynchronously. Implementations
 * should handle thread lifecycle management, task queuing, and proper
 * resource cleanup.
 */
class IThreadPool {
public:
  /**
   * @brief Virtual destructor for proper cleanup
   *
   * Ensures proper cleanup of derived classes when the interface is
   * destroyed through a base class pointer.
   */
  virtual ~IThreadPool() = default;

  /**
   * @brief Start the thread pool with the specified number of threads
   *
   * Initializes and starts the worker threads that will process tasks
   * from the queue. This method should be called before attempting to
   * enqueue any tasks.
   *
   * @param num_threads The number of worker threads to create and manage
   * @throws std::runtime_error if the thread pool cannot be started
   */
  virtual void start(size_t num_threads) = 0;

  /**
   * @brief Stop the thread pool
   *
   * Gracefully shuts down the thread pool, waiting for all currently
   * executing tasks to complete and properly terminating all worker threads.
   * After this method returns, no new tasks should be enqueued.
   */
  virtual void stop() = 0;

  /**
   * @brief Enqueue a task for asynchronous execution
   *
   * Adds a task to the thread pool's queue for execution by one of the
   * worker threads. Tasks are typically executed in FIFO order, but
   * specific implementations may prioritize tasks differently.
   *
   * @param task The function to be executed asynchronously
   * @throws std::runtime_error if the task cannot be enqueued (e.g., pool is
   * stopped)
   */
  virtual void enqueue_task(std::function<void()> task) = 0;
};

} // namespace utils
} // namespace aurora