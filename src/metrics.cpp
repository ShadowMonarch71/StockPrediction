/*
 * metrics.cpp
 *
 * Small utilities for computing simple performance metrics from the backtest
 * outputs. The goal is clarity for learners rather than a complete analytics
 * suite. The functions operate on the equity curve (vector of portfolio values)
 * and the recorded trades.
 *
 * Metrics provided:
 *  - final_equity: the last non-zero entry of the equity vector (or 1.0 if
 *    equity is empty or uninitialized). This represents total portfolio value
 *    relative to the starting capital (1.0 == starting capital).
 *  - max_drawdown: the maximum peak-to-trough loss observed on the equity
 *    curve. Expressed as a fraction (0.2 == 20% drawdown).
 *  - trades: total number of recorded trades.
 *  - wins: number of trades with positive PnL.
 *
 * Notes and assumptions:
 *  - The equity vector may contain zeros for uninitialized slots; zeros are
 *    skipped when calculating peaks and drawdowns.
 *  - This file intentionally keeps calculations simple. More advanced
 *    measures (CAGR, volatility, Sharpe) can be added later and will require
 *    either timestamps or a fixed-bars-per-year assumption to annualize.
 */

#include "metrics.h"
#include <algorithm>

Metrics compute_metrics(const std::vector<double>& equity, const std::vector<Trade>& trades) {
    Metrics m;

    // Final equity: take the last non-zero value if present, otherwise 1.0
    if (!equity.empty()) m.final_equity = equity.back() == 0.0 ? 1.0 : equity.back();

    // Compute max drawdown using a running peak. Skip zero entries which are
    // used by the backtester to indicate uninitialized slots.
    double peak = (equity.empty() ? 1.0 : equity.front());
    double maxdd = 0.0;
    for (double v : equity) {
        if (v <= 0.0) continue; // skip uninitialized zeros
        if (v > peak) peak = v; // new peak
        double dd = (peak - v) / peak; // drawdown from current peak
        if (dd > maxdd) maxdd = dd;
    }
    m.max_drawdown = maxdd;

    // Trade counts & win count
    m.trades = static_cast<int>(trades.size());
    m.wins = 0;
    for (auto &t : trades) if (t.pnl > 0) m.wins++;
    return m;
}
