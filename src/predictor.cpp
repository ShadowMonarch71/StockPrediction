// main program - loads data, trains model, makes predictions

#include "csv_loader.h"
#include "feature_engineer.h"
#include "linear_regression.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <fstream>

using namespace sp;
using namespace std;

int main(int argc, char* argv[]) {
    // check command line args
    if (argc < 2) {
        cerr << "Usage: predictor <csv-path> [prediction_days=1] [train_ratio=0.8]\n";
        cerr << "\nExample: predictor data/sample.csv 1 0.8\n";
        return 1;
    }
    
    string csv_path = argv[1];
    int prediction_days = argc > 2 ? stoi(argv[2]) : 1;
    double train_ratio = argc > 3 ? stod(argv[3]) : 0.8;
    
    cout << "\n=== Stock Price Predictor ===\n\n";
    
    cout << "Configuration:\n";
    cout << "  Data file: " << csv_path << "\n";
    cout << "  Predicting: " << prediction_days << " day(s) ahead\n";
    cout << "  Train/Test split: " << (train_ratio * 100) << "% / " 
              << ((1 - train_ratio) * 100) << "%\n\n";
    
    try {
        // load csv data
        cout << "[Step 1/5] Loading Historical Data\n";
        CSVLoader loader(csv_path);
        auto bars = loader.load();
        
        if (bars.empty()) {
            cerr << "Error: No data found in CSV file\n";
            return 1;
        }
        
        cout << "  Loaded " << bars.size() << " trading days\n";
        cout << "  Period: " << bars.front().date << " to " << bars.back().date << "\n\n";
        
        // set up features (returns, lagged prices, indicators, etc)
        cout << "[Step 2/5] Engineering Features\n";
        FeatureConfig config;
        config.lag_days = 5;
        config.sma_period = 20;
        config.ema_period = 12;
        config.rsi_period = 14;
        
        FeatureEngineer engineer(config);
        auto [features, targets] = engineer.create_features(bars, prediction_days);
        
        if (features.empty()) {
            cerr << "Error: Insufficient data for feature creation\n";
            return 1;
        }
        
        cout << "  Created " << features.size() << " feature samples\n";
        cout << "  Features per sample: " << features[0].size() << "\n";
        
        auto feature_names = engineer.get_feature_names();
        cout << "  Using: ";
        for (size_t i = 0; i < feature_names.size(); ++i) {
            cout << feature_names[i];
            if (i < feature_names.size() - 1) cout << ", ";
        }
        cout << "\n\n";
        
        // split into training and testing sets
        cout << "[Step 3/5] Preparing Training & Test Sets\n";
        auto [train_X, train_y, test_X, test_y] = 
            engineer.train_test_split(features, targets, train_ratio);
        
        cout << "  Training set: " << train_X.size() << " samples\n";
        cout << "  Test set: " << test_X.size() << " samples\n\n";
        
        // train the model
        cout << "[Step 4/5] Training Model\n";
        LinearRegression model;
        
        if (!model.train(train_X, train_y)) {
            cerr << "Error: Model training failed\n";
            return 1;
        }
        
        cout << "  Model trained successfully\n\n";
        
        auto coeffs = model.coefficients();
        cout << "  Model Parameters:\n";
        cout << "    Intercept: " << fixed << setprecision(4) << coeffs[0] << "\n";
        cout << "    Feature Weights:\n";
        for (size_t i = 0; i < feature_names.size() && i + 1 < coeffs.size(); ++i) {
            cout << "      " << setw(20) << left << feature_names[i] 
                 << ": " << setprecision(6) << coeffs[i + 1] << "\n";
        }
        cout << "\n";
        
        // check how well the model performs
        cout << "[Step 5/5] Evaluating Performance\n\n";
        
        double train_mse = model.evaluate(train_X, train_y);
        double train_rmse = sqrt(train_mse);
        double train_r2 = model.r_squared(train_X, train_y);
        
        cout << "Training Set Performance:\n";
        cout << "  Mean Squared Error:  " << fixed << setprecision(4) << train_mse << "\n";
        cout << "  Root MSE:            " << train_rmse << "\n";
        cout << "  R² Score:            " << setprecision(4) << train_r2 
                  << " (" << (train_r2 * 100) << "%)\n\n";
        
        double test_mse = model.evaluate(test_X, test_y);
        double test_rmse = sqrt(test_mse);
        double test_r2 = model.r_squared(test_X, test_y);
        
        cout << "Test Set Performance:\n";
        cout << "  Mean Squared Error:  " << fixed << setprecision(4) << test_mse << "\n";
        cout << "  Root MSE:            " << test_rmse << "\n";
        cout << "  R² Score:            " << setprecision(4) << test_r2 
                  << " (" << (test_r2 * 100) << "%)\n\n";
        
        // show sample predictions and save to file
        cout << "Sample Predictions (First 10 Test Cases):\n";
        cout << setw(12) << "Actual" << setw(15) << "Predicted" 
                  << setw(13) << "Error" << setw(13) << "Error %\n";
        cout << string(53, '-') << "\n";
        
        ofstream pred_file("output/predictions.csv");
        pred_file << "Index,Actual,Predicted,Error,Error_Percent\n";
        
        for (size_t i = 0; i < min(size_t(10), test_X.size()); ++i) {
            double actual = test_y[i];
            double predicted = model.predict(test_X[i]);
            double error = predicted - actual;
            double error_pct = (error / actual) * 100.0;
            
            cout << fixed << setprecision(2)
                      << "$" << setw(10) << actual
                      << " $" << setw(12) << predicted
                      << "  " << setw(10) << error
                      << "  " << setw(9) << error_pct << "%\n";
        }
        
        for (size_t i = 0; i < test_X.size(); ++i) {
            double actual = test_y[i];
            double predicted = model.predict(test_X[i]);
            double error = predicted - actual;
            double error_pct = (error / actual) * 100.0;
            
            pred_file << i << "," << actual << "," << predicted << "," 
                     << error << "," << error_pct << "\n";
        }
        pred_file.close();
        cout << "\nPredictions saved to output/predictions.csv\n";
        
        ofstream metrics_file("output/metrics.txt");
        metrics_file << "Training MSE: " << train_mse << "\n";
        metrics_file << "Training RMSE: " << train_rmse << "\n";
        metrics_file << "Training R²: " << train_r2 << "\n";
        metrics_file << "Test MSE: " << test_mse << "\n";
        metrics_file << "Test RMSE: " << test_rmse << "\n";
        metrics_file << "Test R²: " << test_r2 << "\n";
        metrics_file.close();
        cout << "Metrics saved to output/metrics.txt\n";
        
        if (!test_X.empty()) {
            cout << "\nMost Recent Prediction:\n";
            double latest_pred = model.predict(test_X.back());
            double latest_actual = test_y.back();
            double latest_error = ((latest_pred - latest_actual) / latest_actual) * 100.0;
            
            cout << "  Actual Price:     $" << fixed << setprecision(2) 
                      << latest_actual << "\n";
            cout << "  Predicted Price:  $" << latest_pred << "\n";
            cout << "  Prediction Error: " << setprecision(2) << abs(latest_error) << "%";
            if (latest_error > 0) cout << " (overestimated)";
            else cout << " (underestimated)";
            cout << "\n";
        }
        
        cout << "\n=== Analysis Complete ===\n\n";
        
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
