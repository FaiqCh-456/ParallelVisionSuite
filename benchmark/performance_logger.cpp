#include "performance_logger.h"
#include "csv_writer.h"
#include <chrono>
#include <sstream>
#include <iomanip>
#include <ctime>

PerformanceLogger& PerformanceLogger::instance() {
    static PerformanceLogger inst;
    return inst;
}

static std::string nowTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&t));
    return buf;
}

void PerformanceLogger::addRecord(const BenchRecord& rec) {
    m_records.push_back(rec);
}

bool PerformanceLogger::exportCSV(const std::string& path) const {
    try {
        CSVWriter csv(path);
        csv.writeHeader({"Filter","Mode","Width","Height","Threads",
                         "SerialMs","ParallelMs","Speedup",
                         "Efficiency","FPS","Timestamp"});
        for (const auto& r : m_records) {
            csv.writeRow({
                r.filterName,
                r.modeName,
                std::to_string(r.imageWidth),
                std::to_string(r.imageHeight),
                std::to_string(r.numThreads),
                std::to_string(r.serialTimeMs),
                std::to_string(r.parallelTimeMs),
                std::to_string(r.speedup),
                std::to_string(r.efficiency),
                std::to_string(r.fps),
                r.timestamp
            });
        }
        return true;
    } catch (...) { return false; }
}

double PerformanceLogger::computeSpeedup(double serialMs, double parallelMs) {
    if (parallelMs <= 0.0) return 0.0;
    return serialMs / parallelMs;
}

double PerformanceLogger::computeEfficiency(double speedup, int processors) {
    if (processors <= 0) return 0.0;
    return speedup / processors;
}

void PerformanceLogger::logMessage(const std::string& msg) {
    m_messages.push_back("[" + nowTimestamp() + "] " + msg);
    if (m_messages.size() > 500) m_messages.erase(m_messages.begin());
}