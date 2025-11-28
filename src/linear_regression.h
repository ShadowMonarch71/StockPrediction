/*
 * linear_regression.h
 *
 * Implements a simple linear regression model for stock price prediction.
 * Uses ordinary least squares (OLS) to fit a linear model to historical data
 * and predict future prices based on features.
 *
 * Key features:
 *  - Multiple linear regression (multiple features/predictors)
 *  - Ordinary least squares estimation
 *  - Training on historical data
 *  - Single-step and multi-step prediction
 *  - Model evaluation metrics (MSE, RMSE, R²)
 *
 * Mathematical foundation:
 *  Model: y = β₀ + β₁x₁ + β₂x₂ + ... + βₙxₙ
 *  Where: y = predicted price, xᵢ = features, βᵢ = coefficients
 *
 * Training process:
 *  1. Construct feature matrix X and target vector y from training data
 *  2. Calculate coefficients using normal equation: β = (XᵀX)⁻¹Xᵀy
 *  3. Store coefficients for prediction
 *
 * Usage example:
 *  LinearRegression model;
 *  model.train(features, targets);  // features = 2D vector, targets = 1D vector
 *  double prediction = model.predict(new_features);
 *  double error = model.evaluate(test_features, test_targets);
 */

#pragma once
#include <vector>
#include <string>

namespace sp {

// Linear regression model for price prediction
class LinearRegression {
public:
    // Constructor initializes an empty model
    LinearRegression();
    
    // Train the model using feature matrix and target values
    // features: 2D vector where features[i] = feature vector for sample i
    // targets: 1D vector where targets[i] = actual price for sample i
    // Returns: true if training successful, false if data is invalid
    bool train(const std::vector<std::vector<double>>& features, 
               const std::vector<double>& targets);
    
    // Predict a single price given feature vector
    // features: Vector of feature values for prediction
    // Returns: Predicted price
    double predict(const std::vector<double>& features) const;
    
    // Predict multiple prices for a batch of feature vectors
    // features: 2D vector of feature vectors
    // Returns: Vector of predicted prices
    std::vector<double> predict_batch(const std::vector<std::vector<double>>& features) const;
    
    // Evaluate model performance on test data
    // features: Test feature matrix
    // targets: Actual test target values
    // Returns: Mean Squared Error (MSE)
    double evaluate(const std::vector<std::vector<double>>& features,
                    const std::vector<double>& targets) const;
    
    // Calculate R² (coefficient of determination) on test data
    // Returns: R² value (1.0 = perfect fit, 0.0 = no better than mean)
    double r_squared(const std::vector<std::vector<double>>& features,
                     const std::vector<double>& targets) const;
    
    // Get the learned coefficients (weights)
    // Returns: Vector where [0] = intercept, [1..n] = feature weights
    const std::vector<double>& coefficients() const { return coefficients_; }
    
    // Check if model has been trained
    bool is_trained() const { return trained_; }
    
private:
    std::vector<double> coefficients_;  // Model parameters [intercept, weight1, weight2, ...]
    bool trained_;                      // Whether model has been trained
    
    // Helper: Matrix multiplication for normal equation
    std::vector<std::vector<double>> transpose(const std::vector<std::vector<double>>& matrix) const;
    std::vector<std::vector<double>> multiply(const std::vector<std::vector<double>>& A,
                                               const std::vector<std::vector<double>>& B) const;
    std::vector<double> multiply_vector(const std::vector<std::vector<double>>& A,
                                         const std::vector<double>& b) const;
    std::vector<std::vector<double>> inverse(const std::vector<std::vector<double>>& matrix) const;
};

} // namespace sp
