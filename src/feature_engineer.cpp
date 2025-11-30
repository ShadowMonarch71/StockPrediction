// builds feature vectors from price history for training

#include "feature_engineer.h"
#include <cmath>
#include <algorithm>
#include <numeric>

namespace sp {
using namespace std;

FeatureEngineer::FeatureEngineer() : config_() {}

FeatureEngineer::FeatureEngineer(const FeatureConfig& config) : config_(config) {}

pair<vector<vector<double>>, vector<double>>
FeatureEngineer::create_features(const vector<Bar>& bars, int prediction_horizon) {
    if (bars.size() < static_cast<size_t>(config_.lag_days + prediction_horizon + 50)) {
        return {{}, {}};
    }
    vector<vector<double>> features;
    vector<double> targets;
    
    // extract closing prices for indicator calculations
    vector<double> closes;
    closes.reserve(bars.size());
    for (const auto& bar : bars) {
        closes.push_back(bar.close);
    }
    
    // precompute all indicators once
    vector<double> sma_values, ema_values, rsi_values;
    if (config_.use_sma) {
        SMAIndicator sma(config_.sma_period);
        sma_values = sma.compute(closes);
    }
    if (config_.use_ema) {
        EMAIndicator ema(config_.ema_period);
        ema_values = ema.compute(closes);
    }
    if (config_.use_rsi) {
        RSIIndicator rsi(config_.rsi_period);
        rsi_values = rsi.compute(closes);
    }
    
    // skip early days where we don't have enough history
    size_t start_idx = max(config_.lag_days, 50);
    size_t end_idx = bars.size() - prediction_horizon;
    
    // build feature vector for each day
    for (size_t i = start_idx; i < end_idx; ++i) {
        vector<double> feature_vec;
        
        // add price returns for last N days
        if (config_.use_returns) {
            for (int lag = 1; lag <= config_.lag_days; ++lag) {
                double ret = (bars[i].close - bars[i - lag].close) / bars[i - lag].close;
                feature_vec.push_back(ret);
            }
        }
        
        // add historical prices normalized by current price
        if (config_.use_lagged_prices) {
            for (int lag = 1; lag <= config_.lag_days; ++lag) {
                double normalized = bars[i - lag].close / bars[i].close;
                feature_vec.push_back(normalized);
            }
        }
        
        // add technical indicators
        if (config_.use_sma && !isnan(sma_values[i])) {
            feature_vec.push_back(sma_values[i] / bars[i].close);
        }
        if (config_.use_ema && !isnan(ema_values[i])) {
            feature_vec.push_back(ema_values[i] / bars[i].close);
        }
        if (config_.use_rsi && !isnan(rsi_values[i])) {
            feature_vec.push_back(rsi_values[i] / 100.0);
        }
        
        // add volume features
        if (config_.use_volume) {
            if (i > 0 && bars[i - 1].volume > 0) {
                double vol_change = (bars[i].volume - bars[i - 1].volume) / bars[i - 1].volume;
                feature_vec.push_back(vol_change);
            } else {
                feature_vec.push_back(0.0);
            }
            double avg_vol = 0.0;
            for (int lag = 1; lag <= 5 && i >= static_cast<size_t>(lag); ++lag) {
                avg_vol += bars[i - lag].volume;
            }
            avg_vol /= 5.0;
            if (avg_vol > 0) {
                feature_vec.push_back(bars[i].volume / avg_vol);
            } else {
                feature_vec.push_back(1.0);
            }
        }
        
        // calculate 5-day volatility
        vector<double> recent_returns;
        for (int lag = 1; lag <= 5 && i >= static_cast<size_t>(lag); ++lag) {
            double ret = (bars[i - lag + 1].close - bars[i - lag].close) / bars[i - lag].close;
            recent_returns.push_back(ret);
        }
        if (!recent_returns.empty()) {
            double mean_ret = accumulate(recent_returns.begin(), recent_returns.end(), 0.0) 
                            / recent_returns.size();
            double variance = 0.0;
            for (double r : recent_returns) {
                variance += (r - mean_ret) * (r - mean_ret);
            }
            variance /= recent_returns.size();
            feature_vec.push_back(std::sqrt(variance));
        }
        
        double target_price = bars[i + prediction_horizon].close;
        
        // skip if any feature is NaN
        bool has_nan = false;
        for (double val : feature_vec) {
            if (isnan(val)) { has_nan = true; break; }
        }
        if (!has_nan) {
            features.push_back(feature_vec);
            targets.push_back(target_price);
        }
    }
    return {features, targets};
}

tuple<vector<vector<double>>, vector<double>,
     vector<vector<double>>, vector<double>>
FeatureEngineer::train_test_split(const vector<vector<double>>& features,
                                  const vector<double>& targets,
                                  double train_ratio) {
    
    size_t n = features.size();
    size_t train_size = static_cast<size_t>(n * train_ratio);
    
    vector<vector<double>> train_features(features.begin(), features.begin() + train_size);
    vector<double> train_targets(targets.begin(), targets.begin() + train_size);
    
    vector<vector<double>> test_features(features.begin() + train_size, features.end());
    vector<double> test_targets(targets.begin() + train_size, targets.end());
    
    return {train_features, train_targets, test_features, test_targets};
}

int FeatureEngineer::get_feature_count() const {
    int count = 0;
    
    if (config_.use_returns) count += config_.lag_days;
    if (config_.use_lagged_prices) count += config_.lag_days;
    if (config_.use_sma) count += 1;
    if (config_.use_ema) count += 1;
    if (config_.use_rsi) count += 1;
    if (config_.use_volume) count += 2;  // volume change + volume ratio
    count += 1;  // volatility
    
    return count;
}

vector<string> FeatureEngineer::get_feature_names() const {
    vector<string> names;
    
    if (config_.use_returns) {
        for (int i = 1; i <= config_.lag_days; ++i) {
            names.push_back("return_lag_" + to_string(i));
        }
    }
    
    if (config_.use_lagged_prices) {
        for (int i = 1; i <= config_.lag_days; ++i) {
            names.push_back("price_lag_" + to_string(i) + "_norm");
        }
    }
    
    if (config_.use_sma) names.push_back("sma_" + to_string(config_.sma_period) + "_norm");
    if (config_.use_ema) names.push_back("ema_" + to_string(config_.ema_period) + "_norm");
    if (config_.use_rsi) names.push_back("rsi_" + to_string(config_.rsi_period) + "_norm");
    
    if (config_.use_volume) {
        names.push_back("volume_change");
        names.push_back("volume_ratio_5d");
    }
    
    names.push_back("volatility_5d");
    
    return names;
}

} 
