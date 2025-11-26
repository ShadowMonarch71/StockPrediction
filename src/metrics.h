/*
 * metrics.h
 *
 * Defines performance metrics for evaluating backtest results.
 * Provides simple statistics to assess trading strategy effectiveness.
 *
 * Key metrics:
 *  - Final equity: Total portfolio value at end of backtest
 *  - Max drawdown: Worst peak-to-trough decline (risk measure)
 *  - Trade count: Total number of completed trades
 *  - Win count: Number of profitable trades
 *
 * These metrics help answer:
 *  - Was the strategy profitable overall? (final_equity)
 *  - What was the worst loss from a peak? (max_drawdown)
 *  - How many trades were executed? (trades)
 *  - What percentage won? (wins / trades)
 *
 * Usage example:
 *  auto m = compute_metrics(bt.equity(), bt.trades());
 *  std::cout << "Return: " << (m.final_equity - 1.0) * 100 << "%\n";
 *  std::cout << "Max DD: " << m.max_drawdown * 100 << "%\n";
 *  std::cout << "Win rate: " << (m.wins * 100.0 / m.trades) << "%\n";
 */

#pragma once
#include <vector>
#include "backtester.h"

namespace sp {

// Performance metrics calculated from backtest results
struct Metrics {
    double final_equity = 1.0;   // Final portfolio value (1.0 = starting capital, 1.5 = 50% gain)
    double max_drawdown = 0.0;   // Maximum peak-to-trough decline (0.25 = 25% drawdown)
    int trades = 0;              // Total number of completed trades
    int wins = 0;                // Number of trades with positive P&L
};

// Compute performance metrics from equity curve and trade history
// equity: Portfolio value at each bar from backtester
// trades: List of completed trades from backtester
// Returns: Metrics struct with calculated statistics
Metrics compute_metrics(const std::vector<double>& equity, const std::vector<Trade>& trades);

} // namespace sp
