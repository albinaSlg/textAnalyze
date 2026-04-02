#pragma once
#include <string>
#include "MetricCollector.h"

class IStatisticModule {
public:
    virtual ~IStatisticModule() = default;

    virtual void analyze(const std::string& text, MetricCollector& collector) = 0;
};
