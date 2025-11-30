// defines technical indicators for analyzing price trends

#pragma once
#include <vector>

namespace sp {

// base class for all indicators
class Indicator {
public:
    virtual ~Indicator() = default;
    virtual std::vector<double> compute(const std::vector<double>& prices) = 0;
};

class SMAIndicator : public Indicator {
public:
    explicit SMAIndicator(int period) : period_(period) {}
    std::vector<double> compute(const std::vector<double>& prices) override;
private:
    int period_;
};

class EMAIndicator : public Indicator {
public:
    explicit EMAIndicator(int period) : period_(period) {}
    std::vector<double> compute(const std::vector<double>& prices) override;
private:
    int period_;
};

class RSIIndicator : public Indicator {
public:
    explicit RSIIndicator(int period) : period_(period) {}
    std::vector<double> compute(const std::vector<double>& prices) override;
private:
    int period_;
};

class MACDIndicator : public Indicator {
public:
    MACDIndicator(int fast, int slow) : fast_(fast), slow_(slow) {}
    std::vector<double> compute(const std::vector<double>& prices) override;
private:
    int fast_;
    int slow_;
};

} // namespace sp
