// technical indicators like SMA, EMA, RSI, MACD for price analysis

#include "indicator.h"
#include <cmath>
#include <numeric>

using namespace sp;

// simple moving average - just averages last N prices
std::vector<double> SMAIndicator::compute(const std::vector<double>& x) {
    std::vector<double> out(x.size(), NAN);
    if (period_ <= 0) return out;
    double sum = 0.0;
    for (std::size_t i = 0; i < x.size(); ++i) {
        sum += x[i];
        if (i >= static_cast<std::size_t>(period_))
            sum -= x[i - period_];
        if (i + 1 >= static_cast<std::size_t>(period_))
            out[i] = sum / period_;
    }
    return out;
}

// exponential moving average - gives more weight to recent prices
std::vector<double> EMAIndicator::compute(const std::vector<double>& x) {
    std::vector<double> out(x.size(), NAN);
    if (period_ <= 0 || x.empty()) return out;
    double alpha = 2.0 / (period_ + 1);
    double prev = x[0];
    out[0] = prev;
    for (std::size_t i = 1; i < x.size(); ++i) {
        prev = alpha * x[i] + (1 - alpha) * prev;
        out[i] = prev;
    }
    return out;
}

// RSI - shows if stock is overbought or oversold (0-100 range)
std::vector<double> RSIIndicator::compute(const std::vector<double>& x) {
    std::vector<double> out(x.size(), NAN);
    if (period_ <= 0 || x.size() < 2) return out;
    std::vector<double> gains(x.size(), 0.0), losses(x.size(), 0.0);
    for (std::size_t i = 1; i < x.size(); ++i) {
        double diff = x[i] - x[i - 1];
        gains[i] = std::max(0.0, diff);
        losses[i] = std::max(0.0, -diff);
    }
    double avg_gain = 0.0, avg_loss = 0.0;
    for (int i = 1; i <= period_ && i < (int)gains.size(); ++i) {
        avg_gain += gains[i];
        avg_loss += losses[i];
    }
    avg_gain /= period_;
    avg_loss /= period_;
    if (period_ < (int)gains.size()) {
        out[period_] = 100.0 - (100.0 / (1.0 + avg_gain / (avg_loss == 0 ? 1e-12 : avg_loss)));
    }
    for (std::size_t i = period_ + 1; i < x.size(); ++i) {
        avg_gain = (avg_gain * (period_ - 1) + gains[i]) / period_;
        avg_loss = (avg_loss * (period_ - 1) + losses[i]) / period_;
        double rs = avg_gain / (avg_loss == 0 ? 1e-12 : avg_loss);
        out[i] = 100.0 - (100.0 / (1.0 + rs));
    }
    return out;
}

// MACD - difference between fast and slow EMA, helps spot trend changes
std::vector<double> MACDIndicator::compute(const std::vector<double>& prices) {
    EMAIndicator fastE(fast_);
    EMAIndicator slowE(slow_);
    auto f = fastE.compute(prices);
    auto s = slowE.compute(prices);
    std::vector<double> macd(prices.size(), NAN);
    for (std::size_t i = 0; i < prices.size(); ++i) {
        if (!std::isnan(f[i]) && !std::isnan(s[i]))
            macd[i] = f[i] - s[i];
    }
    return macd;
}
