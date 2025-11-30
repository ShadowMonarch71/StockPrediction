# SP Predictor (C++)

This repository contains a lightweight, educational C++17 implementation of a stock price predictor using multiple linear regression and simple feature engineering. It is dependency-free (standard library only) and cross-platform via CMake.

## Features

- **Linear Regression Model**: Ordinary least squares implementation
- **16 Predictive Features**: Returns, lagged prices, SMA/EMA/RSI, volume stats, volatility
- **Train/Test Split**: Evaluation metrics (MSE, RMSE, R²)
- **CSV Ingestion**: Simple OHLCV data format (Date,Open,High,Low,Close,Volume)

## Quick Start

```powershell
mkdir build
cd build
cmake ..
cmake --build . --config Release
cd ..
.\build\Release\predictor.exe data\stock_data.csv 1 0.8
```

## Output Files

After running predictions, you'll find in the `output/` directory:

**Data Files:**
- `predictions.csv` - All test predictions with actual vs predicted values
- `metrics.txt` - Model performance metrics

## Requirements

- CMake 3.10+
- C++17 compatible compiler
- Visual Studio 2022 (Windows) or GCC/Clang (Linux/Mac)

## Documentation

- **[Quick Reference](markdownFiles/QUICK_REFERENCE.md)** - API and usage reference
- **[Implementation Summary](markdownFiles/IMPLEMENTATION_SUMMARY.md)** - Technical details

## Usage Examples

### Basic Prediction
```powershell
.\build\Release\predictor.exe data\stock_data.csv
```

### Multi-day Prediction
```powershell
.\build\Release\predictor.exe data\stock_data.csv 5
```

### Custom Train/Test Split
```powershell
.\build\Release\predictor.exe data\stock_data.csv 1 0.85
```

Repository

Remote: https://github.com/ShadowMonarch71/SP

License

This project is released under the MIT License. See the `LICENSE` file for details.

Notes
- The code is intentionally simple and educational. Contributions and improvements are welcome.

