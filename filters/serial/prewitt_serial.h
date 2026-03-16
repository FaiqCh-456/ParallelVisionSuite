#pragma once
#include "filter_engine.h"
class SerialPrewittFilter : public IFilter {
public:
    ImageBuffer apply(const ImageBuffer& input, int numThreads = 1) override;
    std::string name() const override { return "Prewitt Edge (Serial)"; }
};
