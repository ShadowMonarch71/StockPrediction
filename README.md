# SP Predictor (C++)

This repository contains a lightweight, educational C++17 implementation of a stock price predictor using multiple linear regression and simple feature engineering. It is dependency-free (standard library only) and cross-platform via CMake.

## Features

- Linear regression model (ordinary least squares)
- 16 predictive features (returns, lagged prices, SMA/EMA/RSI, volume stats, volatility)
- Train/test split with evaluation metrics (MSE, RMSE, R²)
- Simple CSV ingestion for OHLCV data (Date,Open,High,Low,Close,Volume)

## Quick Start

### Build

```powershell
mkdir build; cd build
cmake ..
cmake --build . --config Release
```

### Run Predictor

```powershell
# Predict next day's price using 80% training split
.\Release\predictor.exe ..\data\stock_data.csv 1 0.8

# Predict 5 days ahead with 70% training split
.\Release\predictor.exe ..\data\stock_data.csv 5 0.7
```

Notes:
- The predictor expects 6-column OHLCV CSVs. If your file has an AdjClose column, convert to `stock_data.csv` with columns `Date,Open,High,Low,Close,Volume`.
- Larger datasets (100+ rows) yield more stable models.

## Project Structure

```
src/
├── predictor.cpp         # Predictor CLI
├── linear_regression.*   # ML model
├── feature_engineer.*    # Feature extraction
├── csv_loader.*          # Data loading
└── indicator.*           # Technical indicators (used as features)

tests/
└── predictor_tests.cpp   # Predictor unit tests
```

Repository

Remote: https://github.com/ShadowMonarch71/SP

License

This project is released under the MIT License. See the `LICENSE` file for details.

Notes
- The code is intentionally simple and educational. Contributions and improvements are welcome.

