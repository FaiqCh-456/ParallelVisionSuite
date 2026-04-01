<<<<<<< HEAD
#pragma once
#include "orchestrator.h"
#include "filter_engine.h"

class FilterPanel {
public:
    bool visible = true;
    explicit FilterPanel(Orchestrator& orch);
    void render();
private:
    Orchestrator& m_orch;
    int  m_selectedFilter = 0;
    int  m_numThreads     = 4;
    bool m_processing     = false;
=======
#pragma once
#include "orchestrator.h"
#include "filter_engine.h"

class FilterPanel {
public:
    bool visible = true;
    explicit FilterPanel(Orchestrator& orch);
    void render();
private:
    Orchestrator& m_orch;
    int  m_selectedFilter = 0;
    int  m_numThreads     = 4;
    bool m_processing     = false;
>>>>>>> 7d871911432ff019623cab0b3e39a8bf91592573
};