# Quick Reference - Stock Price Predictor

## Build Commands

```powershell
# Clean build
Remove-Item -Recurse -Force build -ErrorAction SilentlyContinue
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

## Run Commands
## To build the model and run
cd X:\Projects\Personal\SP
Remove-Item -Recurse -Force build -ErrorAction SilentlyContinue
mkdir build
cd build
cmake ..
cmake --build . --config Release
.\Release\predictor.exe ..\data\stock_data.csv 1 0.8

## To build and run
cd X:\Projects\Personal\SP
.\build.ps1 -Run

# OR simply build and run manually:
.\build.ps1
.\build\Release\predictor.exe data\stock_data.csv 1 0.8


## To run the model
.\build\Release\predictor.exe data\stock_data.csv 1 0.8


### Stock Price Predictor (NEW)
```powershell
# Basic usage - predict next day
.\Release\predictor.exe ..\data\ohlcv_100_days_style.csv

# Predict 5 days ahead
.\Release\predictor.exe ..\data\ohlcv_100_days_style.csv 5

# Custom train/test split (70% training)
.\Release\predictor.exe ..\data\ohlcv_100_days_style.csv 1 0.7
```

### Backtester (Original)
```powershell
# Run backtester with default parameters
.\Release\strategy.exe ..\data\ohlcv_100_days_style.csv

# Custom SMA/EMA periods
.\Release\strategy.exe ..\data\ohlcv_100_days_style.csv 50 20
```

### Tests
```powershell
# Run all tests
ctest -C Release

# Run specific tests
.\Release\predictor_tests.exe
.\Release\indicator_tests.exe
```

## File Locations

### Source Files
- **Predictor:** `src/predictor.cpp`
- **Linear Regression:** `src/linear_regression.{h,cpp}`
- **Feature Engineering:** `src/feature_engineer.{h,cpp}`
- **Backtester:** `src/main.cpp`, `src/backtester.{h,cpp}`

### Data Files
- `data/sample.csv` - Small dataset (16 bars) - too small for ML
- `data/ohlcv_100_days_style.csv` - 100 bars - good for ML

### Documentation
- `README.md` - Main project overview
- `PREDICTOR_README.md` - Detailed predictor documentation
- `IMPLEMENTATION_SUMMARY.md` - Technical implementation details
- `PROJECT_REPORT.md` - Comprehensive project analysis

## Key Features

### Predictor Features (16 total)
1-5. **Returns:** `return_lag_1` to `return_lag_5`
6-10. **Lagged Prices:** `price_lag_1_norm` to `price_lag_5_norm`
11. **SMA:** `sma_20_norm`
12. **EMA:** `ema_12_norm`
13. **RSI:** `rsi_14_norm`
14. **Volume Change:** `volume_change`
15. **Volume Ratio:** `volume_ratio_5d`
16. **Volatility:** `volatility_5d`

## Performance Metrics

| Metric | Meaning | Good Value |
|--------|---------|------------|
| **MSE** | Mean Squared Error | Lower is better (closer to 0) |
| **RMSE** | Root Mean Squared Error | Lower is better (in price units) |
| **R²** | Variance explained | Higher is better (closer to 1.0) |

## Common Issues & Solutions

### Issue: "Not enough data to create features"
**Solution:** Use dataset with 100+ bars. `sample.csv` is too small.

### Issue: "Model training failed"
**Solution:** Dataset may be too small or have too many NaN values. Try larger dataset.

### Issue: CMake cache errors
**Solution:** Delete build directory completely and rebuild:
```powershell
Remove-Item -Recurse -Force build
mkdir build
cd build
cmake ..
```

## Project Structure

```
SP/
├── src/                      # Source code
│   ├── predictor.cpp         # 🆕 ML prediction CLI
│   ├── linear_regression.*   # 🆕 ML model
│   ├── feature_engineer.*    # 🆕 Feature extraction
│   ├── main.cpp              # Backtester CLI
│   ├── backtester.*          # Backtest engine
│   ├── strategy.*            # Trading strategies
│   ├── indicator.*           # Technical indicators
│   ├── csv_loader.*          # Data loading
│   └── metrics.*             # Performance metrics
├── tests/                    # Unit tests
│   ├── predictor_tests.cpp   # 🆕 ML tests
│   └── indicator_tests.cpp   # Indicator tests
├── data/                     # Sample datasets
├── docs/                     # Documentation
│   ├── architecture.mmd      # Original flowchart
│   └── ml_architecture.mmd   # 🆕 ML architecture
└── build/                    # Build outputs
    └── Release/
        ├── predictor.exe     # 🆕 Price predictor
        ├── strategy.exe      # Backtester
        ├── predictor_tests.exe  # 🆕 ML tests
        └── indicator_tests.exe  # Indicator tests
```

## Customization

### Change Feature Configuration
Edit `src/predictor.cpp` line ~50:
```cpp
FeatureConfig config;
config.lag_days = 5;        // Number of historical days
config.sma_period = 20;     // SMA lookback
config.ema_period = 12;     // EMA lookback
config.rsi_period = 14;     // RSI lookback
```

### Add New Features
Edit `src/feature_engineer.cpp`:
1. Add feature extraction in `create_features()`
2. Update `get_feature_count()`
3. Update `get_feature_names()`

## Troubleshooting

### Build Issues
```powershell
# Ensure you're in the build directory
cd X:\Projects\Personal\SP\build

# Check CMake version
cmake --version  # Should be 3.10+

# Check compiler
cl  # Should show MSVC compiler
```

### Runtime Issues
```powershell
# Check data file exists
Test-Path ..\data\ohlcv_100_days_style.csv

# Count rows in CSV
(Get-Content ..\data\ohlcv_100_days_style.csv).Count
# Should be 101 (100 data rows + 1 header)
```

## Next Steps

1. **Try the predictor:**
   ```powershell
   .\Release\predictor.exe ..\data\ohlcv_100_days_style.csv
   ```

2. **Run tests:**
   ```powershell
   .\Release\predictor_tests.exe
   ```

3. **Compare with backtester:**
   ```powershell
   .\Release\strategy.exe ..\data\ohlcv_100_days_style.csv
   ```

4. **Read documentation:**
   - `PREDICTOR_README.md` - How to use the predictor
   - `IMPLEMENTATION_SUMMARY.md` - Technical details

## Getting Help

- Check `PREDICTOR_README.md` for detailed documentation
- Review `IMPLEMENTATION_SUMMARY.md` for implementation details
- Examine test files in `tests/` for usage examples
- Read inline comments in source files

---

**Quick Tip:** The predictor needs more data than the backtester. Always use datasets with 100+ bars for machine learning!
