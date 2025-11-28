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
cmake --build .
.\build\strategy.exe ..\data\sample.csv 50 20
```

Repository

Remote: https://github.com/ShadowMonarch71/SP

License

This project is released under the MIT License. See the `LICENSE` file for details.

Notes
- The code is intentionally simple and educational. Contributions and improvements are welcome.

