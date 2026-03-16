#pragma once
#include "filter_engine.h"
class OmpBilateralFilter : public IFilter {
public:
    ImageBuffer apply(const ImageBuffer& input, int numThreads = 4) override;
    std::string name() const override { return "Bilateral Filter (OpenMP)"; }
private:
    static const int RADIUS = 3;
    static constexpr float SIGMA_SPACE = 3.0f;
    static constexpr float SIGMA_COLOR = 50.0f;
};
