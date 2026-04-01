<<<<<<< HEAD
#pragma once
#include "orchestrator.h"
#include "benchmark_runner.h"
#include <atomic>
#include <thread>
#include <memory>
#include <string>

class BenchmarkPanel {
public:
    bool visible = true;
    explicit BenchmarkPanel(Orchestrator& orch);
    ~BenchmarkPanel();

    void render();
    void startBenchmark();
    void exportCSV();

private:
    Orchestrator&   m_orch;
    BenchmarkConfig m_cfg;
    std::unique_ptr<BenchmarkRunner> m_runner;
    std::thread     m_benchThread;
    std::atomic<float> m_progress{0.0f};
    bool            m_running  = false;
    bool            m_finished = false;

    void renderConfigTab();
    void renderResultsTab();
    void renderChartsTab();
=======
#pragma once
#include "orchestrator.h"
#include "benchmark_runner.h"
#include <atomic>
#include <thread>
#include <memory>
#include <string>

class BenchmarkPanel {
public:
    bool visible = true;
    explicit BenchmarkPanel(Orchestrator& orch);
    ~BenchmarkPanel();

    void render();
    void startBenchmark();
    void exportCSV();

private:
    Orchestrator&   m_orch;
    BenchmarkConfig m_cfg;
    std::unique_ptr<BenchmarkRunner> m_runner;
    std::thread     m_benchThread;
    std::atomic<float> m_progress{0.0f};
    bool            m_running  = false;
    bool            m_finished = false;

    void renderConfigTab();
    void renderResultsTab();
    void renderChartsTab();
>>>>>>> 7d871911432ff019623cab0b3e39a8bf91592573
};