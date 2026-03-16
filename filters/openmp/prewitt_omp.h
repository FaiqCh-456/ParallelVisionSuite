#pragma once
#include "filter_engine.h"
class OmpPrewittFilter : public IFilter {
public:
    ImageBuffer apply(const ImageBuffer& input, int numThreads = 4) override;
    std::string name() const override { return "Prewitt Edge (OpenMP)"; }
};
