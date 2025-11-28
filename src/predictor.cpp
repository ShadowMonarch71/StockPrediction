/*
 * predictor.cpp
 *
 * Main entry point for stock price prediction using linear regression.
 * This CLI application loads historical data, trains a linear regression model,
 * and predicts future stock prices.
 *
 * CLI Usage:
 *  predictor <csv-path> [prediction_days=1] [train_ratio=0.8]
 *
 * Arguments:
 *  - csv-path: Path to OHLCV CSV file
 *  - prediction_days: How many days ahead to predict (default: 1)
 *  - train_ratio: Fraction of data for training (default: 0.8 = 80%)
 *
 * Workflow:
 *  1. Load historical bar data from CSV
 *  2. Engineer features from price history and indicators
 *  3. Split data into training (80%) and test (20%) sets
 *  4. Train linear regression model on training set
 *  5. Evaluate on test set (MSE, RMSE, R²)
 *  6. Make predictions and display results
 *
 * Example:
 *  predictor data/sample.csv 1 0.8
 *  Output: Predicts next-day closing price with model performance metrics
 */

#include "csv_loader.h"
#include "feature_engineer.h"
#include "linear_regression.h"
#include <iostream>
#include <iomanip>
#include <cmath>

using namespace sp;

int main(int argc, char* argv[]) {
    
    // Parse command-line arguments
    if (argc < 2) {
        std::cerr << "Usage: predictor <csv-path> [prediction_days=1] [train_ratio=0.8]\n";
        std::cerr << "\nExample: predictor data/sample.csv 1 0.8\n";
        return 1;
    }
    
    std::string csv_path = argv[1];
    int prediction_days = argc > 2 ? std::stoi(argv[2]) : 1;
    double train_ratio = argc > 3 ? std::stod(argv[3]) : 0.8;
    
    std::cout << "=== Stock Price Predictor (Linear Regression) ===\n\n";
    std::cout << "Configuration:\n";
    std::cout << "  CSV file: " << csv_path << "\n";
    std::cout << "  Prediction horizon: " << prediction_days << " day(s)\n";
    std::cout << "  Train/test split: " << (train_ratio * 100) << "% / " 
              << ((1 - train_ratio) * 100) << "%\n\n";
    
    try {
        // Step 1: Load historical data
        std::cout << "[1/5] Loading data...\n";
        CSVLoader loader(csv_path);
        auto bars = loader.load();
        
        if (bars.empty()) {
            std::cerr << "Error: No data loaded from CSV\n";
            return 1;
        }
        
        std::cout << "  Loaded " << bars.size() << " bars\n";
        std::cout << "  Date range: " << bars.front().date << " to " << bars.back().date << "\n\n";
        
        // Step 2: Engineer features
        std::cout << "[2/5] Engineering features...\n";
        FeatureConfig config;
        config.lag_days = 5;
        config.sma_period = 20;
        config.ema_period = 12;
        config.rsi_period = 14;
        
        FeatureEngineer engineer(config);
        auto [features, targets] = engineer.create_features(bars, prediction_days);
        
        if (features.empty()) {
            std::cerr << "Error: Not enough data to create features\n";
            return 1;
        }
        
        std::cout << "  Created " << features.size() << " samples\n";
        std::cout << "  Features per sample: " << features[0].size() << "\n";
        
        // Display feature names
        auto feature_names = engineer.get_feature_names();
        std::cout << "  Features: ";
        for (size_t i = 0; i < feature_names.size(); ++i) {
            std::cout << feature_names[i];
            if (i < feature_names.size() - 1) std::cout << ", ";
        }
        std::cout << "\n\n";
        
        // Step 3: Split into training and test sets
        std::cout << "[3/5] Splitting data...\n";
        auto [train_X, train_y, test_X, test_y] = 
            engineer.train_test_split(features, targets, train_ratio);
        
        std::cout << "  Training samples: " << train_X.size() << "\n";
        std::cout << "  Test samples: " << test_X.size() << "\n\n";
        
        // Step 4: Train model
        std::cout << "[4/5] Training linear regression model...\n";
        LinearRegression model;
        
        if (!model.train(train_X, train_y)) {
            std::cerr << "Error: Model training failed\n";
            return 1;
        }
        
        std::cout << "  Model trained successfully\n";
        
        // Display learned coefficients
        auto coeffs = model.coefficients();
        std::cout << "  Intercept: " << std::fixed << std::setprecision(4) << coeffs[0] << "\n";
        std::cout << "  Feature weights:\n";
        for (size_t i = 0; i < feature_names.size() && i + 1 < coeffs.size(); ++i) {
            std::cout << "    " << std::setw(20) << std::left << feature_names[i] 
                      << ": " << std::setprecision(6) << coeffs[i + 1] << "\n";
        }
        std::cout << "\n";
        
        // Step 5: Evaluate model
        std::cout << "[5/5] Evaluating model performance...\n\n";
        
        // Training set metrics
        double train_mse = model.evaluate(train_X, train_y);
        double train_rmse = std::sqrt(train_mse);
        double train_r2 = model.r_squared(train_X, train_y);
        
        std::cout << "Training Set Performance:\n";
        std::cout << "  MSE:  " << std::fixed << std::setprecision(4) << train_mse << "\n";
        std::cout << "  RMSE: " << train_rmse << "\n";
        std::cout << "  R²:   " << std::setprecision(4) << train_r2 
                  << " (" << (train_r2 * 100) << "%)\n\n";
        
        // Test set metrics
        double test_mse = model.evaluate(test_X, test_y);
        double test_rmse = std::sqrt(test_mse);
        double test_r2 = model.r_squared(test_X, test_y);
        
        std::cout << "Test Set Performance:\n";
        std::cout << "  MSE:  " << std::fixed << std::setprecision(4) << test_mse << "\n";
        std::cout << "  RMSE: " << test_rmse << "\n";
        std::cout << "  R²:   " << std::setprecision(4) << test_r2 
                  << " (" << (test_r2 * 100) << "%)\n\n";
        
        // Show sample predictions
        std::cout << "Sample Predictions (first 10 test samples):\n";
        std::cout << std::setw(10) << "Actual" << std::setw(15) << "Predicted" 
                  << std::setw(12) << "Error" << std::setw(12) << "Error %\n";
        std::cout << std::string(50, '-') << "\n";
        
        for (size_t i = 0; i < std::min(size_t(10), test_X.size()); ++i) {
            double actual = test_y[i];
            double predicted = model.predict(test_X[i]);
            double error = predicted - actual;
            double error_pct = (error / actual) * 100.0;
            
            std::cout << std::fixed << std::setprecision(2)
                      << std::setw(10) << actual
                      << std::setw(15) << predicted
                      << std::setw(12) << error
                      << std::setw(11) << error_pct << "%\n";
        }
        
        // Make prediction for the most recent data
        if (!test_X.empty()) {
            std::cout << "\n=== Latest Prediction ===\n";
            double latest_pred = model.predict(test_X.back());
            double latest_actual = test_y.back();
            double latest_error = ((latest_pred - latest_actual) / latest_actual) * 100.0;
            
            std::cout << "Actual price:    $" << std::fixed << std::setprecision(2) 
                      << latest_actual << "\n";
            std::cout << "Predicted price: $" << latest_pred << "\n";
            std::cout << "Error:           " << std::setprecision(2) << latest_error << "%\n";
        }
        
        std::cout << "\n=== Prediction Complete ===\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
