#pragma once
#include "image_buffer.h"
#include "filter_engine.h"

// Thin stateless dispatcher — translates (mode, filter) → concrete call.
// Orchestrator uses this so it doesn't have conditional chains everywhere.
class ModeDispatcher {
public:
    static ImageBuffer dispatch(const ImageBuffer& input,
                                FilterType         filter,
                                ExecutionMode      mode,
                                int                threads,
                                double&            outMs);
};