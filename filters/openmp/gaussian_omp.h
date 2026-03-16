#pragma once
#include "filter_engine.h"

class OmpGaussianFilter : public IFilter {
public:
    ImageBuffer apply(const ImageBuffer& input, int numThreads = 4) override;
    std::string name() const override { return "Gaussian Blur (OpenMP)"; }
private:
    static const float KERNEL[5][5];
    static const float KERNEL_SUM;
};