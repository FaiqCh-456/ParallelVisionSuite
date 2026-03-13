#pragma once
#include "filter_engine.h"
#include <string>

// Persists user settings across runs (simple text file)
struct SessionState {
    FilterType    lastFilter  = FilterType::GAUSSIAN;
    ExecutionMode lastMode    = ExecutionMode::OPENMP;
    int           lastThreads = 4;
    std::string   lastImagePath;
    std::string   lastVideoPath;
};

class SessionManager {
public:
    static SessionManager& instance();
    bool load(const std::string& path = "session.cfg");
    bool save(const std::string& path = "session.cfg") const;
    SessionState& state() { return m_state; }
    const SessionState& state() const { return m_state; }
private:
    SessionManager() = default;
    SessionState m_state;
};