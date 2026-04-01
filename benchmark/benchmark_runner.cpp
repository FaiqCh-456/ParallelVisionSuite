#include "benchmark_runner.h"
#include "image_loader.h"
#include "filter_engine.h"
#include "performance_logger.h"
#include "timer.h"
#include <algorithm>
#include <numeric>
#include <sstream>

BenchmarkRunner::BenchmarkRunner(const BenchmarkConfig& cfg)
    : m_cfg(cfg) {}

double BenchmarkRunner::measureSerialTime(FilterType ft, int imageSize) {
    ImageBuffer img = ImageLoader::createTestImage(imageSize, imageSize);
    std::vector<double> times;
    for (int r = 0; r < m_cfg.runsPerTest; ++r) {
        double t = 0.0;
        FilterEngine::applyTimed(img, ft, ExecutionMode::SERIAL, 1, t);
        times.push_back(t);
    }
    double avg = std::accumulate(times.begin(), times.end(), 0.0)
               / times.size();
    return avg;
}

BenchRecord BenchmarkRunner::runSingle(FilterType ft,
                                        ExecutionMode mode,
                                        int imageSize,
                                        int numThreads)
{
    ImageBuffer img = ImageLoader::createTestImage(imageSize, imageSize);
    std::vector<double> times;
    for (int r = 0; r < m_cfg.runsPerTest; ++r) {
        double t = 0.0;
        FilterEngine::applyTimed(img, ft, mode, numThreads, t);
        times.push_back(t);
    }
    double parallelMs = std::accumulate(times.begin(), times.end(), 0.0)
                      / times.size();
    double serialMs   = measureSerialTime(ft, imageSize);

    BenchRecord rec;
    rec.filterName    = filterName(ft);
    rec.modeName      = modeName(mode);
    rec.imageWidth    = imageSize;
    rec.imageHeight   = imageSize;
    rec.numThreads    = numThreads;
    rec.serialTimeMs  = serialMs;
    rec.parallelTimeMs= parallelMs;
    rec.speedup       = PerformanceLogger::computeSpeedup(serialMs, parallelMs);
    rec.efficiency    = PerformanceLogger::computeEfficiency(rec.speedup,
                                                             numThreads);
    // FPS: assuming processing one frame per run
    rec.fps           = (parallelMs > 0) ? 1000.0 / parallelMs : 0.0;

    return rec;
}

void BenchmarkRunner::runAll(std::atomic<float>* progress) {
    m_results.clear();
    m_stopRequested = false;

    size_t total = m_cfg.imageSizes.size()
                 * m_cfg.filters.size()
                 * m_cfg.modes.size()
                 * m_cfg.threadCounts.size();
    size_t done  = 0;

    for (int sz : m_cfg.imageSizes) {
        for (FilterType ft : m_cfg.filters) {
            for (ExecutionMode mode : m_cfg.modes) {
                for (int threads : m_cfg.threadCounts) {
                    if (m_stopRequested) return;

                    // Skip irrelevant thread counts for serial/CUDA
                    if (mode == ExecutionMode::SERIAL && threads > 1) {
                        ++done;
                        if (progress) *progress = (float)done / total;
                        continue;
                    }

                    BenchRecord rec = runSingle(ft, mode, sz, threads);
                    m_results.push_back(rec);
                    PerformanceLogger::instance().addRecord(rec);

                    std::ostringstream oss;
                    oss << rec.filterName << " | " << rec.modeName
                        << " | " << sz << "x" << sz
                        << " | threads=" << threads
                        << " | " << rec.parallelTimeMs << " ms"
                        << " | speedup=" << rec.speedup;
                    PerformanceLogger::instance().logMessage(oss.str());

                    ++done;
                    if (progress) *progress = (float)done / total;
                }
            }
        }
    }
}

const std::vector<BenchRecord>& BenchmarkRunner::results() const {
    return m_results;
}

bool BenchmarkRunner::exportCSV(const std::string& path) const {
    return PerformanceLogger::instance().exportCSV(path);
}