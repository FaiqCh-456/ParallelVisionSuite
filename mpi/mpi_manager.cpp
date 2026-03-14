#include "mpi_manager.h"
#include "filter_engine.h"
#include <mpi.h>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <cstring>

MPIManager& MPIManager::instance() {
    static MPIManager inst;
    return inst;
}

void MPIManager::init(int argc, char** argv) {
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
    MPI_Comm_rank(MPI_COMM_WORLD, &m_cfg.rank);
    MPI_Comm_size(MPI_COMM_WORLD, &m_cfg.worldSize);
    m_initialized = true;
}

void MPIManager::finalize() {
    if (m_initialized) {
        MPI_Finalize();
        m_initialized = false;
    }
}

void MPIManager::broadcastMetadata(int& w, int& h, int& ch) {
    int meta[3] = {w, h, ch};
    MPI_Bcast(meta, 3, MPI_INT, 0, MPI_COMM_WORLD);
    w = meta[0]; h = meta[1]; ch = meta[2];
}

void MPIManager::log(const std::string& msg) {
    if (isMaster()) {
        std::cout << "[MPI Master] " << msg << std::endl;
    }
}

ImageBuffer MPIManager::scatterProcessGather(const ImageBuffer& input,
                                              FilterType filter)
{
    int W = 0, H = 0, C = 0;
    if (isMaster()) { W = input.width; H = input.height; C = input.channels; }

    // Step 1: broadcast image dimensions
    broadcastMetadata(W, H, C);

    // Step 2: compute row distribution
    int np = m_cfg.worldSize;
    std::vector<int> rowCounts(np), rowOffsets(np);
    for (int i = 0; i < np; ++i) {
        rowCounts[i]  = (H / np + (i < H % np ? 1 : 0));
        rowOffsets[i] = (i == 0) ? 0 : rowOffsets[i-1] + rowCounts[i-1];
    }

    // Byte counts / offsets
    std::vector<int> sendCounts(np), sendDisps(np);
    for (int i = 0; i < np; ++i) {
        sendCounts[i] = rowCounts[i]  * W * C;
        sendDisps[i]  = rowOffsets[i] * W * C;
    }

    int myRows  = rowCounts[m_cfg.rank];
    int myBytes = myRows * W * C;

    // Step 3: scatter pixel data
    std::vector<uint8_t> localIn(myBytes);
    MPI_Scatterv(
        isMaster() ? input.data.data() : nullptr,
        sendCounts.data(), sendDisps.data(), MPI_UNSIGNED_CHAR,
        localIn.data(), myBytes, MPI_UNSIGNED_CHAR,
        0, MPI_COMM_WORLD
    );

    // Step 4: each rank applies the filter to its chunk
    ImageBuffer localBuf;
    localBuf.width    = W;
    localBuf.height   = myRows;
    localBuf.channels = C;
    localBuf.data     = localIn;

    // Choose filter implementation per rank mode
    ExecutionMode execMode;
    switch(m_cfg.rankMode) {
        case RankProcessMode::OPENMP: execMode = ExecutionMode::OPENMP; break;
        case RankProcessMode::CUDA:   execMode = ExecutionMode::CUDA;   break;
        default:                      execMode = ExecutionMode::SERIAL; break;
    }

    double elapsed = 0.0;
    ImageBuffer localOut = FilterEngine::applyTimed(
        localBuf, filter, execMode, m_cfg.ompThreadsPerRank, elapsed);

    // Step 5: gather back to rank 0
    ImageBuffer result;
    if (isMaster()) {
        result.width    = W;
        result.height   = H;
        result.channels = C;
        result.data.resize((size_t)W * H * C);
    }

    MPI_Gatherv(
        localOut.data.data(), myBytes, MPI_UNSIGNED_CHAR,
        isMaster() ? result.data.data() : nullptr,
        sendCounts.data(), sendDisps.data(), MPI_UNSIGNED_CHAR,
        0, MPI_COMM_WORLD
    );

    return result;
}