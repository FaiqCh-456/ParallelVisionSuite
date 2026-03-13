/**
 * ParallelVisionSuite — main entry point
 *
 * Modes:
 *   (no args)        → launch GUI
 *   --serial         → CLI serial benchmark
 *   --omp <threads>  → CLI OpenMP benchmark
 *   --mpi            → MPI worker/master dispatch
 *   --benchmark      → full automated benchmark, then exit
 */

#include "orchestrator.h"
#include "gui.h"
#include "mpi_manager.h"
#include "mpi_worker.h"
#include "benchmark_runner.h"
#include "performance_logger.h"
#include "image_loader.h"
#include "filter_engine.h"

#include <mpi.h>
#include <iostream>
#include <string>
#include <cstring>

static void printUsage() {
    std::cout
        << "ParallelVisionSuite\n"
        << "  (no args)            Launch GUI\n"
        << "  --serial             Serial CLI benchmark\n"
        << "  --omp <N>            OpenMP CLI benchmark (N threads)\n"
        << "  --benchmark          Full automated benchmark suite\n"
        << "  --mpi                MPI distributed mode (use with mpiexec)\n"
        << "  --help               Show this message\n";
}

static void runCLIBenchmark(ExecutionMode mode, int threads) {
    BenchmarkConfig cfg;
    cfg.imageSizes  = {512, 1024, 2048};
    cfg.runsPerTest = 3;
    BenchmarkRunner runner(cfg);
    std::cout << "Running benchmark (mode=" << modeName(mode)
              << ", threads=" << threads << ")...\n";
    std::atomic<float> prog{0.0f};
    runner.runAll(&prog);
    runner.exportCSV("cli_benchmark.csv");
    std::cout << "Done. Results → cli_benchmark.csv\n";

    for (const auto& r : runner.results()) {
        std::cout << r.filterName << " | " << r.modeName
                  << " | " << r.imageWidth << "x" << r.imageHeight
                  << " | " << r.parallelTimeMs << " ms"
                  << " | speedup=" << r.speedup << "\n";
    }
}

int main(int argc, char** argv) {
    // ── Parse arguments ──────────────────────────────────────────────
    bool guiMode       = true;
    bool mpiMode       = false;
    bool benchmarkMode = false;
    ExecutionMode cliMode = ExecutionMode::SERIAL;
    int  cliThreads    = 4;

    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--help") == 0) {
            printUsage(); return 0;
        } else if (std::strcmp(argv[i], "--serial") == 0) {
            guiMode = false; cliMode = ExecutionMode::SERIAL;
        } else if (std::strcmp(argv[i], "--omp") == 0) {
            guiMode = false; cliMode = ExecutionMode::OPENMP;
            if (i + 1 < argc) cliThreads = std::atoi(argv[++i]);
        } else if (std::strcmp(argv[i], "--benchmark") == 0) {
            guiMode = false; benchmarkMode = true;
        } else if (std::strcmp(argv[i], "--mpi") == 0) {
            guiMode = false; mpiMode = true;
        }
    }

    // ── MPI initialisation ───────────────────────────────────────────
    auto& mpiMgr = MPIManager::instance();
    mpiMgr.init(argc, argv);

    // Non-master MPI ranks enter worker loop and exit when done
    if (mpiMgr.worldSize() > 1 && !mpiMgr.isMaster()) {
        MPIWorker::runWorkerLoop();
        mpiMgr.finalize();
        return 0;
    }

    // ── GPU detection ─────────────────────────────────────────────────
    Orchestrator orch;
    bool gpuOk = orch.initGPU();
    PerformanceLogger::instance().logMessage(
        gpuOk ? ("[OK] GPU: " + orch.gpuName())
              : "[INFO] No CUDA GPU — using CPU fallback");

    // ── Execution path ───────────────────────────────────────────────
    int exitCode = 0;

    if (benchmarkMode) {
        runCLIBenchmark(ExecutionMode::SERIAL, 1);
        runCLIBenchmark(ExecutionMode::OPENMP, cliThreads);
        if (gpuOk) runCLIBenchmark(ExecutionMode::CUDA, 1);

    } else if (mpiMode) {
        // Simple MPI demo: load test image, apply Gaussian, save
        ImageBuffer img = ImageLoader::createTestImage(1024, 1024);
        mpiMgr.log("Running MPI Gaussian Blur on 1024x1024...");
        ImageBuffer result = MPIWorker::masterDispatch(img, FilterType::GAUSSIAN);
        if (mpiMgr.isMaster()) {
            ImageLoader::save(result, "mpi_output.png");
            mpiMgr.log("Saved → mpi_output.png");
        }
        // Signal workers to exit
        int cmd = static_cast<int>(MPIWorker::Tag::TASK_EXIT);
        MPI_Bcast(&cmd, 1, MPI_INT, 0, MPI_COMM_WORLD);

    } else if (!guiMode) {
        runCLIBenchmark(cliMode, cliThreads);

    } else {
        // ── GUI mode ──────────────────────────────────────────────────
        GUI gui(orch);
        if (!gui.init(1600, 900, "ParallelVisionSuite v1.0")) {
            std::cerr << "Failed to initialise GUI\n";
            exitCode = 1;
        } else {
            gui.run();
        }
    }

    mpiMgr.finalize();
    return exitCode;
}