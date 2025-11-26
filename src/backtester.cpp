/*
 * backtester.cpp
 *
 * Implements a simple long-only backtester. Responsibilities:
 *  - Accept a time series of Bars and a parallel vector of integer signals
 *    (1 = long/hold, 0 = flat)
 *  - Execute entry orders at the next bar's open when a signal turns to 1
 *  - Execute exit orders at the next bar's open when a signal turns to 0
 *  - Track cash, position (number of shares), and the per-bar equity curve
 *  - Record completed trades (entry date, exit date, entry/exit prices, pnl)
 *  - Apply fixed slippage (fractional) and fixed per-trade cost
 *  - Force-close any open position at the end of the dataset, recording it as a trade
 *
 * Notes / Assumptions:
 *  - The backtester uses an "all-in" sizing: when entering it invests the entire
 *    available cash into the position (position = cash / entry_price).
 *  - Entry and exit are both executed at the next bar's open price. We apply
 *    slippage by pushing the executed price slightly up on entry and down on exit.
 *  - Per-trade fixed cost (cost_) is subtracted from proceeds when exiting.
 *  - We store an equity vector sized to the number of bars; uninitialized
 *    entries are filled forward to avoid zero artifacts in metrics.
 *  - PnL for a trade is computed as (proceeds - amount_invested_at_entry).
 *
 * Limitations / To improve:
 *  - No shorting, no partial position sizing, no leverage, no margin.
 *  - No order book, market impact, or limit order handling.
 *  - Uses simple next-bar open execution; users may want configurable entry/exit prices.
 */

#include "backtester.h"
#include <iostream>

using namespace sp;

Backtester::Backtester(double slippage, double cost) : slippage_(slippage), cost_(cost) {}

void Backtester::run(const std::vector<Bar>& bars, const std::vector<int>& signals) 
{
    // number of bars
    size_t n = bars.size();

    // prepare equity vector and clear any previous trades
    equity_.assign(n, 0.0);
    trades_.clear();

    // initial portfolio state: start with 1.0 unit of cash (normalized)
    double cash = 1.0;
    double position = 0.0;            // number of shares held
    double entry_invested = 0.0;      // cash invested when entering a position
    double entry_price = 0.0;        // executed entry price
    std::string entry_date;           // date string of entry

    // Iterate bars starting at 1 because entries/exits are executed on the next bar
    for (size_t i = 1; i < n; ++i) 
    {
        // ENTRY: previous signal was 1 (enter/hold) and we have no current position
        if (signals[i-1] == 1 && position == 0.0) 
        {
            // execute at today's open with slippage applied (worse price for us)
            entry_price = bars[i].open * (1.0 + slippage_);

            // invest all available cash
            position = cash / entry_price;
            entry_invested = cash;
            cash = 0.0;
            entry_date = bars[i].date;
        }

        // EXIT: previous signal was 0 and we currently hold a position
        if (signals[i-1] == 0 && position > 0.0) 
        {
            // execute exit at today's open with slippage (slightly worse price)
            double exit_price = bars[i].open * (1.0 - slippage_);

            // proceeds after selling position and subtracting fixed cost
            double proceeds = position * exit_price - cost_;

            // profit/loss relative to the cash originally invested
            double pnl = proceeds - entry_invested;

            // record trade
            Trade t{entry_date, bars[i].date, entry_price, exit_price, (int)position, pnl};
            trades_.push_back(t);

            // update cash and clear position
            cash = proceeds;
            position = 0.0;
            entry_invested = 0.0;
        }

        // mark equity for this bar: cash plus market value of any open position
        double cur = cash + position * bars[i].close;
        equity_[i] = cur;
    }

    // ensure equity[0] reflects initial cash and fill forward any uninitialized entries
    if (n > 0) equity_[0] = 1.0;
    for (size_t i = 1; i < n; ++i) {
        if (equity_[i] == 0.0) equity_[i] = equity_[i-1];
    }

    // If still in a position at the end of the series, force-close at the final close price.
    // This ensures open positions are turned into recorded trades and final equity is concrete.
    if (position > 0.0) {
        double exit_price = bars.back().close * (1.0 - slippage_);
        double proceeds = position * exit_price - cost_;
        double pnl = proceeds - entry_invested;
        Trade t{entry_date, bars.back().date, entry_price, exit_price, (int)position, pnl};
        trades_.push_back(t);
        cash = proceeds;
        position = 0.0;

        // update final equity value with the cash we now hold
        if (!equity_.empty()) equity_.back() = cash;
    }
}
