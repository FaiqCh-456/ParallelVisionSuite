#pragma once
#include "filter_engine.h"
class SerialGrayscaleFilter : public IFilter {
public:
    ImageBuffer apply(const ImageBuffer& input, int numThreads = 1) override;
    std::string name() const override { return "Grayscale (Serial)"; }
};
