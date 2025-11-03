#pragma once
#include <vector>
#include "backtester.h"

struct Metrics {
    double final_equity = 1.0;
    double max_drawdown = 0.0;
    int trades = 0;
    int wins = 0;
};

Metrics compute_metrics(const std::vector<double>& equity, const std::vector<Trade>& trades);
