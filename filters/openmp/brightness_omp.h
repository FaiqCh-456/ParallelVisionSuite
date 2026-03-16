#pragma once
#include "filter_engine.h"
class OmpBrightnessFilter : public IFilter {
public:
    float brightness = 30.0f;
    float contrast   = 1.3f;
    ImageBuffer apply(const ImageBuffer& input, int numThreads = 4) override;
    std::string name() const override { return "Brightness/Contrast (OpenMP)"; }
};
