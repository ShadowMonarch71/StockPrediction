/*
 * strategy.h
 *
 * Defines trading strategy interfaces for generating buy/sell signals from market data.
 * Strategies analyze price bars and produce integer signals that indicate when to
 * enter or exit positions.
 *
 * Architecture:
 *  - Strategy: Abstract base class defining the interface all strategies must implement
 *  - RuleBasedStrategy: Concrete implementation using indicator crossover rules
 *
 * Signal convention:
 *  - 1 = Long position (buy/hold the asset)
 *  - 0 = Flat position (no position, cash only)
 *
 * The generated signals are consumed by the Backtester which executes trades based
 * on signal transitions (0→1 = enter, 1→0 = exit).
 *
 * Usage example:
 *  auto ema = std::make_shared<EMAIndicator>(20);
 *  auto sma = std::make_shared<SMAIndicator>(50);
 *  RuleBasedStrategy strategy(ema, sma);
 *  auto signals = strategy.generate_signals(bars);  // Returns [0,0,1,1,0,1,...]
 */

#pragma once
#include "csv_loader.h"
#include <memory>
#include <vector>

namespace sp {

// Abstract base class for all trading strategies.
// Defines the interface that concrete strategy implementations must follow.
class Strategy {
public:
    // Virtual destructor ensures proper cleanup of derived classes
    virtual ~Strategy() = default;
    
    // Generate trading signals from historical price data.
    // Returns a vector of integers (1=long, 0=flat) aligned with input bars.
    virtual std::vector<int> generate_signals(const std::vector<Bar>& bars) = 0;
};

// Rule-based trading strategy using indicator crossover signals.
// Compares two indicators (typically fast vs slow) to generate buy/sell signals.
// The trading rule: emit signal=1 when fast indicator > slow indicator, else 0.
class RuleBasedStrategy : public Strategy {
public:
    // Constructor takes two indicators for comparison
    // shortEma: Fast-moving indicator (e.g., EMA with shorter period)
    // longSma: Slow-moving indicator (e.g., SMA with longer period)
    RuleBasedStrategy(std::shared_ptr<class Indicator> shortEma, std::shared_ptr<class Indicator> longSma);
    
    // Generate signals by comparing the two indicators at each bar
    // Returns 1 when shortEma > longSma (bullish crossover), 0 otherwise
    std::vector<int> generate_signals(const std::vector<Bar>& bars) override;
    
private:
    std::shared_ptr<Indicator> shortEma_;  // Fast indicator for detecting quick price changes
    std::shared_ptr<Indicator> longSma_;   // Slow indicator for identifying longer-term trends
};

} // namespace sp
