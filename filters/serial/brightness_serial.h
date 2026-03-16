#pragma once
#include "filter_engine.h"
class SerialBrightnessFilter : public IFilter {
public:
    // brightness: -100 to +100,  contrast: 0.5 to 2.0
    float brightness = 30.0f;
    float contrast   = 1.3f;
    ImageBuffer apply(const ImageBuffer& input, int numThreads = 1) override;
    std::string name() const override { return "Brightness/Contrast (Serial)"; }
};
