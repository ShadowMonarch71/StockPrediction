/*
 * indicator.cpp
 *
 * Provides concrete implementations of the Indicator interface:
 *  - SMAIndicator: simple moving average
 *  - EMAIndicator: exponential moving average
 *  - RSIIndicator: relative strength index (Wilder smoothing)
 *  - MACDIndicator: moving average convergence/divergence (fast EMA - slow EMA)
 *
 * Design notes and behavior:
 *  - Each compute(...) method returns a vector<double> the same length as the
 *    input prices vector. Entries that cannot be computed because of warmup
 *    (not enough historical data) are set to NaN so callers can easily detect
 *    valid/invalid regions.
 *  - All implementations are intentionally simple and easy to read. They do
 *    not attempt to be numerically optimal or handle pathological inputs like
 *    Inf/NaN in the price series — validate inputs upstream if needed.
 *
 * Usage example:
 *  std::vector<double> close = ...;
 *  SMAIndicator sma(20);
 *  auto s = sma.compute(close); // s has same length as close; first ~19 entries are NaN
 *
 * Notes & Caveats:
 *  - NaN semantics: callers should test std::isnan(out[i]) to determine whether
 *    a value is defined. This makes it easy to align indicator vectors with
 *    price vectors without special shifting logic.
 *  - Thread-safety: these compute methods are stateless and safe to call from
 *    multiple threads concurrently as long as each thread uses its own Indicator
 *    instances (or only const operations are performed).
 *  - Precision: algorithms use double and simple arithmetic; for very large
 *    series or high performance needs consider numerically-stable accumulators
 *    or vectorized implementations.
 *
 * Complexity:
 *  - Each compute is O(N) time and O(N) extra space for the output vector.
 */

#include "indicator.h"
#include <cmath>
#include <numeric>

using namespace sp;

// Simple Moving Average (SMA)
// Returns a vector where out[i] is the average of the previous `period_` values
// ending at index i. For indices with insufficient history, out[i] is NaN.
std::vector<double> SMAIndicator::compute(const std::vector<double>& x) {
    // Create an output vector of same size as input, filled with NaN
    // NaN is used to represent undefined values (for early entries where window not complete)
    std::vector<double> out(x.size(), NAN);

    // If the period is invalid, return all NaN
    if (period_ <= 0) return out;

    double sum = 0.0; // holds the running sum of values for moving window

    // Iterate through the price series
    for (std::size_t i = 0; i < x.size(); ++i) {
        sum += x[i]; // add the new price to the window

        // When window exceeds 'period_', remove the oldest value
        if (i >= static_cast<std::size_t>(period_))
            sum -= x[i - period_];

        // Compute SMA only when at least 'period_' data points are available
        if (i + 1 >= static_cast<std::size_t>(period_))
            out[i] = sum / period_;
    }

    // Return the full SMA series
    return out;
}

// Exponential Moving Average (EMA)
// Uses the common smoothing factor alpha = 2/(period+1). We seed the EMA with the
// first price value. This is a simple, robust seeding strategy (alternatives: SMA seed).
std::vector<double> EMAIndicator::compute(const std::vector<double>& x) {
    // Initialize output vector with NaN values
    std::vector<double> out(x.size(), NAN);

    // Validate input: need a positive period and non-empty data
    if (period_ <= 0 || x.empty()) return out;

    // α (alpha) determines how much weight is given to new data
    double alpha = 2.0 / (period_ + 1);

    // Seed the first EMA value with the first price
    double prev = x[0];
    out[0] = prev;

    // Compute EMA for the remaining values
    for (std::size_t i = 1; i < x.size(); ++i) {
        prev = alpha * x[i] + (1 - alpha) * prev;
        out[i] = prev;
    }

    return out;
}

// Relative Strength Index (RSI) using Wilder's smoothing
// Returns values in the range [0,100] where defined; NaN for warmup period.
std::vector<double> RSIIndicator::compute(const std::vector<double>& x) {
    // Output vector initialized with NaN
    std::vector<double> out(x.size(), NAN);

    // Need at least two data points and a valid period
    if (period_ <= 0 || x.size() < 2) return out;

    // Create arrays for daily gains and losses
    std::vector<double> gains(x.size(), 0.0), losses(x.size(), 0.0);

    // Calculate daily price changes
    for (std::size_t i = 1; i < x.size(); ++i) {
        double diff = x[i] - x[i - 1];
        gains[i] = std::max(0.0, diff);   // positive change
        losses[i] = std::max(0.0, -diff); // negative change
    }

    // Compute initial average gain and loss over the first 'period_' window
    double avg_gain = 0.0, avg_loss = 0.0;
    for (int i = 1; i <= period_ && i < (int)gains.size(); ++i) {
        avg_gain += gains[i];
        avg_loss += losses[i];
    }
    avg_gain /= period_;
    avg_loss /= period_;

    // Compute the first valid RSI value
    if (period_ < (int)gains.size()) {
        out[period_] = 100.0 - (100.0 / (1.0 + avg_gain / (avg_loss == 0 ? 1e-12 : avg_loss)));
    }

    // For subsequent values, use smoothed averages (Wilder smoothing)
    for (std::size_t i = period_ + 1; i < x.size(); ++i) {
        avg_gain = (avg_gain * (period_ - 1) + gains[i]) / period_;
        avg_loss = (avg_loss * (period_ - 1) + losses[i]) / period_;
        double rs = avg_gain / (avg_loss == 0 ? 1e-12 : avg_loss);
        out[i] = 100.0 - (100.0 / (1.0 + rs));
    }

    return out;
}

// MACD: difference between a fast EMA and a slow EMA
// This returns the MACD line only (fast - slow). A signal line (EMA of MACD)
// can be computed externally if needed.
std::vector<double> MACDIndicator::compute(const std::vector<double>& prices) {
    // MACD = EMA(fast) - EMA(slow)
    EMAIndicator fastE(fast_);
    EMAIndicator slowE(slow_);

    // Compute both fast and slow EMAs
    auto f = fastE.compute(prices);
    auto s = slowE.compute(prices);

    // Initialize MACD output
    std::vector<double> macd(prices.size(), NAN);

    // Compute difference between fast and slow EMA when both are defined
    for (std::size_t i = 0; i < prices.size(); ++i) {
        if (!std::isnan(f[i]) && !std::isnan(s[i]))
            macd[i] = f[i] - s[i];
    }

    return macd;
}
