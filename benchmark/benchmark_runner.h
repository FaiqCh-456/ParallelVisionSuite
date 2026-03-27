#pragma once
#include "filter_engine.h"
#include "performance_logger.h"
#include <vector>
#include <functional>
#include <atomic>

struct BenchmarkConfig {
    std::vector<int>           imageSizes  = {512, 1024, 2048, 4096};
    std::vector<FilterType>    filters     = {
        FilterType::GAUSSIAN, FilterType::SOBEL,
        FilterType::MEDIAN,   FilterType::LAPLACIAN,
        FilterType::SHARPEN
    };
    std::vector<ExecutionMode> modes       = {
        ExecutionMode::SERIAL,
        ExecutionMode::OPENMP,
        ExecutionMode::CUDA
    };
    std::vector<int>           threadCounts = {1, 2, 4, 8};
    int                        runsPerTest  = 3;    // average over N runs
    bool                       saveImages   = false;
    std::string                outputDir    = "./benchmark_out/";
};

class BenchmarkRunner {
public:
    explicit BenchmarkRunner(const BenchmarkConfig& cfg);

    // Runs the full benchmark matrix — may take several minutes
    // Progress [0..1] is written to *progress atomically
    void runAll(std::atomic<float>* progress = nullptr);

    // Run a single test cell and return the record
    BenchRecord runSingle(FilterType ft,
                          ExecutionMode mode,
                          int imageSize,
                          int numThreads);

    const std::vector<BenchRecord>& results() const;

    // Convenience: export results to CSV
    bool exportCSV(const std::string& path) const;

    // Interrupt ongoing benchmark
    void requestStop() { m_stopRequested = true; }

private:
    BenchmarkConfig        m_cfg;
    std::atomic<bool>      m_stopRequested{false};
    std::vector<BenchRecord> m_results;

    double measureSerialTime(FilterType ft, int imageSize);
};