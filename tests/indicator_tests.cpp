#include "../src/indicator.h"
#include <iostream>
#include <vector>
#include <cmath>

using namespace sp;
#include <iostream>
#include <vector>
#include <cmath>

bool approx_eq(double a, double b, double tol=1e-8) {
    return std::fabs(a-b) <= tol;
}

int main(){
    std::vector<double> prices = {1,2,3,4,5,6,7,8,9,10};
    SMAIndicator sma3(3);
    auto s = sma3.compute(prices);
    if (s.size()!=prices.size()){ std::cerr<<"SMA size mismatch\n"; return 1; }
    if (!approx_eq(s.back(), 9.0, 1e-9)) { std::cerr<<"SMA last value expected 9 got "<<s.back()<<"\n"; return 2; }

    EMAIndicator ema3(3);
    auto e = ema3.compute(prices);
    if (!approx_eq(e.front(), prices.front())) { std::cerr<<"EMA seed mismatch\n"; return 3; }

    RSIIndicator rsi5(5);
    auto r = rsi5.compute(prices);
    for (size_t i=0;i<r.size();++i) {
        if (!std::isnan(r[i])) {
            if (r[i] < 0.0 || r[i] > 100.0) { std::cerr<<"RSI out of range at "<<i<<" val="<<r[i]<<"\n"; return 4; }
        }
    }

    MACDIndicator macd(3,6);
    auto mac = macd.compute(prices);
    EMAIndicator fast(3); EMAIndicator slow(6);
    auto f = fast.compute(prices); auto sl = slow.compute(prices);
    for (size_t i=0;i<prices.size();++i){
        if (!std::isnan(mac[i]) && (!std::isnan(f[i]) && !std::isnan(sl[i]))) {
            if (!approx_eq(mac[i], f[i]-sl[i], 1e-9)) { std::cerr<<"MACD mismatch at "<<i<<"\n"; return 5; }
        }
    }

    std::cout<<"indicator_tests: PASS\n";
    return 0;
}
