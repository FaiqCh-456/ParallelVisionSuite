#pragma once
#include "filter_engine.h"
class SerialMotionBlurFilter : public IFilter {
public:
    ImageBuffer apply(const ImageBuffer& input, int numThreads = 1) override;
    std::string name() const override { return "Motion Blur (Serial)"; }
private:
    static const int LENGTH = 15;  // blur length in pixels
};
