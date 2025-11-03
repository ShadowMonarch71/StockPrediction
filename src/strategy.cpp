/*
 * strategy.cpp
 *
 * RuleBasedStrategy implements a tiny, easy-to-follow rule engine used by the
 * example program. The strategy composes two indicators (fast and slow) and
 * produces a per-bar signal vector aligned with the input bars.
 *
 * Contract (inputs / outputs):
 *  - Input: vector<Bar> (bars), where each Bar contains date, open, high,
 *    low, close, volume. Only close prices are read by the example strategy.
 *  - Output: vector<int> signals of the same length as bars. signal[i] is:
 *      - 1 meaning "long/buy" for that bar
 *      - 0 meaning "flat/no position"
 *
 * Behavior and design choices:
 *  - The strategy computes both indicators on the close price series and then
 *    compares their values index-by-index. We set warmup/unavailable values to
 *    NaN in the indicators; this function treats any NaN as an invalid
 *    comparison and emits a 0 signal for that bar.
 *  - The rule used in the reference implementation is: when fastIndicator >
 *    slowIndicator, emit 1; otherwise 0. This is intentionally simple so a
 *    beginner can follow the data flow.
 *  - Signals are aligned to the same index as bars. The backtester decides how
 *    to act on transitions between consecutive signals (e.g., enter on signal
 *    rising to 1, exit on falling to 0).
 *
 * Edge cases and notes:
 *  - Missing data (empty bars) -> returns empty signal vector.
 *  - If indicators return NaN for an index (warmup), the strategy emits 0.
 *  - This function is stateless and thread-safe (reads only const data and
 *    indicator instances must be safe for concurrent const use).
 */

#include "strategy.h"
#include "indicator.h"
#include <cmath>

RuleBasedStrategy::RuleBasedStrategy(std::shared_ptr<Indicator> shortEma, std::shared_ptr<Indicator> longSma)
    : shortEma_(shortEma), longSma_(longSma) {}

std::vector<int> RuleBasedStrategy::generate_signals(const std::vector<Bar>& bars) {
    size_t n = bars.size();
    if (n == 0) return {};

    // Extract close prices into a simple vector the indicators accept.
    std::vector<double> close(n);
    for (size_t i = 0; i < n; ++i) close[i] = bars[i].close;

    // Compute indicators (may contain NaN for warmup periods).
    auto e = shortEma_->compute(close);
    auto s = longSma_->compute(close);

    // Build the integer signal vector. Default is 0 (flat).
    std::vector<int> signal(n, 0);
    for (size_t i = 0; i < n; ++i) {
        // Only emit a long signal when both indicator values are available.
        if (!std::isnan(e[i]) && !std::isnan(s[i]) && e[i] > s[i]) signal[i] = 1;
    }
    return signal;
}
