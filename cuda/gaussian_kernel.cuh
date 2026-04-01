#pragma once
#include "filter_engine.h"

#ifdef HAS_CUDA
class CudaGaussianFilter : public IFilter {
public:
    ImageBuffer apply(const ImageBuffer& input, int numThreads = 1) override;
    std::string name() const override { return "Gaussian Blur (CUDA)"; }
};
#endif