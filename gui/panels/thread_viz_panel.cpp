<<<<<<< HEAD
#include "thread_viz_panel.h"
#include "imgui.h"
#include "implot.h"
#include <GLFW/glfw3.h>
#include <cmath>

ThreadVizPanel::ThreadVizPanel(Orchestrator& orch) : m_orch(orch) {
    m_activity.resize(m_maxThreads, 0.0f);
}

void ThreadVizPanel::render() {
    ImGui::Begin("Thread Activity", &visible);

    int nThreads = m_orch.numThreads();
    m_simTime = glfwGetTime();

    // Simulate thread activity waveforms (sinusoidal per thread)
    m_activity.resize(nThreads);
    for (int i = 0; i < nThreads; ++i) {
        double phase = i * 0.7 + m_simTime * 3.0;
        m_activity[i] = 0.5f + 0.5f * (float)std::sin(phase);
        // Spike when processing
        if (m_orch.isProcessing())
            m_activity[i] = 0.7f + 0.3f * (float)std::fabs(
                std::sin(m_simTime * 8.0 + i));
    }

    ImGui::Text("Active Threads: %d / %d", nThreads, m_maxThreads);
    ImGui::Separator();

    // Draw one progress-bar per thread
    for (int i = 0; i < nThreads; ++i) {
        char lbl[32];
        snprintf(lbl, sizeof(lbl), "T%-2d", i);
        float act = m_activity[i];
        // Colour: green when active, grey when idle
        ImVec4 col = act > 0.3f
            ? ImVec4(0.2f, 0.8f + act*0.2f, 0.3f, 1.0f)
            : ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, col);
        ImGui::ProgressBar(act, ImVec2(-1, 14), "");
        ImGui::PopStyleColor();
        ImGui::SameLine(0, 4);
        ImGui::TextDisabled("%s %.0f%%", lbl, act * 100.0f);
    }

    // ImPlot scrolling timeline
    ImGui::Spacing();
    static std::vector<float> history[16];
    for (int i = 0; i < nThreads && i < 16; ++i) {
        history[i].push_back(m_activity[i]);
        if (history[i].size() > 200) history[i].erase(history[i].begin());
    }

    if (ImPlot::BeginPlot("Thread Timeline", ImVec2(-1, 180))) {
        ImPlot::SetupAxes("Frame", "Activity");
        ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 1.2, ImGuiCond_Always);
        for (int i = 0; i < nThreads && i < 16; ++i) {
            char name[16]; snprintf(name, sizeof(name), "T%d", i);
            ImPlot::PlotLine(name, history[i].data(), (int)history[i].size());
        }
        ImPlot::EndPlot();
    }

    ImGui::End();
=======
#include "thread_viz_panel.h"
#include "imgui.h"
#include "implot.h"
#include <GLFW/glfw3.h>
#include <cmath>

ThreadVizPanel::ThreadVizPanel(Orchestrator& orch) : m_orch(orch) {
    m_activity.resize(m_maxThreads, 0.0f);
}

void ThreadVizPanel::render() {
    ImGui::Begin("Thread Activity", &visible);

    int nThreads = m_orch.numThreads();
    m_simTime = glfwGetTime();

    // Simulate thread activity waveforms (sinusoidal per thread)
    m_activity.resize(nThreads);
    for (int i = 0; i < nThreads; ++i) {
        double phase = i * 0.7 + m_simTime * 3.0;
        m_activity[i] = 0.5f + 0.5f * (float)std::sin(phase);
        // Spike when processing
        if (m_orch.isProcessing())
            m_activity[i] = 0.7f + 0.3f * (float)std::fabs(
                std::sin(m_simTime * 8.0 + i));
    }

    ImGui::Text("Active Threads: %d / %d", nThreads, m_maxThreads);
    ImGui::Separator();

    // Draw one progress-bar per thread
    for (int i = 0; i < nThreads; ++i) {
        char lbl[32];
        snprintf(lbl, sizeof(lbl), "T%-2d", i);
        float act = m_activity[i];
        // Colour: green when active, grey when idle
        ImVec4 col = act > 0.3f
            ? ImVec4(0.2f, 0.8f + act*0.2f, 0.3f, 1.0f)
            : ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, col);
        ImGui::ProgressBar(act, ImVec2(-1, 14), "");
        ImGui::PopStyleColor();
        ImGui::SameLine(0, 4);
        ImGui::TextDisabled("%s %.0f%%", lbl, act * 100.0f);
    }

    // ImPlot scrolling timeline
    ImGui::Spacing();
    static std::vector<float> history[16];
    for (int i = 0; i < nThreads && i < 16; ++i) {
        history[i].push_back(m_activity[i]);
        if (history[i].size() > 200) history[i].erase(history[i].begin());
    }

    if (ImPlot::BeginPlot("Thread Timeline", ImVec2(-1, 180))) {
        ImPlot::SetupAxes("Frame", "Activity");
        ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 1.2, ImGuiCond_Always);
        for (int i = 0; i < nThreads && i < 16; ++i) {
            char name[16]; snprintf(name, sizeof(name), "T%d", i);
            ImPlot::PlotLine(name, history[i].data(), (int)history[i].size());
        }
        ImPlot::EndPlot();
    }

    ImGui::End();
>>>>>>> 7d871911432ff019623cab0b3e39a8bf91592573
}