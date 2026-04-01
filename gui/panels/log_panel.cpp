#include "log_panel.h"
#include "imgui.h"
#include "performance_logger.h"

void LogPanel::render() {
    ImGui::Begin("Debug Log", &visible);
    ImGui::Checkbox("Auto-scroll", &m_autoScroll);
    ImGui::SameLine();
    if (ImGui::Button("Clear")) PerformanceLogger::instance().messages();

    ImGui::Separator();
    ImGui::BeginChild("LogScroll", ImVec2(0, 0), false,
                      ImGuiWindowFlags_HorizontalScrollbar);

    auto& msgs = PerformanceLogger::instance().messages();
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 2));
    for (const auto& m : msgs) {
        // Colour-code by level
        bool isError = m.find("[ERROR]") != std::string::npos;
        bool isWarn  = m.find("[WARN]")  != std::string::npos;
        bool isOk    = m.find("[OK]")    != std::string::npos;
        if      (isError) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1,0.3f,0.3f,1));
        else if (isWarn)  ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1,0.85f,0.1f,1));
        else if (isOk)    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f,1,0.4f,1));
        else              ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.85f,0.85f,0.85f,1));

        ImGui::TextUnformatted(m.c_str());
        ImGui::PopStyleColor();
    }
    ImGui::PopStyleVar();

    if (m_autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);

    ImGui::EndChild();
    ImGui::End();
}

void LogPanel::clear() {
    PerformanceLogger::instance().clear();
}