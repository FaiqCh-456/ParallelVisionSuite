#pragma once
#include "orchestrator.h"
#include <vector>
#include <chrono>

class ThreadVizPanel {
public:
    bool visible = true;
    explicit ThreadVizPanel(Orchestrator& orch);
    void render();
private:
    Orchestrator& m_orch;
    // Simulated thread activity [0..1] per thread
    std::vector<float> m_activity;
    double             m_simTime = 0.0;
    int                m_maxThreads = 16;
};