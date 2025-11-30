// simple linear regression model for predicting stock prices

#include "linear_regression.h"
#include <cmath>
#include <stdexcept>
#include <algorithm>
#include <numeric>

namespace sp {
using namespace std;

LinearRegression::LinearRegression() : trained_(false) {}

// train model using normal equation: (X^T X)^-1 X^T y
bool LinearRegression::train(const vector<vector<double>>& features,
                             const vector<double>& targets) {
    if (features.empty() || targets.empty() || features.size() != targets.size()) {
        return false;
    }
    size_t n_samples = features.size();
    size_t n_features = features[0].size();
    for (const auto& f : features) {
        if (f.size() != n_features) {
            return false;
        }
    }
    // add intercept column (all 1s) to feature matrix
    vector<vector<double>> X(n_samples, vector<double>(n_features + 1));
    for (size_t i = 0; i < n_samples; ++i) {
        X[i][0] = 1.0;
        for (size_t j = 0; j < n_features; ++j) {
            X[i][j + 1] = features[i][j];
        }
    }
    try {
        auto Xt = transpose(X);
        auto XtX = multiply(Xt, X);
        auto XtX_inv = inverse(XtX);
        auto Xty = multiply_vector(Xt, targets);
        coefficients_ = multiply_vector(XtX_inv, Xty);
        trained_ = true;
        return true;
    } catch (const exception&) {
        trained_ = false;
        return false;
    }
}

// predict single price: y = b0 + b1*x1 + b2*x2 + ...
double LinearRegression::predict(const vector<double>& features) const {
    if (!trained_) {
        throw runtime_error("Model not trained");
    }
    if (features.size() + 1 != coefficients_.size()) {
        throw invalid_argument("Feature size mismatch");
    }
    double prediction = coefficients_[0];
    for (size_t i = 0; i < features.size(); ++i) {
        prediction += coefficients_[i + 1] * features[i];
    }
    return prediction;
}

vector<double> LinearRegression::predict_batch(
    const vector<vector<double>>& features) const {
    vector<double> predictions;
    predictions.reserve(features.size());
    for (const auto& f : features) {
        predictions.push_back(predict(f));
    }
    return predictions;
}

// calculate mean squared error on test data
double LinearRegression::evaluate(const vector<vector<double>>& features,
                                  const vector<double>& targets) const {
    if (features.size() != targets.size()) {
        throw invalid_argument("Features and targets size mismatch");
    }
    double mse = 0.0;
    for (size_t i = 0; i < features.size(); ++i) {
        double pred = predict(features[i]);
        double error = targets[i] - pred;
        mse += error * error;
    }
    return mse / features.size();
}

// R² score - how well model fits (1.0 = perfect, 0.0 = bad)
double LinearRegression::r_squared(const vector<vector<double>>& features,
                                   const vector<double>& targets) const {
    double mean_target = accumulate(targets.begin(), targets.end(), 0.0) / targets.size();
    double ss_res = 0.0;
    double ss_tot = 0.0;
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

vector<vector<double>> LinearRegression::transpose(
    const vector<vector<double>>& matrix) const {
    if (matrix.empty()) return {};
    size_t rows = matrix.size();
    size_t cols = matrix[0].size();
    vector<vector<double>> result(cols, vector<double>(rows));
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            result[j][i] = matrix[i][j];
        }
    }
    return result;
}

vector<vector<double>> LinearRegression::multiply(
    const vector<vector<double>>& A,
    const vector<vector<double>>& B) const {
    size_t m = A.size();
    size_t n = B[0].size();
    size_t p = A[0].size();
    vector<vector<double>> result(m, vector<double>(n, 0.0));
    for (size_t i = 0; i < m; ++i) {
        for (size_t j = 0; j < n; ++j) {
            for (size_t k = 0; k < p; ++k) {
                result[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    return result;
}

vector<double> LinearRegression::multiply_vector(
    const vector<vector<double>>& A,
    const vector<double>& b) const {
    size_t m = A.size();
    vector<double> result(m, 0.0);
    for (size_t i = 0; i < m; ++i) {
        for (size_t j = 0; j < A[i].size(); ++j) {
            result[i] += A[i][j] * b[j];
        }
    }
    return result;
}

// matrix inverse using gaussian elimination
vector<vector<double>> LinearRegression::inverse(
    const vector<vector<double>>& matrix) const {
    size_t n = matrix.size();
    vector<vector<double>> aug(n, vector<double>(2 * n));
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            aug[i][j] = matrix[i][j];
            aug[i][j + n] = (i == j) ? 1.0 : 0.0;
        }
    }
    for (size_t i = 0; i < n; ++i) {
        size_t max_row = i;
        for (size_t k = i + 1; k < n; ++k) {
            if (abs(aug[k][i]) > abs(aug[max_row][i])) {
                max_row = k;
            }
        }
        swap(aug[i], aug[max_row]);
        if (abs(aug[i][i]) < 1e-10) {
            throw runtime_error("Matrix is singular");
        }
        double pivot = aug[i][i];
        for (size_t j = 0; j < 2 * n; ++j) {
            aug[i][j] /= pivot;
        }
        for (size_t k = 0; k < n; ++k) {
            if (k != i) {
                double factor = aug[k][i];
                for (size_t j = 0; j < 2 * n; ++j) {
                    aug[k][j] -= factor * aug[i][j];
                }
            }
        }
    }
    vector<vector<double>> result(n, vector<double>(n));
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            result[i][j] = aug[i][j + n];
        }
    }
    return result;
}

} // namespace sp
