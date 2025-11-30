// converts raw price data into ML features for prediction

#pragma once
#include "csv_loader.h"
#include "indicator.h"
#include <vector>
#include <memory>

namespace sp {

// controls which features to generate
struct FeatureConfig {
    bool use_returns = true;
    bool use_lagged_prices = true;
    bool use_sma = true;
    bool use_ema = true;
    bool use_rsi = true;
    bool use_volume = true;
    
    int lag_days = 5;
    int sma_period = 20;
    int ema_period = 12;
    int rsi_period = 14;
};

class FeatureEngineer {
public:
    FeatureEngineer();
    explicit FeatureEngineer(const FeatureConfig& config);
    
    // turns price history into feature matrix + targets
    std::pair<std::vector<std::vector<double>>, std::vector<double>>
    create_features(const std::vector<Bar>& bars, int prediction_horizon = 1);
    
    // splits data into train and test sets
    std::tuple<std::vector<std::vector<double>>, std::vector<double>,
               std::vector<std::vector<double>>, std::vector<double>>
    train_test_split(const std::vector<std::vector<double>>& features,
                     const std::vector<double>& targets,
                     double train_ratio = 0.8);
    
    int get_feature_count() const;
    std::vector<std::string> get_feature_names() const;
    
private:
    FeatureConfig config_;
    
    std::vector<double> extract_returns(const std::vector<Bar>& bars, size_t idx) const;
    std::vector<double> extract_lagged_prices(const std::vector<Bar>& bars, size_t idx) const;
    std::vector<double> extract_indicators(const std::vector<Bar>& bars, size_t idx) const;
    std::vector<double> extract_volume_features(const std::vector<Bar>& bars, size_t idx) const;
};

} 
