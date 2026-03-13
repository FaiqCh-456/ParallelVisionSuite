#include "mode_dispatcher.h"
#include "filter_engine.h"
#include "mpi_manager.h"
#include "mpi_worker.h"
#include "gpu_manager.h"
#include <chrono>    // FIX: added missing include

ImageBuffer ModeDispatcher::dispatch(const ImageBuffer& input,
                                      FilterType         filter,
                                      ExecutionMode      mode,
                                      int                threads,
                                      double&            outMs)
{
    // CUDA fallback — no GPU so always falls back to OpenMP
    if ((mode == ExecutionMode::CUDA || mode == ExecutionMode::MPI_CUDA)
        && !GPUManager::instance().isAvailable())
    {
        mode = (mode == ExecutionMode::CUDA)
             ? ExecutionMode::OPENMP
             : ExecutionMode::MPI_OMP;
    }

    // MPI dispatch
    if (mode == ExecutionMode::MPI     ||
        mode == ExecutionMode::MPI_OMP ||
        mode == ExecutionMode::MPI_CUDA)
    {
        auto& mpi = MPIManager::instance();
        if (!mpi.isInitialized() || mpi.worldSize() <= 1) {
            mode = ExecutionMode::OPENMP;
        } else {
            RankProcessMode rm = RankProcessMode::SERIAL;
            if (mode == ExecutionMode::MPI_OMP)  rm = RankProcessMode::OPENMP;
            if (mode == ExecutionMode::MPI_CUDA) rm = RankProcessMode::CUDA;
            mpi.setRankMode(rm);
            mpi.setOmpThreads(threads);

            // FIX: added #include <chrono> so this now compiles
            auto t0 = std::chrono::high_resolution_clock::now();
            ImageBuffer result = MPIWorker::masterDispatch(input, filter);
            auto t1 = std::chrono::high_resolution_clock::now();
            outMs = std::chrono::duration<double, std::milli>(t1 - t0).count();
            return result;
        }
    }

    // Serial / OpenMP / CUDA
    return FilterEngine::applyTimed(input, filter, mode, threads, outMs);
}