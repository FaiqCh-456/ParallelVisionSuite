#pragma once
#include "mpi_manager.h"
#include "filter_engine.h"
#include <string>

// Runs the worker event loop for non-master MPI ranks.
// Workers wait for a command tag from rank 0.
class MPIWorker {
public:
    enum class Tag : int {
        TASK_FILTER  = 10,
        TASK_BENCH   = 11,
        TASK_EXIT    = 99
    };

    // Blocking loop — workers call this and wait for commands
    static void runWorkerLoop();

    // Master sends filter task to all workers, then calls scatterProcessGather
    static ImageBuffer masterDispatch(const ImageBuffer& input,
                                      FilterType filter);
};