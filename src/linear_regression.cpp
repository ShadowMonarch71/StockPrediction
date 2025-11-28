#include "linear_regression.h"
#include <cmath>
#include <stdexcept>
#include <algorithm>
#include <numeric>

namespace sp {

LinearRegression::LinearRegression() : trained_(false) {}

bool LinearRegression::train(const std::vector<std::vector<double>>& features,
                             const std::vector<double>& targets) {
    // Validate input dimensions
    if (features.empty() || targets.empty() || features.size() != targets.size()) {
        return false;
    }
    
    size_t n_samples = features.size();
    size_t n_features = features[0].size();
    
    // Check all feature vectors have same dimension
    for (const auto& f : features) {
        if (f.size() != n_features) {
            return false;
        }
    }
    
    // Build design matrix X with intercept column (add column of 1s)
    std::vector<std::vector<double>> X(n_samples, std::vector<double>(n_features + 1));
    for (size_t i = 0; i < n_samples; ++i) {
        X[i][0] = 1.0;  // Intercept term
        for (size_t j = 0; j < n_features; ++j) {
            X[i][j + 1] = features[i][j];
        }
    }
    
    // Calculate coefficients using normal equation: β = (XᵀX)⁻¹Xᵀy
    try {
        auto Xt = transpose(X);
        auto XtX = multiply(Xt, X);
        auto XtX_inv = inverse(XtX);
        auto Xty = multiply_vector(Xt, targets);
        coefficients_ = multiply_vector(XtX_inv, Xty);
        
        trained_ = true;
        return true;
    } catch (const std::exception&) {
        trained_ = false;
        return false;
    }
}

double LinearRegression::predict(const std::vector<double>& features) const {
    if (!trained_) {
        throw std::runtime_error("Model not trained");
    }
    
    if (features.size() + 1 != coefficients_.size()) {
        throw std::invalid_argument("Feature size mismatch");
    }
    
    // y = β₀ + β₁x₁ + β₂x₂ + ... + βₙxₙ
    double prediction = coefficients_[0];  // Intercept
    for (size_t i = 0; i < features.size(); ++i) {
        prediction += coefficients_[i + 1] * features[i];
    }
    
    return prediction;
}

std::vector<double> LinearRegression::predict_batch(
    const std::vector<std::vector<double>>& features) const {
    
    std::vector<double> predictions;
    predictions.reserve(features.size());
    
    for (const auto& f : features) {
        predictions.push_back(predict(f));
    }
    
    return predictions;
}

double LinearRegression::evaluate(const std::vector<std::vector<double>>& features,
                                  const std::vector<double>& targets) const {
    if (features.size() != targets.size()) {
        throw std::invalid_argument("Features and targets size mismatch");
    }
    
    // Calculate Mean Squared Error (MSE)
    double mse = 0.0;
    for (size_t i = 0; i < features.size(); ++i) {
        double pred = predict(features[i]);
        double error = targets[i] - pred;
        mse += error * error;
    }
    
    return mse / features.size();
}

double LinearRegression::r_squared(const std::vector<std::vector<double>>& features,
                                   const std::vector<double>& targets) const {
    // Calculate R² = 1 - (SS_res / SS_tot)
    // SS_res = sum of squared residuals
    // SS_tot = total sum of squares
    
    double mean_target = std::accumulate(targets.begin(), targets.end(), 0.0) / targets.size();
    
    double ss_res = 0.0;  // Residual sum of squares
    double ss_tot = 0.0;  // Total sum of squares
    
    for (size_t i = 0; i < features.size(); ++i) {
        double pred = predict(features[i]);
        double residual = targets[i] - pred;
        double total = targets[i] - mean_target;
        
        ss_res += residual * residual;
        ss_tot += total * total;
    }
    
    if (ss_tot == 0.0) return 0.0;
    
    return 1.0 - (ss_res / ss_tot);
}

// Matrix operations for normal equation

std::vector<std::vector<double>> LinearRegression::transpose(
    const std::vector<std::vector<double>>& matrix) const {
    
    if (matrix.empty()) return {};
    
    size_t rows = matrix.size();
    size_t cols = matrix[0].size();
    
    std::vector<std::vector<double>> result(cols, std::vector<double>(rows));
    
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            result[j][i] = matrix[i][j];
        }
    }
    
    return result;
}

std::vector<std::vector<double>> LinearRegression::multiply(
    const std::vector<std::vector<double>>& A,
    const std::vector<std::vector<double>>& B) const {
    
    size_t m = A.size();
    size_t n = B[0].size();
    size_t p = A[0].size();
    
    std::vector<std::vector<double>> result(m, std::vector<double>(n, 0.0));
    
    for (size_t i = 0; i < m; ++i) {
        for (size_t j = 0; j < n; ++j) {
            for (size_t k = 0; k < p; ++k) {
                result[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    
    return result;
}

std::vector<double> LinearRegression::multiply_vector(
    const std::vector<std::vector<double>>& A,
    const std::vector<double>& b) const {
    
    size_t m = A.size();
    std::vector<double> result(m, 0.0);
    
    for (size_t i = 0; i < m; ++i) {
        for (size_t j = 0; j < A[i].size(); ++j) {
            result[i] += A[i][j] * b[j];
        }
    }
    
    return result;
}

// Simple Gaussian elimination for matrix inverse (for small matrices)
std::vector<std::vector<double>> LinearRegression::inverse(
    const std::vector<std::vector<double>>& matrix) const {
    
    size_t n = matrix.size();
    
    // Create augmented matrix [A | I]
    std::vector<std::vector<double>> aug(n, std::vector<double>(2 * n));
    
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            aug[i][j] = matrix[i][j];
            aug[i][j + n] = (i == j) ? 1.0 : 0.0;
        }
    }
    
    // Forward elimination with partial pivoting
    for (size_t i = 0; i < n; ++i) {
        // Find pivot
        size_t max_row = i;
        for (size_t k = i + 1; k < n; ++k) {
            if (std::abs(aug[k][i]) > std::abs(aug[max_row][i])) {
                max_row = k;
            }
        }
        
        // Swap rows
        std::swap(aug[i], aug[max_row]);
        
        // Check for singular matrix
        if (std::abs(aug[i][i]) < 1e-10) {
            throw std::runtime_error("Matrix is singular");
        }
        
        // Scale pivot row
        double pivot = aug[i][i];
        for (size_t j = 0; j < 2 * n; ++j) {
            aug[i][j] /= pivot;
        }
        
        // Eliminate column
        for (size_t k = 0; k < n; ++k) {
            if (k != i) {
                double factor = aug[k][i];
                for (size_t j = 0; j < 2 * n; ++j) {
                    aug[k][j] -= factor * aug[i][j];
                }
            }
        }
    }
    
    // Extract inverse from augmented matrix
    std::vector<std::vector<double>> result(n, std::vector<double>(n));
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            result[i][j] = aug[i][j + n];
        }
    }
    
    return result;
}

} // namespace sp
