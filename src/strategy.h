#pragma once
#include "csv_loader.h"
#include <memory>
#include <vector>

class Strategy {
public:
    virtual ~Strategy() = default;
    virtual std::vector<int> generate_signals(const std::vector<Bar>& bars) = 0;
};

class RuleBasedStrategy : public Strategy {
public:
    RuleBasedStrategy(std::shared_ptr<class Indicator> shortEma, std::shared_ptr<class Indicator> longSma);
    std::vector<int> generate_signals(const std::vector<Bar>& bars) override;
private:
    std::shared_ptr<Indicator> shortEma_;
    std::shared_ptr<Indicator> longSma_;
};
