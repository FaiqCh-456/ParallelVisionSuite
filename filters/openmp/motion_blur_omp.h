#pragma once
#include "filter_engine.h"
class OmpMotionBlurFilter : public IFilter {
public:
    ImageBuffer apply(const ImageBuffer& input, int numThreads = 4) override;
    std::string name() const override { return "Motion Blur (OpenMP)"; }
private:
    static const int LENGTH = 15;
};
