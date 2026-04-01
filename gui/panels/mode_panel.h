<<<<<<< HEAD
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
=======
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
>>>>>>> 7d871911432ff019623cab0b3e39a8bf91592573
};