#pragma once
#include "csv_loader.h"
#include <vector>

struct Trade { std::string entry_date, exit_date; double entry_price, exit_price; int size; double pnl; };

class Backtester {
public:
    Backtester(double slippage = 0.0, double cost = 0.0);
    void run(const std::vector<Bar>& bars, const std::vector<int>& signals);
    const std::vector<double>& equity() const { return equity_; }
    const std::vector<Trade>& trades() const { return trades_; }
private:
    double slippage_, cost_;
    std::vector<double> equity_;
    std::vector<Trade> trades_;
};
