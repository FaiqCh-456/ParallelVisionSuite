#include "session_manager.h"
#include <fstream>
#include <sstream>

SessionManager& SessionManager::instance() {
    static SessionManager inst;
    return inst;
}

bool SessionManager::load(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) return false;
    std::string line;
    while (std::getline(f, line)) {
        std::istringstream ss(line);
        std::string key, val;
        if (std::getline(ss, key, '=') && std::getline(ss, val)) {
            if (key == "filter")      m_state.lastFilter  = static_cast<FilterType>(std::stoi(val));
            else if (key == "mode")   m_state.lastMode    = static_cast<ExecutionMode>(std::stoi(val));
            else if (key == "threads")m_state.lastThreads = std::stoi(val);
            else if (key == "imgPath")m_state.lastImagePath = val;
            else if (key == "vidPath")m_state.lastVideoPath = val;
        }
    }
    return true;
}

bool SessionManager::save(const std::string& path) const {
    std::ofstream f(path);
    if (!f.is_open()) return false;
    f << "filter="  << static_cast<int>(m_state.lastFilter)  << "\n";
    f << "mode="    << static_cast<int>(m_state.lastMode)    << "\n";
    f << "threads=" << m_state.lastThreads                   << "\n";
    f << "imgPath=" << m_state.lastImagePath                  << "\n";
    f << "vidPath=" << m_state.lastVideoPath                  << "\n";
    return true;
}