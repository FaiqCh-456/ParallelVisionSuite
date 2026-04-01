<<<<<<< HEAD
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
=======
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
>>>>>>> 7d871911432ff019623cab0b3e39a8bf91592573
};