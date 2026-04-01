<<<<<<< HEAD
#include "arch_anim_panel.h"
#include "imgui.h"
#include <GLFW/glfw3.h>
#include <cmath>
#include <cstdio>

void ArchAnimPanel::render() {
    ImGui::Begin("Parallel Architecture", &visible);

    m_animTime = (float)glfwGetTime();
    ImDrawList* dl    = ImGui::GetWindowDrawList();
    ImVec2      base  = ImGui::GetCursorScreenPos();
    ImVec2      avail = ImGui::GetContentRegionAvail();

    // Background
    dl->AddRectFilled(base,
        ImVec2(base.x + avail.x, base.y + avail.y),
        IM_COL32(18, 20, 28, 255), 8.0f);

    // Title labels
    dl->AddText(ImVec2(base.x + 10, base.y + 8),
        IM_COL32(180,200,255,200), "Parallel Architecture Visualiser");

    // --- MPI Nodes (left cluster) ---
    ImVec2 mpiOrigin = ImVec2(base.x + 80, base.y + 80);
    drawMPINodes(dl, mpiOrigin, 4);

    // --- CPU Threads (centre) ---
    ImVec2 cpuOrigin = ImVec2(base.x + avail.x * 0.45f, base.y + 70);
    dl->AddText(ImVec2(cpuOrigin.x - 20, cpuOrigin.y - 16),
        IM_COL32(100,220,255,220), "CPU Threads");
    for (int t = 0; t < 8; ++t) {
        float pulse = 0.5f + 0.5f * sinf(m_animTime * 4.0f + t * 0.8f);
        ImU32 col = IM_COL32(
            (int)(30  + 80  * pulse),
            (int)(120 + 100 * pulse),
            (int)(200 + 55  * pulse), 220);
        ImVec2 tl = ImVec2(cpuOrigin.x + (t % 4) * 42,
                           cpuOrigin.y + (t / 4) * 42);
        dl->AddRectFilled(tl, ImVec2(tl.x+34, tl.y+34), col, 4.0f);
        char lbl[8]; snprintf(lbl, sizeof(lbl), "T%d", t);
        dl->AddText(ImVec2(tl.x+8, tl.y+10), IM_COL32(255,255,255,220), lbl);
    }

    // --- GPU Blocks (right) ---
    ImVec2 gpuOrigin = ImVec2(base.x + avail.x * 0.75f, base.y + 60);
    drawGPUBlocks(dl, gpuOrigin);

    // --- Data flow arrows ---
    ImVec2 mpiOut   = ImVec2(mpiOrigin.x + 120, mpiOrigin.y + 80);
    ImVec2 cpuIn    = ImVec2(cpuOrigin.x - 10, cpuOrigin.y + 42);
    ImVec2 cpuOut   = ImVec2(cpuOrigin.x + 170, cpuOrigin.y + 42);
    ImVec2 gpuIn    = ImVec2(gpuOrigin.x - 10, gpuOrigin.y + 60);

    drawDataFlow(dl, mpiOut,  cpuIn,  m_animTime);
    drawDataFlow(dl, cpuOut,  gpuIn,  m_animTime + 1.0f);

    // Reserve draw space
    ImGui::Dummy(avail);
    ImGui::End();
}

void ArchAnimPanel::drawMPINodes(ImDrawList* dl, ImVec2 origin, int n) {
    dl->AddText(ImVec2(origin.x, origin.y - 18),
        IM_COL32(100,255,160,220), "MPI Ranks");
    for (int i = 0; i < n; ++i) {
        float pulse = 0.5f + 0.5f * sinf(m_animTime * 2.0f + i * 1.2f);
        ImVec2 c = ImVec2(origin.x + 45, origin.y + i * 52 + 25);
        ImU32 col = IM_COL32(
            (int)(20  + 60  * pulse),
            (int)(180 + 60  * pulse),
            (int)(80  + 40  * pulse), 230);
        dl->AddCircleFilled(c, 20.0f, col);
        char lbl[12]; snprintf(lbl, sizeof(lbl), "R%d", i);
        dl->AddText(ImVec2(c.x - 8, c.y - 7),
            IM_COL32(255,255,255,230), lbl);
        // Connector lines between ranks
        if (i < n-1) {
            ImVec2 c2 = ImVec2(origin.x+45, origin.y+(i+1)*52+25);
            dl->AddLine(c, c2, IM_COL32(80,200,120,120), 1.5f);
        }
    }
}

void ArchAnimPanel::drawGPUBlocks(ImDrawList* dl, ImVec2 origin) {
    dl->AddText(ImVec2(origin.x, origin.y - 18),
        IM_COL32(255,160,60,220), "GPU Thread Blocks");
    for (int bx = 0; bx < 5; ++bx) {
        for (int by = 0; by < 4; ++by) {
            float pulse = 0.5f+0.5f*sinf(m_animTime*6.0f + bx*0.5f + by*0.7f);
            ImVec2 tl = ImVec2(origin.x + bx*28, origin.y + by*28);
            ImU32 col = IM_COL32(
                (int)(180 + 75 * pulse),
                (int)(80  + 40 * pulse),
                (int)(20  + 20 * pulse), 210);
            dl->AddRectFilled(tl, ImVec2(tl.x+22, tl.y+22), col, 3.0f);
        }
    }
}

void ArchAnimPanel::drawDataFlow(ImDrawList* dl,
                                  ImVec2 from, ImVec2 to, float t)
{
    // Animated dot travelling from → to
    float frac = fmodf(t * 0.5f, 1.0f);
    ImVec2 dot = ImVec2(from.x + (to.x - from.x) * frac,
                        from.y + (to.y - from.y) * frac);
    dl->AddLine(from, to, IM_COL32(100,180,255,80), 1.5f);
    dl->AddCircleFilled(dot, 5.0f, IM_COL32(100,200,255,240));
=======
#include "arch_anim_panel.h"
#include "imgui.h"
#include <GLFW/glfw3.h>
#include <cmath>
#include <cstdio>

void ArchAnimPanel::render() {
    ImGui::Begin("Parallel Architecture", &visible);

    m_animTime = (float)glfwGetTime();
    ImDrawList* dl    = ImGui::GetWindowDrawList();
    ImVec2      base  = ImGui::GetCursorScreenPos();
    ImVec2      avail = ImGui::GetContentRegionAvail();

    // Background
    dl->AddRectFilled(base,
        ImVec2(base.x + avail.x, base.y + avail.y),
        IM_COL32(18, 20, 28, 255), 8.0f);

    // Title labels
    dl->AddText(ImVec2(base.x + 10, base.y + 8),
        IM_COL32(180,200,255,200), "Parallel Architecture Visualiser");

    // --- MPI Nodes (left cluster) ---
    ImVec2 mpiOrigin = ImVec2(base.x + 80, base.y + 80);
    drawMPINodes(dl, mpiOrigin, 4);

    // --- CPU Threads (centre) ---
    ImVec2 cpuOrigin = ImVec2(base.x + avail.x * 0.45f, base.y + 70);
    dl->AddText(ImVec2(cpuOrigin.x - 20, cpuOrigin.y - 16),
        IM_COL32(100,220,255,220), "CPU Threads");
    for (int t = 0; t < 8; ++t) {
        float pulse = 0.5f + 0.5f * sinf(m_animTime * 4.0f + t * 0.8f);
        ImU32 col = IM_COL32(
            (int)(30  + 80  * pulse),
            (int)(120 + 100 * pulse),
            (int)(200 + 55  * pulse), 220);
        ImVec2 tl = ImVec2(cpuOrigin.x + (t % 4) * 42,
                           cpuOrigin.y + (t / 4) * 42);
        dl->AddRectFilled(tl, ImVec2(tl.x+34, tl.y+34), col, 4.0f);
        char lbl[8]; snprintf(lbl, sizeof(lbl), "T%d", t);
        dl->AddText(ImVec2(tl.x+8, tl.y+10), IM_COL32(255,255,255,220), lbl);
    }

    // --- GPU Blocks (right) ---
    ImVec2 gpuOrigin = ImVec2(base.x + avail.x * 0.75f, base.y + 60);
    drawGPUBlocks(dl, gpuOrigin);

    // --- Data flow arrows ---
    ImVec2 mpiOut   = ImVec2(mpiOrigin.x + 120, mpiOrigin.y + 80);
    ImVec2 cpuIn    = ImVec2(cpuOrigin.x - 10, cpuOrigin.y + 42);
    ImVec2 cpuOut   = ImVec2(cpuOrigin.x + 170, cpuOrigin.y + 42);
    ImVec2 gpuIn    = ImVec2(gpuOrigin.x - 10, gpuOrigin.y + 60);

    drawDataFlow(dl, mpiOut,  cpuIn,  m_animTime);
    drawDataFlow(dl, cpuOut,  gpuIn,  m_animTime + 1.0f);

    // Reserve draw space
    ImGui::Dummy(avail);
    ImGui::End();
}

void ArchAnimPanel::drawMPINodes(ImDrawList* dl, ImVec2 origin, int n) {
    dl->AddText(ImVec2(origin.x, origin.y - 18),
        IM_COL32(100,255,160,220), "MPI Ranks");
    for (int i = 0; i < n; ++i) {
        float pulse = 0.5f + 0.5f * sinf(m_animTime * 2.0f + i * 1.2f);
        ImVec2 c = ImVec2(origin.x + 45, origin.y + i * 52 + 25);
        ImU32 col = IM_COL32(
            (int)(20  + 60  * pulse),
            (int)(180 + 60  * pulse),
            (int)(80  + 40  * pulse), 230);
        dl->AddCircleFilled(c, 20.0f, col);
        char lbl[12]; snprintf(lbl, sizeof(lbl), "R%d", i);
        dl->AddText(ImVec2(c.x - 8, c.y - 7),
            IM_COL32(255,255,255,230), lbl);
        // Connector lines between ranks
        if (i < n-1) {
            ImVec2 c2 = ImVec2(origin.x+45, origin.y+(i+1)*52+25);
            dl->AddLine(c, c2, IM_COL32(80,200,120,120), 1.5f);
        }
    }
}

void ArchAnimPanel::drawGPUBlocks(ImDrawList* dl, ImVec2 origin) {
    dl->AddText(ImVec2(origin.x, origin.y - 18),
        IM_COL32(255,160,60,220), "GPU Thread Blocks");
    for (int bx = 0; bx < 5; ++bx) {
        for (int by = 0; by < 4; ++by) {
            float pulse = 0.5f+0.5f*sinf(m_animTime*6.0f + bx*0.5f + by*0.7f);
            ImVec2 tl = ImVec2(origin.x + bx*28, origin.y + by*28);
            ImU32 col = IM_COL32(
                (int)(180 + 75 * pulse),
                (int)(80  + 40 * pulse),
                (int)(20  + 20 * pulse), 210);
            dl->AddRectFilled(tl, ImVec2(tl.x+22, tl.y+22), col, 3.0f);
        }
    }
}

void ArchAnimPanel::drawDataFlow(ImDrawList* dl,
                                  ImVec2 from, ImVec2 to, float t)
{
    // Animated dot travelling from → to
    float frac = fmodf(t * 0.5f, 1.0f);
    ImVec2 dot = ImVec2(from.x + (to.x - from.x) * frac,
                        from.y + (to.y - from.y) * frac);
    dl->AddLine(from, to, IM_COL32(100,180,255,80), 1.5f);
    dl->AddCircleFilled(dot, 5.0f, IM_COL32(100,200,255,240));
>>>>>>> 7d871911432ff019623cab0b3e39a8bf91592573
}