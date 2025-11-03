# Simple Strategy (C++)

This is a beginner-friendly C++ project that implements a simple rule-based trading pipeline:

- CSV loader (Date,Open,High,Low,Close,Volume)
- Indicators: SMA, EMA, RSI, MACD
- Rule-based signal: EMA(short) > SMA(long)
- Simple backtester (enter at next open, exit when rule no longer holds)
- Basic metrics output

Build with CMake and run:

```
mkdir build; cd build
cmake ..
cmake --build .
./strategy ../data/sample.csv
```
