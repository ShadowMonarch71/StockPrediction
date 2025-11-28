# Stock Price Predictor - Linear Regression

## Overview

This project now includes **stock price prediction** capabilities using linear regression! The predictor learns from historical price patterns and technical indicators to forecast future stock prices.

## What Changed?

We've transformed the backtesting-only system into a **machine learning-powered stock predictor**:

### New Components

1. **Linear Regression Model** (`linear_regression.h/cpp`)
   - Multiple linear regression implementation
   - Ordinary least squares (OLS) estimation
   - Model evaluation metrics (MSE, RMSE, R²)
   - No external dependencies - pure C++

2. **Feature Engineering** (`feature_engineer.h/cpp`)
   - Transforms raw price data into predictive features
   - Features include:
     - Price returns (percentage changes)
     - Lagged prices (historical values)
     - Technical indicators (SMA, EMA, RSI)
     - Volume-based features
     - Rolling volatility
   - Automatic train/test splitting

3. **Predictor CLI** (`predictor.cpp`)
   - Command-line tool for making predictions
   - Trains model and evaluates performance
   - Shows prediction accuracy metrics

## How It Works

### The Prediction Pipeline

```
Historical Data (CSV)
        ↓
Feature Engineering (16 features)
        ↓
Train/Test Split (80%/20%)
        ↓
Linear Regression Training
        ↓
Model Evaluation & Predictions
```

### Features Used for Prediction

The model uses 16 features to predict future prices:

1. **Returns (5 features):** % price changes for last 5 days
2. **Lagged prices (5 features):** Normalized historical prices
3. **SMA indicator:** 20-day simple moving average
4. **EMA indicator:** 12-day exponential moving average
5. **RSI indicator:** 14-day relative strength index
6. **Volume change:** Day-over-day volume difference
7. **Volume ratio:** Current volume vs 5-day average
8. **Volatility:** 5-day rolling standard deviation

### Mathematical Model

The predictor uses multiple linear regression:

```
Price(t+1) = β₀ + β₁×Feature₁ + β₂×Feature₂ + ... + β₁₆×Feature₁₆
```

Where:
- `Price(t+1)` = Predicted price for next day
- `β₀` = Intercept (base price)
- `βᵢ` = Learned weight for feature i
- `Featureᵢ` = Technical indicator or price-based feature

## Building the Predictor

```powershell
# Clean build
Remove-Item -Recurse -Force build
mkdir build
cd build
cmake ..
cmake --build . --config Release

# You'll get 4 executables:
# - predictor.exe       (NEW: Stock price predictor)
# - predictor_tests.exe (NEW: Predictor unit tests)
# - strategy.exe        (Original backtester)
# - indicator_tests.exe (Original indicator tests)
```

## Usage

### Basic Prediction

```powershell
.\build\Release\predictor.exe data/your_stock_data.csv
```

### Advanced Options

```powershell
# Syntax
predictor <csv-path> [prediction_days] [train_ratio]

# Predict 1 day ahead with 80% training data (default)
.\predictor.exe data/stock.csv 1 0.8

# Predict 5 days ahead with 70% training data
.\predictor.exe data/stock.csv 5 0.7
```

### Parameters

- **csv-path:** Path to OHLCV CSV file (required)
- **prediction_days:** How many days ahead to predict (default: 1)
- **train_ratio:** Fraction of data for training (default: 0.8 = 80%)

### Data Requirements

The predictor needs **at least 100 bars** of historical data for reliable predictions. Smaller datasets may fail during training.

## Example Output

```
=== Stock Price Predictor (Linear Regression) ===

Configuration:
  CSV file: data/ohlcv_100_days_style.csv
  Prediction horizon: 1 day(s)
  Train/test split: 80% / 20%

[1/5] Loading data...
  Loaded 100 bars
  Date range: 2025-10-01 to 2026-01-08

[2/5] Engineering features...
  Created 49 samples
  Features per sample: 16

[3/5] Splitting data...
  Training samples: 39
  Test samples: 10

[4/5] Training linear regression model...
  Model trained successfully
  Intercept: 145.2341
  Feature weights:
    return_lag_1        : 12.456
    return_lag_2        : -8.234
    sma_20_norm         : 23.891
    ...

[5/5] Evaluating model performance...

Training Set Performance:
  MSE:  2.4512
  RMSE: 1.5656
  R²:   0.9234 (92.34%)

Test Set Performance:
  MSE:  3.2145
  RMSE: 1.7929
  R²:   0.8891 (88.91%)

Sample Predictions (first 10 test samples):
    Actual      Predicted       Error     Error %
--------------------------------------------------
    148.50       149.23        0.73       0.49%
    149.20       148.87       -0.33      -0.22%
    150.10       150.45        0.35       0.23%
    ...

=== Latest Prediction ===
Actual price:    $151.25
Predicted price: $151.78
Error:           0.35%
```

## Performance Metrics Explained

- **MSE (Mean Squared Error):** Average squared difference between predictions and actual prices. Lower is better.
- **RMSE (Root Mean Squared Error):** Square root of MSE. Easier to interpret (same units as price).
- **R² (R-squared):** Percentage of variance explained. 1.0 = perfect fit, 0.0 = no better than average.

## Running Tests

```powershell
# Test the predictor components
cd build
.\Release\predictor_tests.exe

# Test original indicators
.\Release\indicator_tests.exe

# Run all tests with CTest
ctest -C Release
```

## Key Differences: Backtesting vs Prediction

| Feature | Backtesting (strategy.exe) | Prediction (predictor.exe) |
|---------|---------------------------|----------------------------|
| **Purpose** | Evaluate historical strategy performance | Forecast future prices |
| **Method** | Rule-based signals (EMA > SMA) | Machine learning regression |
| **Output** | Equity curve, trades, returns | Price predictions, accuracy metrics |
| **Learning** | No learning (fixed rules) | Learns from data patterns |
| **Use Case** | Strategy validation | Price forecasting |

## Limitations & Disclaimers

⚠️ **Important Caveats:**

1. **Educational Purpose:** This is a learning tool, not production trading software
2. **Linear Model:** Simple linear regression may not capture complex market dynamics
3. **Past Performance:** Historical patterns don't guarantee future results
4. **No Risk Management:** Model doesn't account for black swan events or regime changes
5. **Small Sample Warning:** Needs substantial data (100+ bars minimum) for reliable predictions

## Future Enhancements

Potential improvements for the predictor:

- [ ] Ridge/Lasso regularization to prevent overfitting
- [ ] Polynomial features for non-linear relationships
- [ ] Cross-validation for better generalization
- [ ] Feature importance analysis
- [ ] Confidence intervals for predictions
- [ ] Integration with more advanced ML libraries
- [ ] Time series specific models (ARIMA, LSTM)
- [ ] Rolling window predictions
- [ ] Multi-step ahead forecasting

## File Structure

```
src/
├── predictor.cpp          # CLI for stock prediction
├── linear_regression.*    # Linear regression model
├── feature_engineer.*     # Feature extraction
├── indicator.*            # Technical indicators (shared)
├── csv_loader.*           # Data loading (shared)
└── main.cpp              # Original backtester

tests/
├── predictor_tests.cpp    # Predictor unit tests
└── indicator_tests.cpp    # Indicator tests
```

## Technical Details

### Algorithm: Ordinary Least Squares (OLS)

The model solves the normal equation:

```
β = (XᵀX)⁻¹Xᵀy
```

Where:
- `X` = Feature matrix (n samples × m features)
- `y` = Target vector (actual prices)
- `β` = Coefficient vector (weights to learn)
- `Xᵀ` = Transpose of X
- `⁻¹` = Matrix inverse

### Implementation Notes

- **Matrix operations:** Implemented from scratch using standard C++ vectors
- **Gaussian elimination:** Used for matrix inversion
- **Numerical stability:** Partial pivoting to handle ill-conditioned matrices
- **Feature normalization:** Features are normalized to prevent numerical issues

## Contributing

Want to improve the predictor?

1. **Add new features:** Edit `feature_engineer.cpp` to include momentum, Bollinger Bands, etc.
2. **Try different models:** Implement polynomial regression, decision trees
3. **Improve accuracy:** Add regularization, feature selection
4. **Better evaluation:** Add cross-validation, backtesting of predictions

## License

Same as the main project - MIT License

---

**Remember:** Stock prediction is inherently uncertain. This tool is for educational purposes. Always do your own research before making investment decisions!
