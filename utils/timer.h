#pragma once
#include <chrono>

class Timer {
public:
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = Clock::time_point;

    void start() {
        m_start = Clock::now();
        m_running = true;
    }

    void stop() {
        m_end = Clock::now();
        m_running = false;
    }

    // Returns elapsed milliseconds
    double elapsedMs() const {
        auto endpoint = m_running ? Clock::now() : m_end;
        return std::chrono::duration<double, std::milli>(
            endpoint - m_start).count();
    }

    double elapsedSec() const { return elapsedMs() / 1000.0; }

private:
    TimePoint m_start;
    TimePoint m_end;
    bool m_running = false;
};

// RAII scoped timer — prints on destruction (for debugging)
struct ScopedTimer {
    explicit ScopedTimer(const char* label) : m_label(label) {
        m_timer.start();
    }
    ~ScopedTimer() {
        m_timer.stop();
        // Intentionally silent in production; callers read elapsedMs()
    }
    double elapsedMs() { m_timer.stop(); return m_timer.elapsedMs(); }
    Timer m_timer;
    const char* m_label;
};