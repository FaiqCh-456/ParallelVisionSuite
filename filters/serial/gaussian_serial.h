#pragma once
#include "filter_engine.h"

class SerialGaussianFilter : public IFilter {
public:
    ImageBuffer apply(const ImageBuffer& input, int numThreads = 1) override;
    std::string name() const override { return "Gaussian Blur (Serial)"; }

private:
    // 5x5 Gaussian kernel weights (sigma ≈ 1.4)
    static const float KERNEL[5][5];
    static const float KERNEL_SUM;
};