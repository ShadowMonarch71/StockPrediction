/*
 * predictor_tests.cpp
 *
 * Unit tests for linear regression predictor components.
 * Tests the correctness of the linear regression model and feature engineering.
 */

#include "../src/linear_regression.h"
#include "../src/feature_engineer.h"
#include "../src/csv_loader.h"
#include <iostream>
#include <cmath>
#include <vector>

using namespace sp;

bool approx_eq(double a, double b, double tol = 1e-6) {
    return std::fabs(a - b) <= tol;
}

// Test 1: Linear regression on simple dataset
bool test_linear_regression_basic() {
    std::cout << "Test 1: Basic linear regression...\n";
    
    // Create simple dataset: y = 2x + 1
    std::vector<std::vector<double>> features = {{1.0}, {2.0}, {3.0}, {4.0}, {5.0}};
    std::vector<double> targets = {3.0, 5.0, 7.0, 9.0, 11.0};
    
    LinearRegression model;
    if (!model.train(features, targets)) {
        std::cerr << "  FAIL: Training failed\n";
        return false;
    }
    
    // Coefficients should be approximately [1.0, 2.0] (intercept, slope)
    auto coeffs = model.coefficients();
    if (!approx_eq(coeffs[0], 1.0, 0.1) || !approx_eq(coeffs[1], 2.0, 0.1)) {
        std::cerr << "  FAIL: Coefficients incorrect. Got [" << coeffs[0] << ", " << coeffs[1] << "]\n";
        return false;
    }
    
    // Test prediction
    double pred = model.predict({6.0});
    if (!approx_eq(pred, 13.0, 0.1)) {
        std::cerr << "  FAIL: Prediction incorrect. Expected 13, got " << pred << "\n";
        return false;
    }
    
    std::cout << "  PASS\n";
    return true;
}

// Test 2: Multiple linear regression
bool test_multiple_regression() {
    std::cout << "Test 2: Multiple linear regression...\n";
    
    // y = 1 + 2*x1 + 3*x2
    std::vector<std::vector<double>> features = {
        {1.0, 1.0}, {2.0, 1.0}, {3.0, 2.0}, {4.0, 2.0}, {5.0, 3.0}
    };
    std::vector<double> targets = {6.0, 8.0, 12.0, 14.0, 18.0};
    
    LinearRegression model;
    if (!model.train(features, targets)) {
        std::cerr << "  FAIL: Training failed\n";
        return false;
    }
    
    // Test prediction
    double pred = model.predict({6.0, 4.0});  // Should be 1 + 2*6 + 3*4 = 25
    if (!approx_eq(pred, 25.0, 0.5)) {
        std::cerr << "  FAIL: Prediction incorrect. Expected 25, got " << pred << "\n";
        return false;
    }
    
    std::cout << "  PASS\n";
    return true;
}

// Test 3: Model evaluation metrics
bool test_evaluation_metrics() {
    std::cout << "Test 3: Evaluation metrics...\n";
    
    // Perfect fit case
    std::vector<std::vector<double>> features = {{1.0}, {2.0}, {3.0}};
    std::vector<double> targets = {2.0, 4.0, 6.0};
    
    LinearRegression model;
    model.train(features, targets);
    
    // MSE should be 0 for perfect fit
    double mse = model.evaluate(features, targets);
    if (!approx_eq(mse, 0.0, 1e-4)) {
        std::cerr << "  FAIL: MSE should be ~0 for perfect fit, got " << mse << "\n";
        return false;
    }
    
    // R² should be 1.0 for perfect fit
    double r2 = model.r_squared(features, targets);
    if (!approx_eq(r2, 1.0, 0.01)) {
        std::cerr << "  FAIL: R² should be ~1.0 for perfect fit, got " << r2 << "\n";
        return false;
    }
    
    std::cout << "  PASS\n";
    return true;
}

// Test 4: Feature engineering creates valid features
bool test_feature_engineering() {
    std::cout << "Test 4: Feature engineering...\n";
    
    // Create sample bars
    std::vector<Bar> bars;
    for (int i = 0; i < 100; ++i) {
        Bar b;
        b.date = "2025-01-" + std::to_string(i + 1);
        b.open = 100.0 + i;
        b.high = 105.0 + i;
        b.low = 95.0 + i;
        b.close = 100.0 + i * 0.5;  // Upward trend
        b.volume = 1000000;
        bars.push_back(b);
    }
    
    FeatureEngineer engineer;
    auto [features, targets] = engineer.create_features(bars, 1);
    
    if (features.empty()) {
        std::cerr << "  FAIL: No features created\n";
        return false;
    }
    
    if (features.size() != targets.size()) {
        std::cerr << "  FAIL: Feature and target size mismatch\n";
        return false;
    }
    
    // Check all features have same dimension
    size_t feature_dim = features[0].size();
    for (const auto& f : features) {
        if (f.size() != feature_dim) {
            std::cerr << "  FAIL: Inconsistent feature dimensions\n";
            return false;
        }
    }
    
    // Check for NaN values
    for (const auto& f : features) {
        for (double val : f) {
            if (std::isnan(val)) {
                std::cerr << "  FAIL: Feature contains NaN\n";
                return false;
            }
        }
    }
    
    std::cout << "  Created " << features.size() << " samples with " 
              << feature_dim << " features each\n";
    std::cout << "  PASS\n";
    return true;
}

// Test 5: Train-test split
bool test_train_test_split() {
    std::cout << "Test 5: Train-test split...\n";
    
    std::vector<std::vector<double>> features(100, std::vector<double>(5, 1.0));
    std::vector<double> targets(100, 1.0);
    
    FeatureEngineer engineer;
    auto [train_X, train_y, test_X, test_y] = engineer.train_test_split(features, targets, 0.8);
    
    if (train_X.size() != 80 || test_X.size() != 20) {
        std::cerr << "  FAIL: Split sizes incorrect. Train: " << train_X.size() 
                  << ", Test: " << test_X.size() << "\n";
        return false;
    }
    
    if (train_y.size() != 80 || test_y.size() != 20) {
        std::cerr << "  FAIL: Target split sizes incorrect\n";
        return false;
    }
    
    std::cout << "  PASS\n";
    return true;
}

int main() {
    std::cout << "=== Predictor Unit Tests ===\n\n";
    
    int passed = 0;
    int total = 5;
    
    if (test_linear_regression_basic()) passed++;
    if (test_multiple_regression()) passed++;
    if (test_evaluation_metrics()) passed++;
    if (test_feature_engineering()) passed++;
    if (test_train_test_split()) passed++;
    
    std::cout << "\n=== Results: " << passed << "/" << total << " tests passed ===\n";
    
    if (passed == total) {
        std::cout << "All tests PASSED!\n";
        return 0;
    } else {
        std::cout << (total - passed) << " test(s) FAILED\n";
        return 1;
    }
}
