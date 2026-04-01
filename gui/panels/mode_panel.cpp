#include "mode_panel.h"
#include "imgui.h"
#include "gpu_manager.h"

static const char* MODE_NAMES[] = {
    "Serial CPU",
    "OpenMP Parallel",
    "CUDA GPU",
    "MPI Distributed",
    "MPI + OpenMP",
    "MPI + CUDA"
};

ModePanel::ModePanel(Orchestrator& orch) : m_orch(orch) {}

void ModePanel::render() {
    ImGui::Begin("Execution Mode", &visible);

    ImGui::SeparatorText("Processing Mode");

    auto& gpu = GPUManager::instance();
    for (int i = 0; i < 6; ++i) {
        bool isCuda = (i == 2 || i == 5);
        bool isMPI  = (i >= 3);
        bool avail  = true;
        if (isCuda && !gpu.isAvailable()) avail = false;

        ImGui::BeginDisabled(!avail);
        if (ImGui::RadioButton(MODE_NAMES[i], m_selectedMode == i)) {
            m_selectedMode = i;
            m_orch.setMode(static_cast<ExecutionMode>(i));
        }
        ImGui::EndDisabled();

        if (isCuda && !gpu.isAvailable()) {
            ImGui::SameLine();
            ImGui::TextDisabled("(No GPU)");
        }
        if (isMPI) {
            ImGui::SameLine();
            ImGui::TextDisabled("(Requires mpiexec)");
        }
    }

    ImGui::Spacing();
    ImGui::SeparatorText("GPU Info");
    if (gpu.isAvailable()) {
        auto& info = gpu.deviceInfo();
        ImGui::TextColored(ImVec4(0.2f,1.0f,0.4f,1.0f), "CUDA GPU READY");
        ImGui::Text("Name:  %s",     info.name.c_str());
        ImGui::Text("VRAM:  %zu MB", info.totalMemoryMB);
        ImGui::Text("SMs:   %d",     info.multiprocessorCount);
        ImGui::Text("Arch:  sm_%d%d",info.computeMajor, info.computeMinor);
    } else {
        ImGui::TextColored(ImVec4(1.0f,0.4f,0.2f,1.0f), "No CUDA GPU");
        ImGui::TextWrapped("CUDA modes will fallback to OpenMP.");
    }

    ImGui::End();
}