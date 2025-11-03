# Simple Strategy (C++)

This is a beginner-friendly C++ project that implements a simple rule-based trading pipeline.

Features
- CSV loader (Date,Open,High,Low,Close,Volume)
- Indicators: SMA, EMA, RSI, MACD
- Rule-based signal: EMA(short) > SMA(long)
- Simple backtester (enter at next open, exit when rule no longer holds)
- Basic metrics output

Build & run (using g++ / MinGW or a POSIX shell):

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

