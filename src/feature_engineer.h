/*
 * feature_engineer.h
 *
 * Transforms raw OHLCV bar data into feature vectors for machine learning.
 * Creates predictive features from historical price data using technical
 * indicators, price changes, and rolling statistics.
 *
 * Key concepts:
 *  - Features: Input variables used to predict future prices
 *  - Target: The value we want to predict (e.g., next day's close price)
 *  - Lag: Using past values as features (e.g., price 5 days ago)
 *
 * Available features:
 *  - Price returns (percent change)
 *  - Lagged prices (historical prices from N days ago)
 *  - Technical indicators (SMA, EMA, RSI)
 *  - Rolling statistics (volatility, momentum)
 *  - Volume-based features
 *
 * Workflow:
 *  1. Load historical bar data
 *  2. Extract features for each bar (using past data only)
 *  3. Create target values (future price to predict)
 *  4. Split into training and test sets
 *  5. Train model on training set, evaluate on test set
 *
 * Usage example:
 *  FeatureEngineer fe;
 *  auto [features, targets] = fe.create_features(bars, 1);  // Predict 1 day ahead
 *  // features[i] contains features for day i
 *  // targets[i] is the actual price on day i+1
 */

#pragma once
#include "csv_loader.h"
#include "indicator.h"
#include <vector>
#include <memory>

namespace sp {

// Feature configuration for customizing what features to generate
struct FeatureConfig {
    bool use_returns = true;          // Price return features (% change)
    bool use_lagged_prices = true;    // Historical price values
    bool use_sma = true;              // Simple moving average
    bool use_ema = true;              // Exponential moving average
    bool use_rsi = true;              // Relative Strength Index
    bool use_volume = true;           // Volume-based features
    
    int lag_days = 5;                 // Number of lagged values to include
    int sma_period = 20;              // SMA period
    int ema_period = 12;              // EMA period
    int rsi_period = 14;              // RSI period
};

// Feature engineering for stock price prediction
class FeatureEngineer {
public:
    // Constructor with default configuration
    FeatureEngineer();
    
    // Constructor with custom configuration
    explicit FeatureEngineer(const FeatureConfig& config);
    
    // Create feature matrix and target vector from bar data
    // bars: Historical OHLCV data
    // prediction_horizon: How many days ahead to predict (1 = next day)
    // Returns: pair of (features, targets) where features[i] predicts targets[i]
    std::pair<std::vector<std::vector<double>>, std::vector<double>>
    create_features(const std::vector<Bar>& bars, int prediction_horizon = 1);
    
    // Split data into training and test sets
    // features: Full feature matrix
    // targets: Full target vector
    // train_ratio: Fraction of data to use for training (e.g., 0.8 = 80%)
    // Returns: tuple of (train_features, train_targets, test_features, test_targets)
    std::tuple<std::vector<std::vector<double>>, std::vector<double>,
               std::vector<std::vector<double>>, std::vector<double>>
    train_test_split(const std::vector<std::vector<double>>& features,
                     const std::vector<double>& targets,
                     double train_ratio = 0.8);
    
    // Get number of features that will be generated with current config
    int get_feature_count() const;
    
    // Get feature names for interpretability
    std::vector<std::string> get_feature_names() const;
    
private:
    FeatureConfig config_;
    
    // Feature extraction helpers
    std::vector<double> extract_returns(const std::vector<Bar>& bars, size_t idx) const;
    std::vector<double> extract_lagged_prices(const std::vector<Bar>& bars, size_t idx) const;
    std::vector<double> extract_indicators(const std::vector<Bar>& bars, size_t idx) const;
    std::vector<double> extract_volume_features(const std::vector<Bar>& bars, size_t idx) const;
};

} // namespace sp
