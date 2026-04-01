#pragma once
#include <string>
#include <vector>
#include <chrono>

struct BenchRecord {
    std::string filterName;
    std::string modeName;
    int         imageWidth   = 0;
    int         imageHeight  = 0;
    int         numThreads   = 0;
    double      serialTimeMs = 0.0;   // reference serial time
    double      parallelTimeMs = 0.0;
    double      speedup      = 0.0;
    double      efficiency   = 0.0;
    double      fps          = 0.0;
    std::string timestamp;
};

class PerformanceLogger {
public:
    static PerformanceLogger& instance();

    void addRecord(const BenchRecord& rec);

    // All records collected this session
    const std::vector<BenchRecord>& records() const { return m_records; }

    void clear() { m_records.clear(); }

    // Export to CSV file
    bool exportCSV(const std::string& path) const;

    // Compute speedup from stored serial baseline
    static double computeSpeedup(double serialMs, double parallelMs);
    static double computeEfficiency(double speedup, int processors);

    // Log a text message with timestamp
    void logMessage(const std::string& msg);
    const std::vector<std::string>& messages() const { return m_messages; }

private:
    PerformanceLogger() = default;
    std::vector<BenchRecord>  m_records;
    std::vector<std::string>  m_messages;
};