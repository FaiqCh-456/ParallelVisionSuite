#pragma once
#include "imgui.h"

class ArchAnimPanel {
public:
    bool visible = true;
    void render();
private:
    float m_animTime = 0.0f;
    void  drawMPINodes(ImDrawList* dl, ImVec2 origin, int n);
    void  drawGPUBlocks(ImDrawList* dl, ImVec2 origin);
    void  drawDataFlow(ImDrawList* dl, ImVec2 from, ImVec2 to, float t);
};