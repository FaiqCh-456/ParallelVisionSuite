#pragma once
#include "filter_engine.h"
class SerialSepiaFilter : public IFilter {
public:
    ImageBuffer apply(const ImageBuffer& input, int numThreads = 1) override;
    std::string name() const override { return "Sepia Tone (Serial)"; }
};
