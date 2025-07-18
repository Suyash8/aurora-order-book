#pragma once

#include "core/DataTypes.h"

#include <string>
#include <vector>

namespace aurora {
namespace core {

/**
 * @brief Interface for market statistics calculation and reporting
 *
 * Defines the contract for components that track and calculate market
 * statistics such as best bid/ask prices, spread, market depth, and trading
 * volume. Implementations should efficiently update statistics as market
 * conditions change and provide access to current market metrics.
 */
class IStatisticsEngine {
public:
  /**
   * @brief Virtual destructor for proper cleanup
   */
  virtual ~IStatisticsEngine() = default;

  /**
   * @brief Update statistics based on a new trade
   *
   * Processes a newly executed trade to update relevant market statistics
   * such as volume, trade count, and price information.
   *
   * @param trade The trade that was just executed
   */
  virtual void update_on_trade(const Trade &trade) = 0;

  /**
   * @brief Update statistics based on order book changes
   *
   * Updates market statistics when the order book state changes, such as
   * when orders are added, cancelled, or modified.
   *
   * @param instrument_id The identifier for the instrument whose book changed
   * @param snapshot The current snapshot of the order book
   */
  virtual void update_on_book_change(const std::string &instrument_id,
                                     const BookSnapshot &snapshot) = 0;

  /**
   * @brief Get current market statistics for an instrument
   *
   * Retrieves the latest calculated market statistics for the specified
   * financial instrument.
   *
   * @param instrument_id The identifier for the instrument
   * @return MarketStats The current market statistics for the instrument
   */
  virtual MarketStats
  get_statistics(const std::string &instrument_id) const = 0;

  /**
   * @brief Get statistics for all active instruments
   *
   * Retrieves the latest calculated market statistics for all instruments
   * currently being tracked by the statistics engine.
   *
   * @return std::vector<MarketStats> Statistics for all active instruments
   */
  virtual std::vector<MarketStats> get_all_statistics() const = 0;
};

} // namespace core
} // namespace aurora