#include "filter_panel.h"
#include "imgui.h"
#include "filter_engine.h"
#include "performance_logger.h"

FilterPanel::FilterPanel(Orchestrator& orch) : m_orch(orch) {}

void FilterPanel::render() {
    ImGui::Begin("Filter Controls", &visible);

    // ── Filter selection ──────────────────────────────────────────────
    ImGui::SeparatorText("Select Filter");

    static const struct { const char* label; FilterType type; const char* desc; } kFilters[] = {
        // Original 5
        { "Gaussian Blur",       FilterType::GAUSSIAN,            "Smooth / noise reduction"    },
        { "Sobel Edge",          FilterType::SOBEL,               "Horizontal + vertical edges" },
        { "Median Filter",       FilterType::MEDIAN,              "Salt & pepper noise removal" },
        { "Laplacian Edge",      FilterType::LAPLACIAN,           "Second-derivative edges"     },
        { "Sharpen",             FilterType::SHARPEN,             "Enhance fine detail"         },
        // New 8
        { "Box Blur",            FilterType::BOX_BLUR,            "Fast uniform blur"           },
        { "Emboss",              FilterType::EMBOSS,              "3D raised relief effect"     },
        { "Sepia Tone",          FilterType::SEPIA,               "Vintage brown-tone effect"   },
        { "Grayscale",           FilterType::GRAYSCALE,           "Black & white conversion"    },
        { "Brightness/Contrast", FilterType::BRIGHTNESS_CONTRAST, "Adjust luminance & range"    },
        { "Bilateral Filter",    FilterType::BILATERAL,           "Edge-preserving blur"        },
        { "Motion Blur",         FilterType::MOTION_BLUR,         "Horizontal motion smear"     },
        { "Prewitt Edge",        FilterType::PREWITT,             "Prewitt gradient detector"   },
    };
    static const int kCount = (int)(sizeof(kFilters)/sizeof(kFilters[0]));

    for (int i = 0; i < kCount; ++i) {
        bool selected = (m_orch.currentFilter() == kFilters[i].type);
        if (ImGui::RadioButton(kFilters[i].label, selected))
            m_orch.setFilter(kFilters[i].type);
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("%s", kFilters[i].desc);
        }
    }

    // ── Thread count ──────────────────────────────────────────────────
    ImGui::Spacing();
    ImGui::SeparatorText("Thread Count");
    int threads = m_orch.numThreads();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 60);
    if (ImGui::SliderInt("##threads", &threads, 1, 32))
        m_orch.setNumThreads(threads);
    ImGui::SameLine();
    ImGui::Text("%d", threads);

    // ── Actions ───────────────────────────────────────────────────────
    ImGui::Spacing();
    ImGui::SeparatorText("Actions");

    bool busy = m_orch.isProcessing();
    if (busy) ImGui::BeginDisabled();

    float btnW = ImGui::GetContentRegionAvail().x;
    if (ImGui::Button("Process Image", ImVec2(btnW, 32)))
        m_orch.processCurrentImage();

    if (busy) ImGui::EndDisabled();

    if (busy) {
        ImGui::ProgressBar(m_orch.processingProgress(),
                           ImVec2(btnW, 6), "");
    }

    // ── Last result ───────────────────────────────────────────────────
    ImGui::Spacing();
    ImGui::SeparatorText("Last Result");
    double t  = m_orch.lastTimeMs();
    double st = m_orch.lastSerialTimeMs();
    double su = (t > 0 && st > 0) ? st / t : 0.0;
    double fps = (t > 0) ? 1000.0 / t : 0.0;

    ImGui::Text("Time:    %.2f ms", t);
    ImGui::Text("FPS:     %.1f", fps);
    if (su > 0) {
        ImVec4 col = su >= 2.0 ? ImVec4(0.2f,0.9f,0.4f,1.0f)
                   : su >= 1.2 ? ImVec4(0.9f,0.8f,0.2f,1.0f)
                               : ImVec4(0.8f,0.3f,0.3f,1.0f);
        ImGui::TextColored(col, "Speedup: %.2fx", su);
    }

    ImGui::End();
}
