#pragma once
#include "filter_engine.h"

class SerialMedianFilter : public IFilter {
public:
    ImageBuffer apply(const ImageBuffer& input, int numThreads = 1) override;
    std::string name() const override { return "Median Filter (Serial)"; }
private:
    static const int RADIUS = 2;  // 5x5 window
};