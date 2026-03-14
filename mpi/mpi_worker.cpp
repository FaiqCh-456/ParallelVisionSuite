#include "mpi_worker.h"
#include <mpi.h>
#include <iostream>

void MPIWorker::runWorkerLoop() {
    auto& mgr = MPIManager::instance();
    if (mgr.isMaster()) return;   // only non-master ranks run this

    while (true) {
        int cmd = 0;
        MPI_Bcast(&cmd, 1, MPI_INT, 0, MPI_COMM_WORLD);

        if (cmd == static_cast<int>(Tag::TASK_EXIT)) break;

        if (cmd == static_cast<int>(Tag::TASK_FILTER)) {
            int filterInt = 0;
            MPI_Bcast(&filterInt, 1, MPI_INT, 0, MPI_COMM_WORLD);
            FilterType ft = static_cast<FilterType>(filterInt);
            ImageBuffer empty; // metadata will be received inside
            mgr.scatterProcessGather(empty, ft);
        }
    }
}

ImageBuffer MPIWorker::masterDispatch(const ImageBuffer& input,
                                      FilterType filter)
{
    auto& mgr = MPIManager::instance();
    // Broadcast command
    int cmd = static_cast<int>(Tag::TASK_FILTER);
    MPI_Bcast(&cmd, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int filterInt = static_cast<int>(filter);
    MPI_Bcast(&filterInt, 1, MPI_INT, 0, MPI_COMM_WORLD);

    return mgr.scatterProcessGather(input, filter);
}