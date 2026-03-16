#pragma once
#include "image_buffer.h"
#include "filter_engine.h"
#include <string>
#include <vector>
#include <functional>

// Per-rank processing mode when used with MPI+OMP or MPI+CUDA
enum class RankProcessMode { SERIAL, OPENMP, CUDA };

struct MPIConfig {
    int rank       = 0;
    int worldSize  = 1;
    RankProcessMode rankMode = RankProcessMode::OPENMP;
    int ompThreadsPerRank    = 4;
};

class MPIManager {
public:
    static MPIManager& instance();

    // Must be called before any MPI work
    void init(int argc, char** argv);
    void finalize();

    int  rank()      const { return m_cfg.rank; }
    int  worldSize() const { return m_cfg.worldSize; }
    bool isMaster()  const { return m_cfg.rank == 0; }

    void setRankMode(RankProcessMode mode) { m_cfg.rankMode = mode; }
    void setOmpThreads(int t)              { m_cfg.ompThreadsPerRank = t; }

    // Main entry: master scatters, all ranks filter, master gathers.
    // Only rank 0 needs a valid 'input'; others pass empty buffer.
    // Returns processed image on rank 0; empty on others.
    ImageBuffer scatterProcessGather(const ImageBuffer& input,
                                     FilterType filter);

    // Broadcast image metadata from rank 0 to all
    void broadcastMetadata(int& w, int& h, int& ch);

    // Log a message (only rank 0 prints by default)
    void log(const std::string& msg);

    bool isInitialized() const { return m_initialized; }

private:
    MPIManager() = default;
    MPIConfig m_cfg;
    bool      m_initialized = false;
};