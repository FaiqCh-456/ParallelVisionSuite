#pragma once
#include "orchestrator.h"
#include "filter_engine.h"

class ModePanel {
public:
    bool visible = true;
    explicit ModePanel(Orchestrator& orch);
    void render();
private:
    Orchestrator& m_orch;
    int m_selectedMode = 0;
};