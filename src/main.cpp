/*
 * main.cpp
 *
 * Entry point for the tiny backtesting CLI application. This file wires the
 * components together: CSV loader -> indicators -> strategy -> backtester ->
 * metrics reporter. The code is intentionally minimal so learners can see the
 * full data flow in one place.
 *
 * CLI contract:
 *  - Usage: strategy <csv-path> [sma_period=50] [ema_period=20]
 *  - csv-path: path to an OHLCV CSV file with header (Date,Open,High,Low,Close,Volume)
 *  - sma_period, ema_period: optional integer indicator periods used by the
 *    rule-based strategy.
 *
 * High-level behavior:
 *  1. Load bars with `CSVLoader`.
 *  2. Construct indicator instances (EMA short, SMA long) with the provided
 *     periods.
 *  3. Create `RuleBasedStrategy` that compares the fast and slow indicators.
 *  4. Run the `Backtester` to simulate trades and produce an equity curve +
 *     trade list.
 *  5. Compute and print simple metrics and the first few trades.
 *
 * Extension points and suggestions:
 *  - To add logging or CSV exports, extend `Backtester` to write trades and
 *    equity to files and add CLI switches to toggle output paths.
 *  - To add parameterization, integrate a small options parser (e.g., getopt
 *    or a header-only CLI library) and expose more backtest parameters.
 */

#include "csv_loader.h"
#include "indicator.h"
#include "strategy.h"
#include "backtester.h"      
#include "metrics.h"
#include <iostream>
#include <memory>

using namespace sp;

int main(int argc, char *argv[]) {

    // Basic usage check: require at least CSV path
    if (argc < 2) {
        std::cerr << "Usage: strategy <csv-path> [sma_period=50] [ema_period=20]\n";
        return 1;
    }

    // Parse CLI args with sensible defaults. We keep parsing trivial to avoid
    // bringing in a dependency; for larger projects prefer a dedicated parser.
    std::string path = argv[1];
    int sma_p = argc > 2 ? std::stoi(argv[2]) : 50;
    int ema_p = argc > 3 ? std::stoi(argv[3]) : 20;

    try {
        // Load historical OHLCV data from the CSV file into a vector of Bar objects
        // Each Bar contains date, open, high, low, close, and volume
        CSVLoader loader(path);
        auto bars = loader.load();

        // If the dataset is empty, print an error and exit
        if (bars.empty()) {
            std::cerr << "No bars loaded\n";
            return 1;
        }

        // Create indicator objects for technical analysis
        // EMAIndicator tracks short-term trends (fast moving average)
        // SMAIndicator tracks long-term trends (slow moving average)
        auto shortEma = std::make_shared<EMAIndicator>(ema_p);
        auto longSma  = std::make_shared<SMAIndicator>(sma_p);

        // Create a rule-based strategy that uses the two indicators
        // The rule: when EMA > SMA → generate a buy signal (1), otherwise 0
        RuleBasedStrategy strat(shortEma, longSma);

        // Generate a list of signals corresponding to each data point
        // Signals are derived by comparing indicator values
        auto signals = strat.generate_signals(bars);

        // Debug: print signals to see the pattern
        std::cout << "Signal pattern:\n";
        for (size_t i = 0; i < signals.size(); ++i) {
            std::cout << bars[i].date << " close=" << bars[i].close 
                      << " signal=" << signals[i] << "\n";
        }
        std::cout << "\n";

        // Initialize the backtester
        // Parameters: slippage = 0.0005 (0.05%), cost = 0.0
        // The backtester simulates a trading session:
        // - Opens a position when signal changes from 0 to 1
        // - Closes it when signal changes from 1 to 0
        // - Updates cash and equity after each trade
        Backtester bt(0.0005, 0.0);
        bt.run(bars, signals);

        // Calculate overall trading performance using the equity curve and trades
        // Metrics include final portfolio value, max drawdown, number of trades, and winning trades
        auto m = compute_metrics(bt.equity(), bt.trades());

        // Print overall performance metrics to the console
        std::cout << "Trades: " << m.trades
                  << " Wins: " << m.wins
                  << " Final equity: " << m.final_equity
                  << " MaxDD: " << m.max_drawdown << "\n";

        // Print detailed information about the first 10 trades
        // Each trade record includes entry/exit dates and profit/loss
        for (size_t i = 0; i < std::min(bt.trades().size(), (size_t)10); ++i) {
            auto &t = bt.trades()[i];
            std::cout << "Trade " << i << ": "
                      << t.entry_date << " -> " << t.exit_date
                      << " pnl=" << t.pnl << "\n";
        }
    }

    // Catch and handle any runtime errors that occur during execution
    // This prevents the program from crashing and prints a readable error message
    catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    // Return 0 to indicate successful program completion
    return 0;
}
