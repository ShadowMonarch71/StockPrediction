/*
 * indicator.h
 *
 * Defines technical indicators for analyzing price data and generating trading signals.
 * All indicators implement the Indicator interface and return vectors of computed values
 * aligned with the input price series.
 *
 * Available indicators:
 *  - SMAIndicator: Simple Moving Average - unweighted average of N prices
 *  - EMAIndicator: Exponential Moving Average - weighted average favoring recent prices
 *  - RSIIndicator: Relative Strength Index - momentum oscillator (0-100 range)
 *  - MACDIndicator: Moving Average Convergence Divergence - trend-following momentum
 *
 * Usage pattern:
 *  1. Create an indicator instance with desired period(s)
 *  2. Call compute() with a vector of prices
 *  3. Receive a vector of indicator values (same length as input)
 *  4. Early values may be NaN where insufficient data exists for calculation
 *
 * Example:
 *  std::vector<double> closes = {100, 101, 102, 103, 104};
 *  SMAIndicator sma(3);
 *  auto values = sma.compute(closes);  // [NaN, NaN, 101, 102, 103]
 */

#pragma once
#include <vector>

namespace sp {

// Abstract base class for all technical indicators.
// Defines the interface that all concrete indicators must implement.
class Indicator {
public:
    // Virtual destructor ensures proper cleanup of derived classes
    virtual ~Indicator() = default;
    
    // Compute indicator values from a price series.
    // Returns a vector of the same length as input, with NaN for undefined warmup values.
    virtual std::vector<double> compute(const std::vector<double>& prices) = 0;
};

// Simple Moving Average (SMA) indicator.
// Computes the arithmetic mean of the last N prices for each position.
// Formula: SMA[i] = (Price[i] + Price[i-1] + ... + Price[i-period+1]) / period
class SMAIndicator : public Indicator {
public:
    // Constructor takes the lookback period (number of bars to average)
    explicit SMAIndicator(int period) : period_(period) {}
    
    // Compute SMA values for the given price series
    std::vector<double> compute(const std::vector<double>& prices) override;
    
private:
    int period_;  // Number of periods to include in the moving average
};

// Exponential Moving Average (EMA) indicator.
// Gives more weight to recent prices using exponential decay.
// Formula: EMA[i] = α × Price[i] + (1-α) × EMA[i-1], where α = 2/(period+1)
class EMAIndicator : public Indicator {
public:
    // Constructor takes the smoothing period
    explicit EMAIndicator(int period) : period_(period) {}
    
    // Compute EMA values for the given price series
    std::vector<double> compute(const std::vector<double>& prices) override;
    
private:
    int period_;  // Period used to calculate the smoothing factor
};

// Relative Strength Index (RSI) indicator.
// Measures momentum on a 0-100 scale, comparing average gains to average losses.
// Values above 70 typically indicate overbought, below 30 indicate oversold.
// Formula: RSI = 100 - (100 / (1 + RS)), where RS = avg_gain / avg_loss
class RSIIndicator : public Indicator {
public:
    // Constructor takes the lookback period for averaging gains/losses
    explicit RSIIndicator(int period) : period_(period) {}
    
    // Compute RSI values for the given price series
    std::vector<double> compute(const std::vector<double>& prices) override;
    
private:
    int period_;  // Number of periods for smoothing gains and losses
};

// Moving Average Convergence Divergence (MACD) indicator.
// Measures the relationship between two EMAs to identify trend changes.
// Formula: MACD = EMA(fast) - EMA(slow)
// Common values: fast=12, slow=26
class MACDIndicator : public Indicator {
public:
    // Constructor takes both fast and slow EMA periods
    MACDIndicator(int fast, int slow) : fast_(fast), slow_(slow) {}
    
    // Compute MACD line values (difference between fast and slow EMAs)
    std::vector<double> compute(const std::vector<double>& prices) override;
    
private:
    int fast_;  // Period for the fast EMA
    int slow_;  // Period for the slow EMA
};

} // namespace sp
