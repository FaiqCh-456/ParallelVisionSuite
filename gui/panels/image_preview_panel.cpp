#include "image_preview_panel.h"
#include "imgui.h"

ImagePreviewPanel::ImagePreviewPanel(Orchestrator& orch) : m_orch(orch) {}

void ImagePreviewPanel::render() {
    ImGui::Begin("Image Preview", &visible);

    // Upload textures if images changed
    if (m_orch.hasOriginalImage())
        m_texOriginal.upload(m_orch.originalImage());
    if (m_orch.hasProcessedImage())
        m_texProcessed.upload(m_orch.processedImage());

    // Controls row
    ImGui::Checkbox("Side-by-Side", &m_sideBy);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(120);
    ImGui::SliderFloat("Zoom", &m_zoom, 0.1f, 3.0f);
    ImGui::Separator();

    if (m_sideBy) {
        // ── Side by side layout ───────────────────────────────────────
        ImVec2 avail = ImGui::GetContentRegionAvail();

        // Each image gets exactly half the available width minus a small gap
        float imgW = (avail.x * 0.5f) - 8.0f;
        if (imgW < 10) imgW = 10;

        auto drawImage = [&](GLTexture& tex, const char* label) {
            ImGui::BeginGroup();
            ImGui::TextColored(ImVec4(0.7f, 0.9f, 1.0f, 1.0f), "%s", label);
            if (tex.valid()) {
                // Maintain aspect ratio within imgW
                float ratio = (float)tex.height() / (float)tex.width();
                float dw = imgW * m_zoom;
                if (dw > imgW) dw = imgW;   // never exceed column width
                float dh = dw * ratio;
                ImGui::Image((ImTextureID)(uint64_t)tex.id(), ImVec2(dw, dh));
                ImGui::TextDisabled("%dx%d", tex.width(), tex.height());
            } else {
                ImGui::TextDisabled("(no image)");
            }
            ImGui::EndGroup();
        };

        drawImage(m_texOriginal,  "Original");
        ImGui::SameLine(0, 16);          // 16px gap between the two images
        drawImage(m_texProcessed, "Processed");

    } else {
        // ── Tab layout ────────────────────────────────────────────────
        if (ImGui::BeginTabBar("ImgTabs")) {
            if (ImGui::BeginTabItem("Original")) {
                if (m_texOriginal.valid()) {
                    ImVec2 avail = ImGui::GetContentRegionAvail();
                    float ratio = (float)m_texOriginal.height() / (float)m_texOriginal.width();
                    float dw = avail.x * m_zoom;
                    if (dw > avail.x) dw = avail.x;
                    ImGui::Image((ImTextureID)(uint64_t)m_texOriginal.id(),
                                 ImVec2(dw, dw * ratio));
                    ImGui::TextDisabled("%dx%d", m_texOriginal.width(), m_texOriginal.height());
                } else {
                    ImGui::TextDisabled("(no image loaded)");
                }
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Processed")) {
                if (m_texProcessed.valid()) {
                    ImVec2 avail = ImGui::GetContentRegionAvail();
                    float ratio = (float)m_texProcessed.height() / (float)m_texProcessed.width();
                    float dw = avail.x * m_zoom;
                    if (dw > avail.x) dw = avail.x;
                    ImGui::Image((ImTextureID)(uint64_t)m_texProcessed.id(),
                                 ImVec2(dw, dw * ratio));
                    ImGui::TextDisabled("%dx%d", m_texProcessed.width(), m_texProcessed.height());
                } else {
                    ImGui::TextDisabled("(not processed yet)");
                }
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
    }

    ImGui::End();
}