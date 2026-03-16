#pragma once
#include "filter_engine.h"
class SerialBoxBlurFilter : public IFilter {
public:
    ImageBuffer apply(const ImageBuffer& input, int numThreads = 1) override;
    std::string name() const override { return "Box Blur (Serial)"; }
private:
    static const int RADIUS = 2;
};
