# Stock Price Predictor - Project Summary

## Overview
A C++ implementation of a stock price prediction system using multiple linear regression with engineered features. The model predicts next-day closing prices from historical OHLCV (Open, High, Low, Close, Volume) data.

---

## Project Architecture

### Core Components

1. **CSV Loader** (`csv_loader.h/cpp`)
   - Parses historical stock data from CSV files
   - Loads OHLCV records into `Bar` structures
   - Handles malformed rows gracefully

2. **Technical Indicators** (`indicator.h/cpp`)
   - SMA (Simple Moving Average)
   - EMA (Exponential Moving Average)
   - RSI (Relative Strength Index)
   - Provides momentum and trend signals

3. **Feature Engineer** (`feature_engineer.h/cpp`)
   - Transforms raw price data into 16 predictive features
   - Handles train/test splitting
   - Manages feature configuration and naming

4. **Linear Regression Model** (`linear_regression.h/cpp`)
   - Ordinary Least Squares (OLS) implementation
   - Normal equation solver: β = (X^T X)^(-1) X^T y
   - Matrix operations (transpose, multiply, inverse)
   - Gaussian elimination with partial pivoting
   - Performance metrics (MSE, RMSE, R²)

5. **Predictor CLI** (`predictor.cpp`)
   - Command-line interface orchestrating the pipeline
   - Loads data → engineers features → trains model → evaluates → predicts
   - Displays coefficients, metrics, and sample predictions

---

## Data Flow Pipeline

```
Raw CSV (152 bars)
    ↓
[CSVLoader] Parse OHLCV records
    ↓
Bar objects (152 bars with date, open, high, low, close, volume)
    ↓
[FeatureEngineer] Compute technical features from sliding windows
    ↓
Feature matrix (101 samples × 16 features) + Targets (101 prices)
    ↓
[train_test_split] Chronological 80/20 split
    ↓
train_X (80×16)  +  train_y (80×1)
test_X  (21×16)  +  test_y  (21×1)
    ↓
[LinearRegression::train] Add intercept column → X (80×17)
    ↓
[Normal Equation] β = (X^T X)^(-1) X^T y
    ↓
Coefficients β (17×1) = [intercept, 16 feature weights]
    ↓
[predict] For each test sample: ŷ = β₀ + Σ(β_j × x_j)
    ↓
[evaluate] Compute MSE, RMSE, R² on test set
    ↓
Display results and predictions
```

---

## Feature Engineering

### 16 Features Generated Per Sample

#### 1. Price Returns (5 features)
```cpp
return_lag_1 through return_lag_5
```
- Percentage price changes over last 5 days
- Captures momentum and trend direction
- Formula: `(close[i] - close[i-lag]) / close[i-lag]`

#### 2. Normalized Lagged Prices (5 features)
```cpp
price_lag_1_norm through price_lag_5_norm
```
- Past prices relative to current price
- Detects mean-reversion patterns
- Formula: `close[i-lag] / close[i]`

#### 3. Technical Indicators (3 features)
```cpp
sma_20_norm   // 20-day Simple Moving Average / current_close
ema_12_norm   // 12-day Exponential Moving Average / current_close
rsi_14_norm   // 14-day Relative Strength Index / 100
```
- Trend identification and momentum strength
- Normalized for scale invariance

#### 4. Volume Features (2 features)
```cpp
volume_change      // Day-over-day volume % change
volume_ratio_5d    // Current volume / 5-day average volume
```
- Confirms price movements with trading activity
- Detects unusual market interest

#### 5. Volatility (1 feature)
```cpp
volatility_5d      // Rolling 5-day standard deviation of returns
```
- Measures price uncertainty and risk
- Indicates stability vs high-variation periods

---

## Mathematical Foundation

### Model Equation
```
ŷ = β₀ + β₁x₁ + β₂x₂ + ... + β₁₆x₁₆
```

Where:
- ŷ = predicted next-day closing price
- β₀ = intercept (learned constant offset)
- β₁...β₁₆ = feature weights (learned coefficients)
- x₁...x₁₆ = feature values for a given sample

### Training: Normal Equation

**Objective:** Minimize sum of squared errors
```
Loss = Σ(y_i - ŷ_i)²
```

**Solution:**
```
β = (X^T X)^(-1) X^T y
```

**Step-by-step:**
1. Build design matrix X (n × 17) with intercept column of 1s
2. Transpose X → X^T (17 × n)
3. Multiply X^T × X → (17 × 17) symmetric matrix
4. Invert (X^T X) using Gaussian elimination
5. Compute X^T × y → (17 × 1) vector
6. Final multiplication → β (17 × 1) coefficient vector

**Key Properties:**
- One-shot analytical solution (no iteration)
- Finds global minimum of squared error
- Exact for linear models
- Complexity: O(p³) for p features (p=17 here)

### Prediction

For new sample with features [x₁, x₂, ..., x₁₆]:
```cpp
ŷ = coefficients[0] +           // intercept
    coefficients[1] * x₁ +       // return_lag_1 weight × value
    coefficients[2] * x₂ +       // return_lag_2 weight × value
    ...
    coefficients[16] * x₁₆       // volatility_5d weight × value
```

### Evaluation Metrics

**MSE (Mean Squared Error):**
```
MSE = (1/n) Σ(y_i - ŷ_i)²
```
Units: price² (e.g., dollars²)

**RMSE (Root Mean Squared Error):**
```
RMSE = √MSE
```
Units: price (e.g., dollars) — interpretable as typical prediction error

**R² (Coefficient of Determination):**
```
R² = 1 - (SS_res / SS_tot)
   = 1 - [Σ(y_i - ŷ_i)²] / [Σ(y_i - ȳ)²]
```
Range: [0, 1] (can be negative if model is worse than mean)
- 1.0 = perfect predictions
- 0.0 = model no better than predicting the mean
- Interpretation: % of variance explained by features

---

## Implementation Details

### Matrix Operations

**Transpose**
```cpp
result[j][i] = matrix[i][j]
```
Swaps rows and columns.

**Multiply (Matrix × Matrix)**
```cpp
result[i][j] = Σ_k A[i][k] * B[k][j]
```
Standard triple-loop multiplication.

**Multiply (Matrix × Vector)**
```cpp
result[i] = Σ_j A[i][j] * b[j]
```
Row-wise dot products.

**Inverse (Gaussian Elimination)**
```cpp
// Create augmented matrix [A | I]
// Forward elimination with partial pivoting
// Back substitution
// Extract [I | A^(-1)]
```
- Partial pivoting for numerical stability
- Throws exception if matrix is singular
- Complexity: O(n³) for n×n matrix

### Feature Warmup

Early bars (first ~50) are skipped because:
- SMA/EMA need history to stabilize
- RSI requires sufficient data for gain/loss smoothing
- Lag features need lookback windows

`FeatureEngineer::create_features()` starts at:
```cpp
start_idx = max(config.lag_days, 50)
```

### NaN Handling

Samples with any NaN feature value are dropped:
```cpp
for (double val : feature_vec) {
    if (isnan(val)) { has_nan = true; break; }
}
if (!has_nan) {
    features.push_back(feature_vec);
    targets.push_back(target_price);
}
```

Protects linear algebra operations from numerical issues.

---

## Performance Results

### Typical Metrics (152-bar dataset)

**Data Split:**
- Total bars: 152
- Usable samples: 101 (after warmup and horizon)
- Training: 80 samples
- Testing: 21 samples

**Training Set:**
- MSE: 2.80
- RMSE: $1.67
- R²: 93.04%

**Test Set:**
- MSE: 9.48
- RMSE: $3.08
- R²: 87.70%

**Interpretation:**
- Model explains 87.7% of price variance on unseen data
- Typical prediction error: ~$3 on ~$100 stock (3%)
- Slight overfitting (5.3% R² drop from train to test) — acceptable
- Generalization is good

---

## Usage

### Build
```powershell
cd X:\Projects\Personal\SP
.\build.ps1
```

### Run
```powershell
.\build\Release\predictor.exe <csv_path> [prediction_days] [train_ratio]
```

**Example:**
```powershell
.\build\Release\predictor.exe .\data\stock_data.csv 1 0.8
```

**Arguments:**
- `csv_path`: Path to OHLCV CSV file (required)
- `prediction_days`: Forecast horizon in days (default: 1)
- `train_ratio`: Training set fraction (default: 0.8 = 80%)

### Output

1. **Configuration summary**
2. **Data loading stats** (bars loaded, date range)
3. **Feature engineering info** (samples, features, names)
4. **Train/test split sizes**
5. **Learned coefficients** (intercept + 16 weights)
6. **Training metrics** (MSE, RMSE, R²)
7. **Test metrics** (MSE, RMSE, R²)
8. **Sample predictions** (first 10 test samples with errors)
9. **Latest prediction** (most recent test sample)

---

## Design Decisions

### Why Linear Regression?
- **Simplicity:** Interpretable, no hyperparameter tuning
- **Transparency:** Can inspect feature weights to understand model
- **Speed:** Trains instantly on small datasets
- **Baseline:** Good starting point before trying complex models

### Why Normal Equation (vs Gradient Descent)?
- **Exact solution:** No convergence issues or learning rate tuning
- **Fast for small p:** 17 features → (17×17) inversion is trivial
- **Deterministic:** Same result every run
- **Educational:** Teaches matrix algebra fundamentals

### Why 16 Features?
- Proven in technical analysis (returns, MAs, RSI, volume)
- Balance between information and overfitting
- Small enough to avoid multicollinearity issues
- Large enough to capture multiple market dynamics

### Why No External ML Libraries?
- **Learning goal:** Understand internals, not just API calls
- **Portability:** No dependencies (pure C++17 stdlib)
- **Control:** Full visibility into every operation
- **Performance:** No framework overhead

---

## Limitations & Future Enhancements

### Current Limitations

1. **Linear assumptions:** Real price relationships may be nonlinear
2. **No regularization:** Ridge/Lasso could improve generalization
3. **Fixed feature set:** No automated feature selection
4. **Single horizon:** Only predicts 1 day ahead
5. **No external data:** Ignores market indices, news, sentiment
6. **Small dataset:** Only 152 bars (limited generalization)

### Potential Improvements

**Model:**
- Add L2 regularization (Ridge regression) to reduce overfitting
- Implement gradient descent alternative for comparison
- Try polynomial features (x², x³) for nonlinear patterns
- Ensemble multiple models (bagging)

**Features:**
- MACD (Moving Average Convergence Divergence)
- Bollinger Bands
- ATR (Average True Range)
- Market correlation features (S&P 500 returns)
- Sentiment indicators (VIX, news scores)

**Engineering:**
- Feature standardization (z-score normalization)
- Adaptive lag selection (cross-validation)
- Rolling window validation (walk-forward)
- Outlier detection and handling

**Infrastructure:**
- Real-time data ingestion
- Database storage for historical data
- Web API for predictions
- Backtesting framework with transaction costs

---

## Code Statistics

**Lines of Code:**
- Total: ~1,500 LOC (excluding comments/blanks)
- Headers: ~300 LOC
- Implementation: ~1,200 LOC

**Key Files:**
- `linear_regression.cpp`: 280 LOC (matrix ops + OLS)
- `feature_engineer.cpp`: 250 LOC (feature extraction)
- `predictor.cpp`: 200 LOC (CLI orchestration)
- `indicator.cpp`: 180 LOC (SMA/EMA/RSI)
- `csv_loader.cpp`: 100 LOC (parsing)

**Dependencies:**
- C++17 standard library only
- No external dependencies

---

## Key Takeaways

### What This Project Demonstrates

✅ **Machine Learning Fundamentals**
- Supervised learning pipeline
- Feature engineering from raw data
- Train/test evaluation methodology
- Performance metrics interpretation

✅ **Linear Algebra in Practice**
- Matrix operations implementation
- Normal equation solver
- Gaussian elimination
- Numerical stability considerations

✅ **Software Engineering**
- Modular architecture
- Separation of concerns
- Clean interfaces
- Build automation

✅ **Domain Knowledge**
- Technical analysis indicators
- Market microstructure (OHLCV)
- Time series patterns
- Trading concepts

### Educational Value

This project teaches:
1. How ML models work **under the hood**
2. Why matrix math powers modern AI
3. Feature engineering as the critical ML step
4. Importance of evaluation and validation
5. Trade-offs between simplicity and sophistication

---

## Conclusion

This stock price predictor successfully demonstrates multiple linear regression applied to financial time series. With 88% test R² and ~3% RMSE, the model captures short-term price patterns using engineered features from technical analysis.

While not suitable for real trading (markets are more complex than 16 features can capture), the project serves as an excellent learning platform for understanding:
- End-to-end ML pipeline construction
- Matrix mathematics in regression
- Feature engineering importance
- Model evaluation best practices

The clean C++ implementation with no external dependencies makes it an ideal reference for anyone learning how linear regression really works behind the sklearn/TensorFlow APIs.

---

**Project Status:** Complete and functional  
**Language:** C++17  
**Build System:** CMake + PowerShell helper  
**License:** MIT (or your choice)  
**Author:** ShadowMonarch71  
**Last Updated:** November 29, 2025
