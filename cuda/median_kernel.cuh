#pragma once
#include "filter_engine.h"

#ifdef HAS_CUDA
class CudaMedianFilter : public IFilter {
public:
    ImageBuffer apply(const ImageBuffer& input, int numThreads = 1) override;
    std::string name() const override { return "Median Filter (CUDA)"; }
};
#endif