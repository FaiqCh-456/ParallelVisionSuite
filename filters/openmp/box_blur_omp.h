#pragma once
#include "filter_engine.h"
class OmpBoxBlurFilter : public IFilter {
public:
    ImageBuffer apply(const ImageBuffer& input, int numThreads = 4) override;
    std::string name() const override { return "Box Blur (OpenMP)"; }
private:
    static const int RADIUS = 2;
};
