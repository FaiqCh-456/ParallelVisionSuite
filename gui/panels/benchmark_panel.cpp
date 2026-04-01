<<<<<<< HEAD
#include "benchmark_panel.h"
#include "imgui.h"
#include "implot.h"
#include "performance_logger.h"
#include <vector>
#include <string>
#include <numeric>

BenchmarkPanel::BenchmarkPanel(Orchestrator& orch) : m_orch(orch) {}

BenchmarkPanel::~BenchmarkPanel() {
    if (m_runner) m_runner->requestStop();
    if (m_benchThread.joinable()) m_benchThread.join();
}

void BenchmarkPanel::startBenchmark() {
    if (m_running) return;
    m_runner   = std::make_unique<BenchmarkRunner>(m_cfg);
    m_progress = 0.0f;
    m_running  = true;
    m_finished = false;
    m_benchThread = std::thread([this]() {
        m_runner->runAll(&m_progress);
        m_running  = false;
        m_finished = true;
    });
}

void BenchmarkPanel::exportCSV() {
    if (m_runner)
        m_runner->exportCSV("benchmark_results.csv");
    PerformanceLogger::instance().logMessage(
        "Benchmark results exported to benchmark_results.csv");
}

void BenchmarkPanel::render() {
    ImGui::Begin("Benchmark", &visible);

    if (ImGui::BeginTabBar("BenchTabs")) {
        if (ImGui::BeginTabItem("Config"))  { renderConfigTab();   ImGui::EndTabItem(); }
        if (ImGui::BeginTabItem("Results")) { renderResultsTab();  ImGui::EndTabItem(); }
        if (ImGui::BeginTabItem("Charts"))  { renderChartsTab();   ImGui::EndTabItem(); }
        ImGui::EndTabBar();
    }
    ImGui::End();
}

void BenchmarkPanel::renderConfigTab() {
    ImGui::SeparatorText("Image Sizes");
    static bool sz512=true, sz1024=true, sz2048=true, sz4096=false;
    ImGui::Checkbox("512",  &sz512);  ImGui::SameLine();
    ImGui::Checkbox("1024", &sz1024); ImGui::SameLine();
    ImGui::Checkbox("2048", &sz2048); ImGui::SameLine();
    ImGui::Checkbox("4096", &sz4096);

    m_cfg.imageSizes.clear();
    if (sz512)  m_cfg.imageSizes.push_back(512);
    if (sz1024) m_cfg.imageSizes.push_back(1024);
    if (sz2048) m_cfg.imageSizes.push_back(2048);
    if (sz4096) m_cfg.imageSizes.push_back(4096);

    ImGui::SeparatorText("Runs per test");
    ImGui::SliderInt("Runs", &m_cfg.runsPerTest, 1, 10);

    ImGui::Spacing();
    if (m_running) {
        ImGui::ProgressBar(m_progress, ImVec2(-1,0), "Running...");
        if (ImGui::Button("Stop")) {
            if (m_runner) m_runner->requestStop();
        }
    } else {
        if (ImGui::Button("Run Full Benchmark", ImVec2(-1, 40)))
            startBenchmark();
    }
    if (m_finished)
        ImGui::TextColored(ImVec4(0.2f,1.0f,0.3f,1.0f), "Benchmark complete!");

    if (ImGui::Button("Export CSV", ImVec2(-1,0))) exportCSV();
}

void BenchmarkPanel::renderResultsTab() {
    auto& recs = PerformanceLogger::instance().records();
    if (recs.empty()) { ImGui::TextDisabled("No results yet."); return; }

    if (ImGui::BeginTable("BenchTable", 8,
        ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
        ImGuiTableFlags_ScrollY | ImGuiTableFlags_Resizable,
        ImVec2(0, 400)))
    {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Filter");
        ImGui::TableSetupColumn("Mode");
        ImGui::TableSetupColumn("Size");
        ImGui::TableSetupColumn("Threads");
        ImGui::TableSetupColumn("Serial ms");
        ImGui::TableSetupColumn("Parallel ms");
        ImGui::TableSetupColumn("Speedup");
        ImGui::TableSetupColumn("Efficiency");
        ImGui::TableHeadersRow();

        for (const auto& r : recs) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("%s", r.filterName.c_str());
            ImGui::TableSetColumnIndex(1); ImGui::Text("%s", r.modeName.c_str());
            ImGui::TableSetColumnIndex(2); ImGui::Text("%dx%d", r.imageWidth, r.imageHeight);
            ImGui::TableSetColumnIndex(3); ImGui::Text("%d", r.numThreads);
            ImGui::TableSetColumnIndex(4); ImGui::Text("%.2f", r.serialTimeMs);
            ImGui::TableSetColumnIndex(5); ImGui::Text("%.2f", r.parallelTimeMs);
            ImGui::TableSetColumnIndex(6); ImGui::Text("%.2f", r.speedup);
            ImGui::TableSetColumnIndex(7); ImGui::Text("%.0f%%", r.efficiency*100);
        }
        ImGui::EndTable();
    }
}

void BenchmarkPanel::renderChartsTab() {
    auto& recs = PerformanceLogger::instance().records();
    if (recs.empty()) { ImGui::TextDisabled("Run benchmark first."); return; }

    // 1. Group speedup values
    std::vector<double> speedupSerial, speedupOmp, speedupCuda;
    for (const auto& r : recs) {
        if (r.modeName == "Serial")  speedupSerial.push_back(r.speedup);
        if (r.modeName == "OpenMP")  speedupOmp.push_back(r.speedup);
        if (r.modeName == "CUDA")    speedupCuda.push_back(r.speedup);
    }

    auto avg = [](const std::vector<double>& v) -> double {
        if (v.empty()) return 0;
        double sum = 0;
        for (double d : v) sum += d;
        return sum / (double)v.size();
    };

    const char* labels[] = {"Serial", "OpenMP", "CUDA"};
    double values[3] = { avg(speedupSerial), avg(speedupOmp), avg(speedupCuda) };
    double positions[3] = {0, 1, 2};

    // 2. Bar Chart (Bulletproof version)
    if (ImPlot::BeginPlot("Average Speedup by Mode", ImVec2(-1, 300))) {
        ImPlot::SetupAxes("Mode", "Speedup (x)");
        ImPlot::SetupAxisTicks(ImAxis_X1, positions, 3, labels);
        
        // Use IM_COL32 to bake alpha into the color (0xD2 = ~210/255 alpha)
        // Format: IM_COL32(R, G, B, A)
        ImU32 colors[3] = { 
            IM_COL32(160, 160, 160, 210), // Serial: Grey
            IM_COL32(60, 140, 240, 210),  // OpenMP: Blue
            IM_COL32(60, 210, 100, 210)   // CUDA: Green
        };

        for (int i = 0; i < 3; ++i) {
            // Using raw index 0 which corresponds to ImPlotCol_Line/Fill
            ImPlot::PushStyleColor(0, colors[i]); 
            ImPlot::PlotBars(labels[i], &positions[i], &values[i], 1, 0.6);
            ImPlot::PopStyleColor();
        }

        ImPlot::EndPlot();
    }

    // 3. Line Chart: Performance Scaling
    std::vector<double> sizes, times;
    for (const auto& r : recs) {
        if (r.modeName == "OpenMP" && r.numThreads == 4) {
            sizes.push_back((double)r.imageWidth);
            times.push_back(r.parallelTimeMs);
        }
    }

    if (!sizes.empty()) {
        if (ImPlot::BeginPlot("OpenMP Scaling (4 Threads)", ImVec2(-1, 250))) {
            ImPlot::SetupAxes("Image Width (px)", "Execution Time (ms)");
            ImPlot::PlotLine("OpenMP", sizes.data(), times.data(), (int)sizes.size());
            ImPlot::EndPlot();
        }
    }
=======
#include "benchmark_panel.h"
#include "imgui.h"
#include "implot.h"
#include "performance_logger.h"
#include <vector>
#include <string>
#include <numeric>

BenchmarkPanel::BenchmarkPanel(Orchestrator& orch) : m_orch(orch) {}

BenchmarkPanel::~BenchmarkPanel() {
    if (m_runner) m_runner->requestStop();
    if (m_benchThread.joinable()) m_benchThread.join();
}

void BenchmarkPanel::startBenchmark() {
    if (m_running) return;
    m_runner   = std::make_unique<BenchmarkRunner>(m_cfg);
    m_progress = 0.0f;
    m_running  = true;
    m_finished = false;
    m_benchThread = std::thread([this]() {
        m_runner->runAll(&m_progress);
        m_running  = false;
        m_finished = true;
    });
}

void BenchmarkPanel::exportCSV() {
    if (m_runner)
        m_runner->exportCSV("benchmark_results.csv");
    PerformanceLogger::instance().logMessage(
        "Benchmark results exported to benchmark_results.csv");
}

void BenchmarkPanel::render() {
    ImGui::Begin("Benchmark", &visible);

    if (ImGui::BeginTabBar("BenchTabs")) {
        if (ImGui::BeginTabItem("Config"))  { renderConfigTab();   ImGui::EndTabItem(); }
        if (ImGui::BeginTabItem("Results")) { renderResultsTab();  ImGui::EndTabItem(); }
        if (ImGui::BeginTabItem("Charts"))  { renderChartsTab();   ImGui::EndTabItem(); }
        ImGui::EndTabBar();
    }
    ImGui::End();
}

void BenchmarkPanel::renderConfigTab() {
    ImGui::SeparatorText("Image Sizes");
    static bool sz512=true, sz1024=true, sz2048=true, sz4096=false;
    ImGui::Checkbox("512",  &sz512);  ImGui::SameLine();
    ImGui::Checkbox("1024", &sz1024); ImGui::SameLine();
    ImGui::Checkbox("2048", &sz2048); ImGui::SameLine();
    ImGui::Checkbox("4096", &sz4096);

    m_cfg.imageSizes.clear();
    if (sz512)  m_cfg.imageSizes.push_back(512);
    if (sz1024) m_cfg.imageSizes.push_back(1024);
    if (sz2048) m_cfg.imageSizes.push_back(2048);
    if (sz4096) m_cfg.imageSizes.push_back(4096);

    ImGui::SeparatorText("Runs per test");
    ImGui::SliderInt("Runs", &m_cfg.runsPerTest, 1, 10);

    ImGui::Spacing();
    if (m_running) {
        ImGui::ProgressBar(m_progress, ImVec2(-1,0), "Running...");
        if (ImGui::Button("Stop")) {
            if (m_runner) m_runner->requestStop();
        }
    } else {
        if (ImGui::Button("Run Full Benchmark", ImVec2(-1, 40)))
            startBenchmark();
    }
    if (m_finished)
        ImGui::TextColored(ImVec4(0.2f,1.0f,0.3f,1.0f), "Benchmark complete!");

    if (ImGui::Button("Export CSV", ImVec2(-1,0))) exportCSV();
}

void BenchmarkPanel::renderResultsTab() {
    auto& recs = PerformanceLogger::instance().records();
    if (recs.empty()) { ImGui::TextDisabled("No results yet."); return; }

    if (ImGui::BeginTable("BenchTable", 8,
        ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
        ImGuiTableFlags_ScrollY | ImGuiTableFlags_Resizable,
        ImVec2(0, 400)))
    {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Filter");
        ImGui::TableSetupColumn("Mode");
        ImGui::TableSetupColumn("Size");
        ImGui::TableSetupColumn("Threads");
        ImGui::TableSetupColumn("Serial ms");
        ImGui::TableSetupColumn("Parallel ms");
        ImGui::TableSetupColumn("Speedup");
        ImGui::TableSetupColumn("Efficiency");
        ImGui::TableHeadersRow();

        for (const auto& r : recs) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("%s", r.filterName.c_str());
            ImGui::TableSetColumnIndex(1); ImGui::Text("%s", r.modeName.c_str());
            ImGui::TableSetColumnIndex(2); ImGui::Text("%dx%d", r.imageWidth, r.imageHeight);
            ImGui::TableSetColumnIndex(3); ImGui::Text("%d", r.numThreads);
            ImGui::TableSetColumnIndex(4); ImGui::Text("%.2f", r.serialTimeMs);
            ImGui::TableSetColumnIndex(5); ImGui::Text("%.2f", r.parallelTimeMs);
            ImGui::TableSetColumnIndex(6); ImGui::Text("%.2f", r.speedup);
            ImGui::TableSetColumnIndex(7); ImGui::Text("%.0f%%", r.efficiency*100);
        }
        ImGui::EndTable();
    }
}

void BenchmarkPanel::renderChartsTab() {
    auto& recs = PerformanceLogger::instance().records();
    if (recs.empty()) { ImGui::TextDisabled("Run benchmark first."); return; }

    // 1. Group speedup values
    std::vector<double> speedupSerial, speedupOmp, speedupCuda;
    for (const auto& r : recs) {
        if (r.modeName == "Serial")  speedupSerial.push_back(r.speedup);
        if (r.modeName == "OpenMP")  speedupOmp.push_back(r.speedup);
        if (r.modeName == "CUDA")    speedupCuda.push_back(r.speedup);
    }

    auto avg = [](const std::vector<double>& v) -> double {
        if (v.empty()) return 0;
        double sum = 0;
        for (double d : v) sum += d;
        return sum / (double)v.size();
    };

    const char* labels[] = {"Serial", "OpenMP", "CUDA"};
    double values[3] = { avg(speedupSerial), avg(speedupOmp), avg(speedupCuda) };
    double positions[3] = {0, 1, 2};

    // 2. Bar Chart (Bulletproof version)
    if (ImPlot::BeginPlot("Average Speedup by Mode", ImVec2(-1, 300))) {
        ImPlot::SetupAxes("Mode", "Speedup (x)");
        ImPlot::SetupAxisTicks(ImAxis_X1, positions, 3, labels);
        
        // Use IM_COL32 to bake alpha into the color (0xD2 = ~210/255 alpha)
        // Format: IM_COL32(R, G, B, A)
        ImU32 colors[3] = { 
            IM_COL32(160, 160, 160, 210), // Serial: Grey
            IM_COL32(60, 140, 240, 210),  // OpenMP: Blue
            IM_COL32(60, 210, 100, 210)   // CUDA: Green
        };

        for (int i = 0; i < 3; ++i) {
            // Using raw index 0 which corresponds to ImPlotCol_Line/Fill
            ImPlot::PushStyleColor(0, colors[i]); 
            ImPlot::PlotBars(labels[i], &positions[i], &values[i], 1, 0.6);
            ImPlot::PopStyleColor();
        }

        ImPlot::EndPlot();
    }

    // 3. Line Chart: Performance Scaling
    std::vector<double> sizes, times;
    for (const auto& r : recs) {
        if (r.modeName == "OpenMP" && r.numThreads == 4) {
            sizes.push_back((double)r.imageWidth);
            times.push_back(r.parallelTimeMs);
        }
    }

    if (!sizes.empty()) {
        if (ImPlot::BeginPlot("OpenMP Scaling (4 Threads)", ImVec2(-1, 250))) {
            ImPlot::SetupAxes("Image Width (px)", "Execution Time (ms)");
            ImPlot::PlotLine("OpenMP", sizes.data(), times.data(), (int)sizes.size());
            ImPlot::EndPlot();
        }
    }
>>>>>>> 7d871911432ff019623cab0b3e39a8bf91592573
}