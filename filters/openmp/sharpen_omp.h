#pragma once
#include "filter_engine.h"

class OmpSharpenFilter : public IFilter {
public:
    ImageBuffer apply(const ImageBuffer& input, int numThreads = 4) override;
    std::string name() const override { return "Sharpen (OpenMP)"; }
};