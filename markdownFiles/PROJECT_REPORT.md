# Simple Strategy (SP) - Project Report

**Generated:** November 28, 2025  
**Repository:** https://github.com/ShadowMonarch71/SP  
**Branch:** main  
**License:** MIT

---

## Executive Summary

Simple Strategy (SP) is an educational C++ backtesting framework designed for beginners learning algorithmic trading concepts. The project implements a complete trading pipeline from data ingestion to performance reporting, featuring rule-based strategies using technical indicators. With approximately 809 lines of well-documented code across 13 source files, it serves as an excellent introduction to systematic trading development.

---

## Project Overview

### Purpose
This project provides a minimal, easy-to-understand implementation of a backtesting system that allows users to:
- Load historical OHLCV (Open, High, Low, Close, Volume) market data from CSV files
- Calculate technical indicators (SMA, EMA, RSI, MACD)
- Generate trading signals using rule-based strategies
- Simulate realistic trading with slippage and transaction costs
- Evaluate strategy performance using standard metrics

### Target Audience
- C++ beginners learning about algorithmic trading
- Students studying quantitative finance
- Developers looking for a simple backtesting framework to extend

---

## Technical Architecture

### Component Structure

```
┌─────────────┐
│ CSV Loader  │ → Historical Data (OHLCV Bars)
└─────────────┘
       ↓
┌─────────────┐
│ Indicators  │ → Technical Analysis Values
└─────────────┘   (SMA, EMA, RSI, MACD)
       ↓
┌─────────────┐
│  Strategy   │ → Trading Signals (0/1)
└─────────────┘   (Rule: EMA > SMA)
       ↓
┌─────────────┐
│ Backtester  │ → Equity Curve + Trade List
└─────────────┘   (Simulated Execution)
       ↓
┌─────────────┐
│   Metrics   │ → Performance Statistics
└─────────────┘   (Returns, Drawdown, Win Rate)
```

### Core Components

#### 1. **CSV Loader** (`csv_loader.h/cpp`)
- **Purpose:** Parse CSV files containing historical market data
- **Features:**
  - Header validation (Date, Open, High, Low, Close, Volume)
  - Robust error handling for malformed data
  - Returns vector of `Bar` structs
- **Data Format:** Expects standard OHLCV format with date strings

#### 2. **Indicators** (`indicator.h/cpp`)
- **Architecture:** Abstract base class with concrete implementations
- **Implemented Indicators:**
  - **SMA (Simple Moving Average):** Unweighted N-period average
  - **EMA (Exponential Moving Average):** Weighted average favoring recent prices
  - **RSI (Relative Strength Index):** Momentum oscillator (0-100 scale)
  - **MACD (Moving Average Convergence Divergence):** Trend-following indicator
- **Design Pattern:** Strategy pattern for indicator polymorphism
- **Output:** Vector of doubles (same length as input, NaN for warmup periods)

#### 3. **Strategy** (`strategy.h/cpp`)
- **Base Class:** Abstract `Strategy` interface
- **Implementation:** `RuleBasedStrategy`
  - Compares two indicators (fast vs. slow)
  - Generates signal = 1 when fast > slow, else 0
  - Current configuration: EMA(20) > SMA(50)
- **Extensibility:** Easy to add new strategy types by inheriting from `Strategy`

#### 4. **Backtester** (`backtester.h/cpp`)
- **Execution Logic:**
  - Long-only positions (no short selling)
  - All-in position sizing (100% capital deployment)
  - Signal 0→1: Enter at next bar's open price
  - Signal 1→0: Exit at next bar's open price
  - Automatic position closure at end of data
- **Realistic Costs:**
  - Configurable slippage (default: 0.05%)
  - Fixed transaction costs (default: $0)
- **Output:**
  - Equity curve (portfolio value at each timestep)
  - Trade list (entry/exit details, P&L per trade)

#### 5. **Metrics** (`metrics.h/cpp`)
- **Calculated Statistics:**
  - Final equity (total return)
  - Maximum drawdown (worst peak-to-trough decline)
  - Trade count (total trades executed)
  - Win count (profitable trades)
  - Implicit: Win rate = wins / trades

---

## Technical Implementation

### Language & Standards
- **Language:** C++
- **Standard:** C++17
- **Build System:** CMake 3.10+
- **Compiler Support:** MSVC, GCC, Clang

### Code Quality Features
- **Documentation:** Comprehensive inline comments explaining logic and purpose
- **Namespace:** All code organized under `sp` namespace
- **Memory Management:** Smart pointers (`std::shared_ptr`) for indicator ownership
- **Error Handling:** Try-catch blocks in main entry point
- **Const Correctness:** Proper use of const for read-only methods

### Project Statistics
- **Total Lines of Code:** 809 lines
- **Source Files:** 13 files (`.h` + `.cpp`)
- **Components:**
  - 6 header files
  - 6 implementation files
  - 1 test file
  - Build scripts and configuration

---

## Build & Deployment

### Build System
The project uses **CMake** for cross-platform compilation with two executables:

1. **`strategy.exe`** - Main backtesting application
   - Sources: main.cpp, csv_loader.cpp, indicator.cpp, strategy.cpp, backtester.cpp, metrics.cpp

2. **`indicator_tests.exe`** - Unit test suite
   - Sources: indicator_tests.cpp, indicator.cpp
   - Registered with CTest for automated testing

### Build Process
```powershell
# PowerShell build script included (build.ps1)
mkdir build; cd build
cmake ..
cmake --build .
.\build\strategy.exe ..\data\sample.csv 50 20
```

### Command-Line Interface
```
Usage: strategy <csv-path> [sma_period=50] [ema_period=20]

Arguments:
  csv-path    : Path to OHLCV CSV file (required)
  sma_period  : Long-period SMA (default: 50)
  ema_period  : Short-period EMA (default: 20)

Example:
  strategy data/sample.csv 50 20
```

---

## Testing Strategy

### Unit Tests (`tests/indicator_tests.cpp`)
Validates correctness of all indicator implementations:

1. **SMA Validation:**
   - Size consistency check
   - Arithmetic correctness (last value = avg of last N)

2. **EMA Validation:**
   - Seed value equals first price
   - Exponential smoothing behavior

3. **RSI Validation:**
   - Range check: all values ∈ [0, 100]
   - NaN handling for warmup period

4. **MACD Validation:**
   - Equals difference of fast and slow EMAs
   - Consistency with manual calculation

### Test Execution
```bash
cmake --build .
ctest  # Runs indicator_tests
```

---

## Data Flow Example

### Sample Execution Trace
Using `data/sample.csv` with EMA(20) and SMA(50):

1. **Load Data:** CSV → 100 bars of OHLCV data
2. **Compute Indicators:**
   - EMA(20) calculated for all bars
   - SMA(50) calculated (first 49 values are NaN)
3. **Generate Signals:**
   - Compare EMA vs SMA at each bar
   - Signal = 1 when EMA > SMA (bullish)
   - Signal = 0 otherwise (bearish/neutral)
4. **Execute Backtest:**
   - Start with $1.0 normalized capital
   - Enter position when signal flips 0→1
   - Exit position when signal flips 1→0
   - Apply 0.05% slippage on each trade
5. **Calculate Metrics:**
   - Final equity: e.g., 1.15 (+15% return)
   - Max drawdown: e.g., 0.08 (8% worst decline)
   - Trades: e.g., 12 trades executed
   - Wins: e.g., 7 profitable trades (58% win rate)

---

## Current Capabilities

### Strengths
✅ **Educational Focus:** Clear, well-commented code perfect for learning  
✅ **Complete Pipeline:** End-to-end from data to metrics  
✅ **Realistic Simulation:** Includes slippage and transaction costs  
✅ **Extensible Design:** Abstract interfaces for easy customization  
✅ **Cross-Platform:** Works on Windows, Linux, macOS  
✅ **Tested Components:** Unit tests for critical indicator logic  

### Limitations
⚠️ **Long-Only:** Cannot simulate short positions  
⚠️ **All-In Sizing:** No position sizing or risk management  
⚠️ **Single Asset:** No portfolio/multi-asset support  
⚠️ **Basic Metrics:** Limited performance statistics  
⚠️ **No Visualization:** Console output only, no charts  
⚠️ **CSV Only:** No support for databases or APIs  

---

## Extension Opportunities

### Recommended Enhancements

#### 1. **Position Sizing**
- Fixed fractional (e.g., 50% of capital per trade)
- Kelly criterion
- Volatility-based sizing

#### 2. **Advanced Metrics**
- Sharpe ratio
- Sortino ratio
- Calmar ratio
- Maximum adverse excursion (MAE)

#### 3. **Multiple Strategies**
- Mean reversion strategies
- Breakout strategies
- Multi-indicator combinations

#### 4. **Data Export**
- Trade list to CSV (`trades.csv`)
- Equity curve to CSV (`equity.csv`)
- JSON reporting format

#### 5. **Visualization**
- Integration with Python plotting libraries
- Web-based dashboard (C++ backend, JS frontend)
- Real-time equity curve during backtest

#### 6. **Parameter Optimization**
- Grid search for optimal indicator periods
- Walk-forward analysis
- Monte Carlo simulation

#### 7. **Additional Indicators**
- Bollinger Bands
- ATR (Average True Range)
- Stochastic oscillator
- Volume-weighted indicators

#### 8. **Risk Management**
- Stop-loss orders
- Take-profit targets
- Trailing stops
- Maximum drawdown limits

---

## Development Workflow

### File Organization
```
SP/
├── src/              # Core implementation
│   ├── main.cpp      # CLI entry point
│   ├── csv_loader.*  # Data ingestion
│   ├── indicator.*   # Technical indicators
│   ├── strategy.*    # Signal generation
│   ├── backtester.*  # Simulation engine
│   └── metrics.*     # Performance stats
├── tests/            # Unit tests
├── data/             # Sample CSV files
├── docs/             # Documentation
│   └── architecture.mmd  # System flowchart
├── build/            # Build artifacts (gitignored)
└── CMakeLists.txt    # Build configuration
```

### Coding Conventions
- **Headers:** Comprehensive file-level and function-level comments
- **Naming:** `snake_case` for functions/variables, `PascalCase` for classes
- **Includes:** Explicit dependencies, avoid circular includes
- **Memory:** Prefer stack allocation, use smart pointers when needed

---

## Performance Characteristics

### Computational Complexity
- **CSV Loading:** O(n) where n = number of bars
- **Indicator Calculation:** O(n) for each indicator
- **Signal Generation:** O(n)
- **Backtesting:** O(n) single-pass simulation
- **Overall:** O(n) linear time complexity

### Memory Usage
- **Bars:** ~72 bytes per bar (date string + 5 doubles)
- **Indicators:** ~8 bytes per value × number of indicators
- **Equity Curve:** ~8 bytes per bar
- **Trades:** ~120 bytes per trade (strings + doubles)

**Example:** For 1000 bars with 4 indicators:
- Bars: ~72 KB
- Indicators: ~32 KB
- Equity: ~8 KB
- **Total:** ~120 KB (excluding trade list)

### Scalability
- Can handle multi-year daily data (thousands of bars) easily
- For intraday data (millions of bars), consider:
  - Memory-mapped files
  - Streaming/chunked processing
  - Incremental indicator updates

---

## Dependencies

### Core Requirements
- **C++17 Compiler:** MSVC 19.14+, GCC 7+, Clang 5+
- **CMake:** 3.10 or higher
- **Standard Library:** Full STL support

### Optional Tools
- **Git:** Version control
- **PowerShell:** Build automation (Windows)
- **CTest:** Test execution

### No External Libraries
The project intentionally avoids dependencies to remain beginner-friendly and portable.

---

## Learning Outcomes

### Skills Developed
By studying/extending this project, developers learn:

1. **C++ Best Practices:**
   - Object-oriented design patterns
   - Smart pointer usage
   - Abstract interfaces and polymorphism
   - Const correctness

2. **Trading Concepts:**
   - Technical analysis fundamentals
   - Strategy backtesting methodology
   - Performance measurement
   - Trading cost modeling

3. **Software Engineering:**
   - Modular architecture design
   - Unit testing principles
   - Build system configuration (CMake)
   - Documentation standards

4. **Quantitative Finance:**
   - Time series analysis
   - Signal generation
   - Risk metrics (drawdown)
   - Trade execution simulation

---

## Use Cases

### Educational Scenarios
1. **CS Students:** Learn C++ through practical finance application
2. **Finance Students:** Understand algorithmic trading implementation
3. **Bootcamps:** Template for trading system assignments
4. **Self-Study:** Extend features as learning exercises

### Research Applications
1. **Indicator Testing:** Rapid prototyping of technical indicators
2. **Strategy Exploration:** Quick backtesting of simple rules
3. **Proof of Concept:** Validate ideas before production implementation

---

## Future Roadmap

### Short-Term (Next 3 Months)
- [ ] Add trade export to CSV
- [ ] Implement stop-loss functionality
- [ ] Add Sharpe ratio calculation
- [ ] Create Python plotting script for equity curves

### Medium-Term (3-6 Months)
- [ ] Parameter optimization framework
- [ ] Walk-forward testing capabilities
- [ ] Multi-asset portfolio support
- [ ] Integration with market data APIs

### Long-Term (6-12 Months)
- [ ] Real-time trading simulation
- [ ] Machine learning indicator integration
- [ ] Web-based visualization dashboard
- [ ] Database backend for historical data

---

## Contributing Guidelines

### How to Contribute
1. Fork the repository
2. Create feature branch (`git checkout -b feature/new-indicator`)
3. Implement changes with proper documentation
4. Add unit tests for new functionality
5. Ensure all tests pass (`ctest`)
6. Submit pull request with clear description

### Code Standards
- Follow existing naming conventions
- Add comprehensive comments to new functions
- Maintain C++17 compatibility
- Update README for user-facing changes

---

## License & Acknowledgments

### License
This project is released under the **MIT License**, allowing free use, modification, and distribution with attribution.

### Repository Information
- **Owner:** ShadowMonarch71
- **Repository:** SP (Simple Strategy)
- **GitHub:** https://github.com/ShadowMonarch71/SP

---

## Conclusion

Simple Strategy (SP) successfully achieves its goal of being an educational, minimal backtesting framework. With clean architecture, comprehensive documentation, and working test coverage, it provides an excellent foundation for learning both C++ and algorithmic trading. The project's extensible design makes it suitable for incremental enhancement as developers grow their skills.

**Overall Assessment:**
- ✅ Code Quality: Excellent (well-documented, clean structure)
- ✅ Educational Value: High (clear examples, simple concepts)
- ✅ Extensibility: Good (abstract interfaces, modular design)
- ⚠️ Production Readiness: Low (by design - educational focus)

**Recommendation:** Ideal for educational purposes and prototyping. For production trading, consider migrating proven strategies to enterprise frameworks like QuantLib or Zipline.

---

**Report Generated by:** GitHub Copilot  
**Date:** November 28, 2025  
**Project Version:** Current main branch snapshot
