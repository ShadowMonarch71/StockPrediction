/*
 * backtester.h
 *
 * Defines the backtesting engine that simulates trading based on signals.
 * The backtester executes trades, tracks portfolio value over time, and records
 * all completed trades with their profit/loss.
 *
 * Key features:
 *  - Long-only strategy execution (no shorting)
 *  - All-in position sizing (invests entire portfolio on each trade)
 *  - Realistic trading costs: slippage and fixed transaction fees
 *  - Executes trades at next bar's open price after signal change
 *  - Tracks equity curve (portfolio value at each bar)
 *  - Records detailed trade history (entry/exit dates, prices, P&L)
 *
 * Trading mechanics:
 *  - Signal 0→1: Enter long position at next open (buy)
 *  - Signal 1→0: Exit long position at next open (sell)
 *  - Signal stays same: Hold current position (no action)
 *  - End of data: Force-close any open position
 *
 * Usage example:
 *  Backtester bt(0.0005, 0.0);  // 0.05% slippage, no fixed cost
 *  bt.run(bars, signals);
 *  auto equity = bt.equity();    // Get portfolio value over time
 *  auto trades = bt.trades();    // Get all completed trades
 */

#pragma once
#include "csv_loader.h"
#include <vector>

namespace sp {

// Represents a single completed trade with entry/exit details and profit/loss
struct Trade { 
    std::string entry_date;   // Date when position was opened (e.g., "2025-10-15")
    std::string exit_date;    // Date when position was closed
    double entry_price;       // Execution price when buying (includes slippage)
    double exit_price;        // Execution price when selling (includes slippage)
    int size;                 // Number of shares traded
    double pnl;              // Profit and Loss: (exit proceeds - entry cost)
};

// Backtesting engine that simulates trading and tracks performance
class Backtester {
public:
    // Constructor initializes trading cost parameters
    // slippage: Fractional price impact (e.g., 0.001 = 0.1% worse price)
    // cost: Fixed transaction fee per trade (e.g., 0.5 = $0.50 per trade)
    Backtester(double slippage = 0.0, double cost = 0.0);
    
    // Execute backtest by processing bars and signals
    // bars: Historical OHLCV price data
    // signals: Trading signals aligned with bars (1=long, 0=flat)
    // Populates equity_ and trades_ vectors during execution
    void run(const std::vector<Bar>& bars, const std::vector<int>& signals);
    
    // Get the equity curve (portfolio value at each bar)
    // Returns vector where equity[i] = total portfolio value at bar i
    const std::vector<double>& equity() const { return equity_; }
    
    // Get all completed trades
    // Returns vector of Trade structs with entry/exit details and P&L
    const std::vector<Trade>& trades() const { return trades_; }
    
private:
    double slippage_;              // Fractional slippage applied to entry/exit prices
    double cost_;                  // Fixed cost subtracted from each trade's proceeds
    std::vector<double> equity_;   // Portfolio value at each bar (same length as input bars)
    std::vector<Trade> trades_;    // List of all completed trades
};

} // namespace sp
