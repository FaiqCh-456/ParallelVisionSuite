# ParallelVisionSuite Architecture Documentation

## Table of Contents

1. [Overview](#overview)
2. [Design Principles](#design-principles)
3. [Core Components](#core-components)
4. [System Architecture](#system-architecture)
5. [Execution Paths](#execution-paths)
6. [Filter Architecture](#filter-architecture)
7. [Parallelization Backends](#parallelization-backends)
8. [GUI Architecture](#gui-architecture)
9. [Data Structures](#data-structures)
10. [Threading & Concurrency](#threading--concurrency)
11. [Performance Measurement](#performance-measurement)
12. [Error Handling](#error-handling)
13. [Extensibility Guide](#extensibility-guide)

---

## Overview

ParallelVisionSuite is a modular, multi-threaded image processing framework designed to benchmark and compare different parallelization techniques:

- **Serial** (single-threaded baseline)
- **OpenMP** (shared-memory parallelism)
- **CUDA** (GPU acceleration)
- **MPI** (distributed computing)
- **Hybrid modes** (MPI+OpenMP, MPI+CUDA)

The architecture emphasizes:
- **Separation of Concerns**: Each layer has a single responsibility
- **Scalability**: Easy to add new filters and execution modes
- **Performance**: Minimal overhead, optimized for benchmarking
- **Flexibility**: Support for GUI and CLI interfaces

---

## Design Principles

### 1. **Factory Pattern (Filter Creation)**
The `FilterEngine` uses the Factory pattern to instantiate the appropriate filter implementation based on execution mode:

```
FilterEngine::create(FilterType, ExecutionMode)
    ↓ (Execution Mode)
    ├─ Serial → SerialGaussianFilter, SerialSobelFilter, ...
    ├─ OpenMP → OmpGaussianFilter, OmpSobelFilter, ...
    ├─ CUDA → (GPU device memory, CUDA kernels)
    └─ MPI/Hybrid → MPI dispatching → Worker filters
```

**Benefit**: Single call point for filter instantiation; adding new filters requires minimal changes.

### 2. **Strategy Pattern (Execution Modes)**
Each execution mode is a different strategy for processing:

```
IFilter interface
    ├─ SerialXXXFilter (1 thread, baseline)
    ├─ OmpXXXFilter (N threads via OpenMP)
    ├─ CudaXXXFilter (GPU via CUDA)
    └─ [Implicit via MPI dispatch]
```

**Benefit**: New parallelization methods can be added without changing existing code.

### 3. **Single Responsibility Principle**

| Component | Responsibility |
|-----------|-----------------|
| **Orchestrator** | Manages application state, image/video loading, processing workflow |
| **FilterEngine** | Creates and manages filter instances, measures execution time |
| **ModeDispatcher** | Routes execution to correct backend (stateless) |
| **GUI** | Renders ImGui interface, handles user input |
| **BenchmarkRunner** | Executes benchmark suite, collects metrics, exports CSV |
| **PerformanceLogger** | Centralized logging for debug messages and metrics |
| **SessionManager** | Persists user configuration across runs |

### 4. **Dependency Injection**
- GUI panels receive `Orchestrator&` dependency in constructor
- Avoids global state; easier to test in isolation

### 5. **Const-Correctness**
Used throughout for safety:
```cpp
const ImageBuffer& Orchestrator::originalImage() const;
bool hasVideo() const;
FilterType currentFilter() const { return m_filter; }
```

---

## Core Components

### 1. Orchestrator (`src/orchestrator.h/cpp`)

**Purpose**: Central application state manager and workflow coordinator.

**Key Responsibilities**:
- Manage current image and video state
- Manage current filter, execution mode, and thread count
- Initiate image/video processing workflows
- Initialize GPU (if available)
- Track processing progress and metrics
- Coordinate between GUI panels and processing backends

**Key Methods**:
```cpp
// File I/O
void loadImage(const std::string& path);
void loadVideo(const std::string& path);

// Image/Video state queries
bool hasOriginalImage() const;
const ImageBuffer& originalImage() const;
const ImageBuffer& processedImage() const;
ImageBuffer getVideoFrame(int idx);

// Configuration
void setFilter(FilterType ft);
void setMode(ExecutionMode mode);
void setNumThreads(int n);

// Processing
void processCurrentImage();
void processCurrentVideo();

// Status queries
bool isProcessing() const;
float processingProgress() const;
double lastTimeMs() const;

// GPU management
bool initGPU();
bool gpuAvailable() const;
std::string gpuName() const;
```

**Threading Model**: 
- GUI runs on main thread
- Processing happens on background thread (spawned via `std::thread`)
- Atomic flags (`m_processing`, `m_progress`) for thread-safe status queries

---

### 2. FilterEngine (`filters/filter_engine.h/cpp`)

**Purpose**: Unified interface for all filter implementations across execution modes.

**Key Pattern**: Factory + Strategy
- `create()`: Returns concrete filter implementation based on mode
- `applyTimed()`: Applies filter and measures execution time

**Key Methods**:
```cpp
// Factory method: Creates appropriate filter for mode
static IFilter* create(FilterType type, ExecutionMode mode);

// Applies filter and measures wall-clock time
static ImageBuffer applyTimed(const ImageBuffer& input,
                              FilterType type,
                              ExecutionMode mode,
                              int numThreads,
                              double& outTimeMs);
```

**Filter Creation Logic**:
```cpp
IFilter* FilterEngine::create(FilterType type, ExecutionMode mode) {
    // Check if OpenMP mode
    if (mode == OPENMP || mode == MPI_OMP) {
        // Return OpenMP variant (e.g., OmpGaussianFilter)
    }
    // Check if CUDA available and mode allows it
    else if (mode == CUDA || mode == MPI_CUDA) {
        // Try to return CUDA variant
        // Fallback to Serial if GPU unavailable
    }
    // MPI modes delegated to ModeDispatcher
    else if (mode == MPI || mode == MPI_OMP || mode == MPI_CUDA) {
        // (Handled by ModeDispatcher)
    }
    // Default: Serial implementation
    return new SerialXXXFilter();
}
```

**Supported Filters**: 13 total
- **Edge Detection**: Gaussian, Sobel, Laplacian, Prewitt
- **Blur**: Box Blur, Median, Bilateral, Motion Blur
- **Enhancement**: Sharpen, Emboss
- **Tone**: Brightness/Contrast, Grayscale, Sepia

---

### 3. ModeDispatcher (`src/mode_dispatcher.h/cpp`)

**Purpose**: Stateless dispatcher that routes execution to the appropriate backend.

**Design Rationale**:
- Keeps `Orchestrator` clean (doesn't contain execution logic)
- Handles complex routing (e.g., MPI initialization, GPU checking)
- Single point for mode-specific setup

**Key Method**:
```cpp
static ImageBuffer dispatch(const ImageBuffer& input,
                           FilterType filter,
                           ExecutionMode mode,
                           int threads,
                           double& outMs);
```

**Dispatch Logic**:
```
if (mode == SERIAL || OPENMP || CUDA) {
    return FilterEngine::applyTimed(...);
} else if (mode == MPI || MPI_OMP || MPI_CUDA) {
    // Initialize MPI, set rank mode and thread count
    // Call MPIManager for setup
    // Call MPIWorker::masterDispatch()
    return result;
}
```

---

### 4. GUI (`gui/gui.h/cpp`)

**Purpose**: ImGui-based graphical user interface for interactive processing.

**Architecture**:
```
GUI (main window, event loop)
├─ ImGui context (backend: OpenGL, GLFW)
├─ Dockspace (configurable layout)
└─ Panels (one per feature)
    ├─ ImagePreviewPanel
    ├─ VideoPreviewPanel
    ├─ FilterPanel (filter selection, thread count)
    ├─ ModePanel (execution mode selection)
    ├─ BenchmarkPanel (run benchmark, export)
    ├─ LogPanel (performance metrics, debug messages)
    ├─ ThreadVizPanel (thread utilization visualization)
    └─ ArchAnimPanel (architecture animation)
```

**Key Methods**:
```cpp
bool init(int windowW, int windowH, const std::string& title);
void run();  // Main event loop, blocks until window closed
void close();
```

**Panel Design Pattern**:
- Each panel has `visible` flag for show/hide
- Each receives `Orchestrator&` in constructor for state access/modification
- `render()` method called every frame

**Example Panel** (`FilterPanel`):
```cpp
class FilterPanel {
public:
    bool visible = true;
    explicit FilterPanel(Orchestrator& orch);
    void render();  // Called every frame
private:
    Orchestrator& m_orch;  // Dependency injection
};
```

---

### 5. BenchmarkRunner (`benchmark/benchmark_runner.h/cpp`)

**Purpose**: Automated benchmark suite execution and CSV export.

**Configuration**:
```cpp
struct BenchmarkConfig {
    std::vector<int>           imageSizes   = {512, 1024, 2048, 4096};
    std::vector<FilterType>    filters      = {...};  // Subset of filters
    std::vector<ExecutionMode> modes        = {...};  // Subset of modes
    std::vector<int>           threadCounts = {1, 2, 4, 8};
    int                        runsPerTest  = 3;      // Average over 3 runs
    std::string                outputDir    = "./benchmark_out/";
};
```

**Key Methods**:
```cpp
// Single filter+mode+size combination
BenchRecord runSingle(FilterType, ExecutionMode, int imageSize, int threads);

// Full suite
void runAll(std::atomic<float>* progress);

// Export results
bool exportCSV(const std::string& path) const;
```

**Execution Flow**:
1. For each image size:
   2. For each filter:
       3. For each mode:
           4. For each thread count:
               - Run filter N times (runsPerTest)
               - Average execution times
               - Calculate speedup and efficiency
               - Record result

**CSV Output**:
```
filter_name,mode_name,image_width,image_height,parallel_time_ms,serial_time_ms,speedup,efficiency,fps
Gaussian Blur,Serial,512,512,10.5,10.5,1.00,100.0,95.2
Gaussian Blur,OpenMP_4,512,512,3.2,10.5,3.28,82.0,312.5
```

---

### 6. PerformanceLogger (`benchmark/performance_logger.h/cpp`)

**Purpose**: Centralized logging for metrics and debug messages.

**Design**: Singleton pattern (thread-safe)

**Key Methods**:
```cpp
static PerformanceLogger& instance();

void logMessage(const std::string& msg);
void logMetric(const std::string& name, double value);

std::vector<std::string> messages() const;
std::map<std::string, double> metrics() const;

void clear();
```

**Usage**:
```cpp
PerformanceLogger::instance().logMessage("[OK] Filter applied in 15.5ms");
PerformanceLogger::instance().logMetric("speedup_4thread", 3.8);
```

---

### 7. SessionManager (`src/session_manager.h/cpp`)

**Purpose**: Persist user configuration across application runs.

**Stored State**:
```cpp
struct SessionState {
    FilterType    lastFilter  = FilterType::GAUSSIAN;
    ExecutionMode lastMode    = ExecutionMode::OPENMP;
    int           lastThreads = 4;
    std::string   lastImagePath;
    std::string   lastVideoPath;
};
```

**Key Methods**:
```cpp
static SessionManager& instance();
bool load(const std::string& path = "session.cfg");
bool save(const std::string& path = "session.cfg") const;
SessionState& state();
```

**Usage**: Loads on startup, saves on exit or manually.

---

## System Architecture

### High-Level Block Diagram

```
                    ┌─────────────────────────────┐
                    │      MAIN ENTRY POINT       │
                    │      (src/main.cpp)         │
                    └──────────────┬──────────────┘
                                   │
                    ┌──────────────┬────────────────┐
                    │              │                │
            (no args)        --serial        --omp/--benchmark
                    │              │                │
        ┌───────────▼──────┐   ┌──▼──┐   ┌────────▼────────┐
        │    GUI Mode      │   │ CLI │   │  Benchmark      │
        │   (Orchestrator) │   │Mode │   │  Mode           │
        │   (GUI / ImGui)  │   │     │   │                 │
        └─────────┬────────┘   └──┬──┘   └────────┬────────┘
                  │                │              │
                  └────────────────┼──────────────┘
                                   │
                    ┌──────────────▼──────────────┐
                    │   Orchestrator              │
                    │  - Image/Video state       │
                    │  - Filter + Mode config    │
                    │  - Processing workflow     │
                    └──────────────┬──────────────┘
                                   │
                                   │ processCurrentImage()
                                   │ or
                                   │ processCurrentVideo()
                                   │
                    ┌──────────────▼──────────────┐
                    │   FilterEngine::applyTimed()│
                    │   (Measures wall-clock time)│
                    └──────────────┬──────────────┘
                                   │
                    ┌──────────────▼──────────────┐
                    │  FilterEngine::create()     │
                    │  (Factory Pattern)          │
                    └──┬──┬──┬──┬────────────────┘
                       │  │  │  │
        ┌──────────────┘  │  │  └─────────────────┐
        │                 │  │                    │
    ┌───▼────┐    ┌──────▼──┴──────┐   ┌────────▼────┐
    │ Serial │    │  OpenMP        │   │    CUDA     │
    │ Filters│    │  Filters       │   │   Filters   │
    │        │    │  (Pthread)     │   │   (GPU)     │
    └────────┘    └────────────────┘   └─────────────┘
                           │
        ┌──────────────────┴──────────────────┐
        │                                     │
    ┌───▼──────────────────────────┐  ┌──────▼──────────┐
    │  ModeDispatcher              │  │ MPIManager      │
    │  (Routing for MPI modes)     │  │ MPIWorker       │
    │                              │  │ (Distributed)   │
    └──────────────────────────────┘  └─────────────────┘
```

---

## Execution Paths

### Path 1: Interactive Image Processing (GUI)

```
User opens image file
    ↓
Orchestrator::loadImage(path)
    ├─ ImageLoader::load(path) → ImageBuffer
    └─ Store in m_original
    ↓
User selects filter, mode, threads
    ├─ Orchestrator::setFilter(FilterType)
    ├─ Orchestrator::setMode(ExecutionMode)
    └─ Orchestrator::setNumThreads(int)
    ↓
User clicks "Process Image"
    ↓
Orchestrator::processCurrentImage()
    ├─ Spawn background thread
    ├─ Measure serial baseline:
    │  └─ FilterEngine::applyTimed(m_original, SERIAL, 1) → serialMs
    ├─ Measure selected mode:
    │  └─ FilterEngine::applyTimed(m_original, m_mode, m_threads) → t, m_processed
    ├─ Create BenchRecord with metrics
    ├─ PerformanceLogger::logMessage() → "Filter applied in X ms"
    └─ m_processing = false, m_progress = 1.0f
    ↓
GUI renders processed image in ImagePreviewPanel
    ↓
User exports result (optional)
    └─ ImageLoader::save(m_processed, filename)
```

### Path 2: Video Frame Processing

```
User opens video file
    ↓
Orchestrator::loadVideo(path)
    └─ VideoLoader::load(path) → VideoLoader instance
    ↓
User configures filter/mode
    └─ (same as image)
    ↓
User clicks "Process Video"
    ↓
Orchestrator::processCurrentVideo()
    ├─ Spawn background thread
    ├─ For each frame in video:
    │  ├─ getVideoFrame(idx) → ImageBuffer
    │  ├─ FilterEngine::applyTimed(frame, m_filter, m_mode, m_threads, t)
    │  ├─ update m_progress = idx / totalFrames
    │  └─ Accumulate output frames
    ├─ VideoLoader::writeVideo("output_video.mp4", outputFrames, fps)
    └─ m_processing = false
    ↓
Output video saved to disk
```

### Path 3: Automated Benchmarking (CLI)

```
User runs: ./app --benchmark
    ↓
main() parses --benchmark flag
    ↓
BenchmarkRunner runner(config)
    ├─ config.imageSizes = {512, 1024, 2048, 4096}
    ├─ config.filters = {GAUSSIAN, SOBEL, MEDIAN, ...}
    ├─ config.modes = {SERIAL, OPENMP, CUDA}
    ├─ config.threadCounts = {1, 2, 4, 8}
    └─ config.runsPerTest = 3
    ↓
runner.runAll(&progress)
    └─ For each (imageSize, filter, mode, threadCount):
        ├─ imageBuffer = ImageLoader::createTestImage(size, size)
        ├─ For r = 1 to runsPerTest:
        │  └─ FilterEngine::applyTimed() → time[r]
        ├─ parallelMs = avg(time[])
        ├─ serialMs = measureSerialTime(filter, imageSize)
        ├─ speedup = serialMs / parallelMs
        ├─ efficiency = speedup / numThreads
        └─ Create BenchRecord
    ↓
runner.exportCSV("benchmark_results.csv")
    ↓
Print results to console and file
```

### Path 4: MPI Distributed Processing

```
User runs: mpiexec -n 4 ./app --mpi
    ↓
main() initializes MPI
    ├─ MPI_Init(&argc, &argv)
    ├─ MPI_Comm_rank() → rank
    └─ MPI_Comm_size() → size
    ↓
if (rank == 0) {
    // MASTER PROCESS
    ├─ Load image (same as GUI)
    ├─ MPIManager::init() → setup communicators
    ├─ ModeDispatcher::dispatch(image, filter, mode=MPI)
    └─ ModeDispatcher routes to MPIWorker::masterDispatch()
        ├─ MPIWorker::masterDispatch(input, FilterType)
        │  ├─ Broadcast filter type to all ranks
        │  ├─ Scatter image tiles to worker ranks
        │  ├─ Workers process their tiles
        │  ├─ Gather results from workers
        │  └─ Assemble into output image
        └─ Return result image
} else {
    // WORKER PROCESS
    └─ MPIWorker::runWorkerLoop()
        └─ while (true) {
            ├─ Receive filter type from master
            ├─ Receive image tile
            ├─ Apply filter to tile
            ├─ Send result back to master
           }
}
    ↓
MPI_Finalize()
```

---

## Filter Architecture

### Filter Hierarchy

```
IFilter (Abstract Base)
├── SerialGaussianFilter
├── SerialSobelFilter
├── SerialMedianFilter
├── ... (11 serial implementations)
│
├── OmpGaussianFilter
├── OmpSobelFilter
├── OmpMedianFilter
├── ... (13 OpenMP implementations)
│
└── (CUDA kernels)
    ├── gaussian_kernel.cu
    ├── sobel_kernel.cu
    ├── median_kernel.cu
    └── ... (5 core filters with GPU support)
```

### IFilter Interface

```cpp
class IFilter {
public:
    virtual ~IFilter() = default;
    
    // Apply filter to input image, return filtered result
    // numThreads: for OpenMP (ignored for Serial)
    virtual ImageBuffer apply(const ImageBuffer& input, 
                             int numThreads = 1) = 0;
    
    // Human-readable filter name
    virtual std::string name() const = 0;
};
```

### Example: Gaussian Blur

**Serial Implementation** (`filters/serial/gaussian_serial.cpp`):
```cpp
class SerialGaussianFilter : public IFilter {
    // 5×5 Gaussian kernel (fixed)
    static const float KERNEL[5][5];
    static const float KERNEL_SUM;
    
    ImageBuffer apply(const ImageBuffer& in, int) override {
        // Nested loops: for each pixel, convolve with kernel
        // Single-threaded, no parallelization
    }
};
```

**OpenMP Implementation** (`filters/openmp/gaussian_omp.cpp`):
```cpp
class OmpGaussianFilter : public IFilter {
    ImageBuffer apply(const ImageBuffer& in, int numThreads) override {
        omp_set_num_threads(numThreads);
        #pragma omp parallel for collapse(2)  // Parallelize pixel loops
        for (int r = 0; r < H; ++r) {
            for (int c = 0; c < W; ++c) {
                // Same convolution logic, executed in parallel
                // Each thread processes independent pixels
            }
        }
    }
};
```

**CUDA Implementation** (`cuda/gaussian_kernel.cu`):
```cuda
__global__ void gaussianKernel(float* input, float* output, 
                               int width, int height) {
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;
    
    if (x < width && y < height) {
        // Compute Gaussian convolution for pixel (x, y)
        // One thread per pixel, executed on GPU
    }
}
```

### Filter Implementation Guidelines

**To Add New Filter**:

1. **Create Serial Implementation**:
   ```
   filters/serial/my_filter_serial.h/cpp
   ```
   - Inherit from `IFilter`
   - Implement `apply()` with simple nested loops
   - No parallelism, baseline implementation

2. **Create OpenMP Variant**:
   ```
   filters/openmp/my_filter_omp.h/cpp
   ```
   - Inherit from `IFilter`
   - Copy `apply()` logic from Serial
   - Add `#pragma omp parallel for` directives

3. **(Optional) Create CUDA Kernel**:
   ```
   cuda/my_filter_kernel.cu/cuh
   ```
   - Implement `__global__` CUDA kernel
   - Allocate GPU memory, copy data, launch kernel, retrieve result

4. **Register in FilterEngine**:
   - Add to `FilterEngine::create()` factory method
   - Add case statement for each mode

5. **Update UI**:
   - Add to `FilterPanel` with label and description

---

## Parallelization Backends

### 1. Serial Execution

**Characteristics**:
- Single-threaded baseline
- Minimal overhead
- Used for speedup comparison
- Always available (no dependencies)

**Implementation**:
```cpp
// Simple nested loops
for (int r = 0; r < H; ++r) {
    for (int c = 0; c < W; ++c) {
        // Compute pixel value (no synchronization)
    }
}
```

---

### 2. OpenMP (Shared-Memory Parallelism)

**Characteristics**:
- Multi-threaded on single machine
- Shared address space (low communication overhead)
- `#pragma omp parallel for` directives
- Configurable thread count (1-64)

**Architecture**:
```
Master Thread
├─ Fork N worker threads
├─ Each thread processes subset of pixels
├─ Implicit barrier at end of parallel region
└─ Join worker threads
```

**Loop Parallelization**:
```cpp
#pragma omp parallel for schedule(dynamic, 4) collapse(2)
for (int r = 0; r < H; ++r) {
    for (int c = 0; c < W; ++c) {
        // Each (r, c) pair assigned to a thread
        // Dynamic scheduling for load balancing
    }
}
```

**Key Parameters**:
- `schedule(dynamic, 4)`: Dynamic scheduling, chunk size 4
- `collapse(2)`: Collapse nested loops for better parallelization
- Implicit thread barrier at end

---

### 3. CUDA (GPU Acceleration)

**Characteristics**:
- GPU memory separate from CPU
- Thousands of lightweight threads
- Massive parallelism
- Significant data transfer overhead

**Data Flow**:
```
Host Memory (CPU)
    │ (upload)
    ▼
GPU Memory
    │
    ├─ Kernel execution (thousands of threads)
    │
    ▼
    │ (download)
    ▼
Host Memory (CPU)
```

**GPU Manager** (`cuda/gpu_manager.h`):
```cpp
class GPUManager {
    // Device memory allocation/deallocation
    uint8_t* deviceAlloc(size_t bytes);
    void deviceFree(uint8_t* ptr);
    
    // Host ↔ Device data transfer
    void uploadToDevice(const uint8_t* host, uint8_t* device, size_t bytes);
    void downloadToHost(const uint8_t* device, uint8_t* host, size_t bytes);
    
    // Synchronization
    void sync();
};
```

**Typical CUDA Filter Pattern**:
```cpp
class CudaGaussianFilter : public IFilter {
    ImageBuffer apply(const ImageBuffer& in, int) override {
        size_t bytes = in.width * in.height * in.channels * sizeof(float);
        
        // Allocate GPU memory
        uint8_t* d_input = GPUManager::instance().deviceAlloc(bytes);
        uint8_t* d_output = GPUManager::instance().deviceAlloc(bytes);
        
        // Upload data
        GPUManager::instance().uploadToDevice(
            (uint8_t*)&in.data[0], d_input, bytes);
        
        // Launch kernel
        dim3 threads(16, 16);
        dim3 blocks(divUp(in.width, 16), divUp(in.height, 16));
        gaussianKernel<<<blocks, threads>>>(
            (float*)d_input, (float*)d_output, in.width, in.height);
        
        // Download result
        ImageBuffer out = in.clone();
        GPUManager::instance().downloadToHost(
            d_output, (uint8_t*)&out.data[0], bytes);
        
        // Cleanup
        GPUManager::instance().deviceFree(d_input);
        GPUManager::instance().deviceFree(d_output);
        
        return out;
    }
};
```

---

### 4. MPI (Distributed Computing)

**Characteristics**:
- Multiple processes, separate memory spaces
- Message passing for communication
- Works across multiple machines
- Higher latency than shared-memory

**Architecture**:

```
Master Rank (rank 0)
├─ Load input image
├─ Partition into tiles
├─ Send tiles to worker ranks
├─ Receive processed tiles from workers
└─ Assemble output image

Worker Rank 1              Worker Rank 2              Worker Rank N
├─ Receive tile            ├─ Receive tile            ├─ Receive tile
├─ Apply filter to tile    ├─ Apply filter to tile    ├─ Apply filter to tile
└─ Send result to master   └─ Send result to master   └─ Send result to master
```

**MPI Message Tags**:
```cpp
enum class Tag : int {
    TASK_FILTER = 10,   // Filter task
    TASK_BENCH = 11,    // Benchmark task
    TASK_EXIT = 99      // Shutdown signal
};
```

**Worker Loop** (`mpi/mpi_worker.cpp`):
```cpp
static void MPIWorker::runWorkerLoop() {
    while (true) {
        // Receive filter type from master
        int filterTag = receive_from_master();
        
        if (filterTag == TASK_EXIT) break;  // Shutdown
        
        // Receive image tile
        ImageBuffer tile = receive_tile_from_master();
        
        // Apply filter
        ImageBuffer processed = FilterEngine::apply(tile, ...);
        
        // Send result back
        send_to_master(processed);
    }
}
```

**Master Dispatch** (`mpi/mpi_worker.cpp`):
```cpp
ImageBuffer MPIWorker::masterDispatch(const ImageBuffer& input,
                                      FilterType ft) {
    // 1. Partition input into NxN tiles (one per rank)
    auto tiles = partition_image(input);
    
    // 2. Broadcast filter type to all workers
    broadcast(ft, ALL_WORKERS);
    
    // 3. Send tile to each worker
    for (int rank = 1; rank < worldSize; ++rank) {
        send(tiles[rank], rank);
    }
    
    // 4. Receive processed tiles from workers
    std::vector<ImageBuffer> results;
    for (int rank = 1; rank < worldSize; ++rank) {
        results[rank] = receive(rank);
    }
    
    // 5. Assemble output
    return assemble_tiles(results);
}
```

---

### 5. Hybrid Modes (MPI+OpenMP, MPI+CUDA)

**MPI+OpenMP**:
```
Rank 0  →  Tile 0  →  [4 OpenMP threads]  →  Result 0
Rank 1  →  Tile 1  →  [4 OpenMP threads]  →  Result 1
Rank 2  →  Tile 2  →  [4 OpenMP threads]  →  Result 2
Rank 3  →  Tile 3  →  [4 OpenMP threads]  →  Result 3

Total: 4 ranks × 4 threads = 16 parallel workers
```

**MPI+CUDA**:
```
Rank 0 → Tile 0 → [GPU 0 kernel]  → Result 0
Rank 1 → Tile 1 → [GPU 1 kernel]  → Result 1
Rank 2 → Tile 2 → [GPU 2 kernel]  → Result 2
Rank 3 → Tile 3 → [GPU 3 kernel]  → Result 3
```

---

## GUI Architecture

### Component Hierarchy

```
GUI (main window, ImGui context setup)
├─ Initialize GLFW (window creation)
├─ Initialize OpenGL (rendering context)
├─ Initialize ImGui (Dear ImGui context)
├─ Load fonts and styles
│
└─ Main Render Loop (blocks until window closes)
   ├─ Each frame:
   │  ├─ newFrame() (ImGui frame initialization)
   │  ├─ renderMainMenuBar()
   │  ├─ renderDockspace() (ImGui docking)
   │  ├─ renderAllPanels():
   │  │  ├─ ImagePreviewPanel::render()
   │  │  ├─ VideoPreviewPanel::render()
   │  │  ├─ FilterPanel::render()
   │  │  ├─ ModePanel::render()
   │  │  ├─ BenchmarkPanel::render()
   │  │  ├─ LogPanel::render()
   │  │  ├─ ThreadVizPanel::render()
   │  │  └─ ArchAnimPanel::render()
   │  ├─ endFrame() (rendering)
   │  └─ ~16ms per frame (60 FPS target)
```

### Panel Descriptions

#### ImagePreviewPanel
- Displays current original and processed images
- Side-by-side comparison mode
- Zoom and pan controls
- Supports PNG, JPG, BMP, TIFF formats

#### VideoPreviewPanel
- Video player interface
- Frame scrubbing
- Play/Pause/Stop controls
- Current frame indicator

#### FilterPanel
- Radio buttons for 13 filters
- Dropdown categorization (Edge, Blur, Enhancement, Tone)
- Thread count slider (1-32)
- Process button with progress indicator
- Tooltips for filter descriptions

#### ModePanel
- Dropdown for 6 execution modes (Serial, OpenMP, CUDA, MPI, MPI+OpenMP, MPI+CUDA)
- Contextual parameters (thread count for OpenMP, GPU info for CUDA)
- Hardware capability detection

#### BenchmarkPanel
- Configurable benchmark settings (image sizes, thread counts, iterations)
- Run Full Suite button
- Progress bar
- Export CSV button

#### LogPanel
- Real-time message display
- Performance metrics summary
- Color-coded message levels (OK, WARNING, ERROR)
- Scrollable message history

#### ThreadVizPanel
- Per-thread workload display
- Color-coded thread states (idle/busy/blocked)
- Timeline visualization
- Load balancing indicators

#### ArchAnimPanel
- Animated visualization of parallel execution
- Shows data flow between ranks/threads
- Highlights synchronization points
- Performance bottleneck indicators

### ImGui Integration

**Backend**:
- GLFW for window and input management
- OpenGL 3.3+ for rendering
- Dear ImGui for UI widgets

**Threading Considerations**:
- ImGui is single-threaded (main UI thread only)
- Background processing uses separate `std::thread`
- Atomic flags for thread-safe status updates
- No direct GUI calls from processing thread

---

## Data Structures

### ImageBuffer

```cpp
struct ImageBuffer {
    int width, height, channels;
    std::vector<float> data;  // Linear buffer [row-major order]
    
    // Access pixel: data[y * width * channels + x * channels + c]
    float at(int row, int col, int channel) const;
    void set(int row, int col, int channel, float value);
    ImageBuffer clone() const;
};
```

**Memory Layout**:
```
data: [R0G0B0 | R0G0B0 | ... | R1G1B1 | ... | RHG HBH]
       ↑ Pixel(0,0)     ↑ Next pixel in row
                                          ↑ Pixel(1,0)
```

**Usage**:
```cpp
ImageBuffer img = ImageLoader::load("image.png");
float red = img.at(100, 50, 0);  // Red channel at (50, 100)
img.set(100, 50, 0, 255.0f);
```

---

### FilterType Enum

```cpp
enum class FilterType {
    // Edge Detection
    SOBEL, LAPLACIAN, PREWITT,
    
    // Blur
    GAUSSIAN, BOX_BLUR, MEDIAN, BILATERAL, MOTION_BLUR,
    
    // Enhancement
    SHARPEN, EMBOSS,
    
    // Tone
    GRAYSCALE, SEPIA, BRIGHTNESS_CONTRAST
};
```

---

### ExecutionMode Enum

```cpp
enum class ExecutionMode {
    SERIAL,      // Single thread, no parallelism
    OPENMP,      // Shared-memory, multi-threaded
    CUDA,        // GPU acceleration
    MPI,         // Distributed computing
    MPI_OMP,     // Hybrid: MPI + OpenMP
    MPI_CUDA     // Hybrid: MPI + CUDA GPU
};
```

---

### BenchRecord

```cpp
struct BenchRecord {
    std::string filterName;     // e.g., "Gaussian Blur"
    std::string modeName;       // e.g., "OpenMP_4"
    int imageWidth, imageHeight;
    int numThreads;
    double serialTimeMs;        // Serial baseline
    double parallelTimeMs;      // Selected mode time
    double speedup;             // = serialTime / parallelTime
    double efficiency;          // = speedup / numThreads
    double fps;                 // = 1000 / parallelTime
};
```

---

### BenchmarkConfig

```cpp
struct BenchmarkConfig {
    std::vector<int>           imageSizes   = {512, 1024, 2048, 4096};
    std::vector<FilterType>    filters;      // Subset of 13 filters
    std::vector<ExecutionMode> modes;        // Subset of 6 modes
    std::vector<int>           threadCounts = {1, 2, 4, 8, 16};
    int                        runsPerTest  = 3;
    bool                       saveImages   = false;
    std::string                outputDir    = "./benchmark_out/";
};
```

---

## Threading & Concurrency

### Main Thread (GUI)

```cpp
int main() {
    GUI gui(orchestrator);
    gui.init(1600, 900, "ParallelVisionSuite");
    gui.run();  // Blocks here until window closes
}
```

**Responsibilities**:
- Render ImGui UI
- Handle user input (mouse, keyboard)
- Update GUI state
- Do NOT perform long-running computations

---

### Background Processing Thread

```cpp
void Orchestrator::processCurrentImage() {
    if (m_processing) return;  // Already processing
    m_processing = true;        // Atomic flag
    m_progress = 0.0f;
    
    std::thread([this]() {
        try {
            // Long-running filter computation (off main thread)
            double t = 0.0;
            m_processed = FilterEngine::applyTimed(
                m_original, m_filter, m_mode, m_threads, t);
            m_lastTimeMs = t;
        } catch (...) {
            PerformanceLogger::instance().logMessage("[ERROR] ...");
        }
        m_processing = false;  // Atomic flag
    }).detach();  // Launch and forget
}
```

**Key Points**:
- Runs asynchronously
- Uses atomic flags for thread-safe status
- GUI periodically polls `isProcessing()`, `processingProgress()`
- No direct GUI calls from processing thread

---

### Lock-Free Synchronization

**Used Pattern**: Atomic flags

```cpp
std::atomic<bool> m_processing;   // Processing in progress?
std::atomic<float> m_progress;    // Progress [0.0, 1.0]
double m_lastTimeMs;              // Last execution time
```

**Thread-Safe Access**:
```cpp
// Main thread (GUI)
if (!orch.isProcessing()) {
    orch.processCurrentImage();  // Safe to start
}
float prog = orch.processingProgress();  // Safe to read
ImGui::ProgressBar(prog);
```

**Reasoning**: 
- No mutexes (avoids contention)
- Atomic operations efficient for simple flags
- Sufficient for GUI polling pattern

---

### OpenMP Thread Management

**Handled Automatically**:
- OpenMP runtime manages thread pool
- `omp_set_num_threads(n)` before parallel region
- Implicit barrier at end of parallel region

```cpp
omp_set_num_threads(numThreads);

#pragma omp parallel for
for (int i = 0; i < n; ++i) {
    // Automatic thread allocation and synchronization
}
// Implicit barrier here (all threads must complete)
```

---

### MPI Process Management

**Handled by `mpiexec`**:
- Launches multiple processes
- Provides process rank and size
- Routes messages between processes

```bash
mpiexec -n 4 ./app --mpi
# Launches 4 separate processes (rank 0-3)
```

**Synchronization**:
- Explicit MPI calls:
  ```cpp
  MPI_Send(...);      // Blocking send
  MPI_Recv(...);      // Blocking receive
  MPI_Bcast(...);     // Collective broadcast
  MPI_Gather(...);    // Collective gather
  MPI_Barrier();      // Synchronization barrier
  ```

---

## Performance Measurement

### Timer Implementation

```cpp
class Timer {
public:
    void start();       // Record start time
    void stop();        // Record end time
    double elapsedMs(); // Return elapsed milliseconds
private:
    std::chrono::high_resolution_clock::time_point m_start, m_end;
};
```

**Usage**:
```cpp
Timer t;
t.start();
// ... do work ...
t.stop();
std::cout << "Elapsed: " << t.elapsedMs() << " ms\n";
```

---

### Metrics Collection

**In Orchestrator**:
```cpp
void Orchestrator::processCurrentImage() {
    // Serial baseline
    double serialMs = 0.0;
    FilterEngine::applyTimed(m_original, m_filter, 
                            ExecutionMode::SERIAL, 1, serialMs);
    
    // Parallel mode
    double parallelMs = 0.0;
    m_processed = FilterEngine::applyTimed(m_original, m_filter,
                                          m_mode, m_threads, parallelMs);
    
    // Calculate metrics
    double speedup = serialMs / parallelMs;
    double efficiency = speedup / m_threads * 100.0;
    
    // Create record for logging
    BenchRecord rec{
        filterName(m_filter),
        modeName(m_mode),
        m_original.width,
        m_original.height,
        m_threads,
        serialMs,
        parallelMs,
        speedup,
        efficiency,
        1000.0 / parallelMs  // FPS
    };
    
    // Log result
    PerformanceLogger::instance().addRecord(rec);
}
```

---

### CSV Export Format

**Headers**:
```
filter_name,mode_name,image_width,image_height,parallel_time_ms,serial_time_ms,speedup,efficiency,fps
```

**Example Row**:
```
Gaussian Blur,OpenMP_4,1024,1024,40.2,150.5,3.74,93.5,24.9
```

**Analysis**:
- Filter: Gaussian Blur
- Mode: OpenMP with 4 threads
- Image: 1024×1024 pixels
- Parallel time: 40.2 ms
- Serial baseline: 150.5 ms
- Speedup: 3.74× (75% of theoretical 4×)
- Efficiency: 93.5% (good utilization)
- Throughput: 24.9 frames per second

---

## Error Handling

### Strategy

**Principle**: Fail gracefully with informative messages

**Three-Level Error Response**:

1. **Try-Catch (C++ Exceptions)**
   ```cpp
   try {
       FilterEngine::applyTimed(...);
   } catch (const std::exception& e) {
       PerformanceLogger::instance().logMessage(
           "[ERROR] Filter failed: " + std::string(e.what()));
   }
   ```

2. **Logging (PerformanceLogger)**
   ```cpp
   if (!GPUManager::instance().init()) {
       PerformanceLogger::instance().logMessage(
           "[WARNING] GPU not available, using fallback");
   }
   ```

3. **User Feedback (GUI)**
   - Error messages appear in LogPanel
   - Application continues running
   - User can retry with different parameters

---

### Common Error Scenarios

| Scenario | Handling |
|----------|----------|
| GPU unavailable | Fallback to Serial/OpenMP from CUDA |
| Image file not found | Log error, do nothing (user reloads) |
| Invalid thread count | Clamp to valid range (1-CPU cores) |
| Out of GPU memory | Log "GPU memory full", use CPU fallback |
| MPI initialization failed | Log error, continue in serial mode |
| Filter application error | Log exception message, keep UI responsive |

---

### CUDA Error Checking

```cpp
void GPUManager::init() {
    cudaDeviceProp prop;
    cudaError_t err = cudaGetDeviceProperties(&prop, 0);
    
    if (err != cudaSuccess) {
        PerformanceLogger::instance().logMessage(
            "[ERROR] CUDA init failed: " + 
            std::string(cudaGetErrorString(err)));
        m_available = false;
        return;
    }
    
    m_available = true;
}
```

---

### MPI Error Handling

```cpp
int rank, size;
MPI_Comm_rank(MPI_COMM_WORLD, &rank);
MPI_Comm_size(MPI_COMM_WORLD, &size);

if (size < 2) {
    PerformanceLogger::instance().logMessage(
        "[ERROR] MPI mode requires at least 2 ranks");
    // Fallback to serial mode
    return;
}
```

---

## Extensibility Guide

### Adding a New Filter

**Step 1: Create Serial Implementation**

File: `filters/serial/my_filter_serial.h`
```cpp
#pragma once
#include "filter_engine.h"

class SerialMyFilter : public IFilter {
public:
    ImageBuffer apply(const ImageBuffer& input, int) override;
    std::string name() const override { return "My Filter (Serial)"; }
};
```

File: `filters/serial/my_filter_serial.cpp`
```cpp
#include "my_filter_serial.h"

ImageBuffer SerialMyFilter::apply(const ImageBuffer& input, int) {
    ImageBuffer output = input.clone();
    const int W = input.width, H = input.height, C = input.channels;
    
    for (int r = 0; r < H; ++r) {
        for (int c = 0; c < W; ++c) {
            for (int ch = 0; ch < C; ++ch) {
                // Apply filter logic to output.at(r, c, ch)
            }
        }
    }
    
    return output;
}
```

**Step 2: Create OpenMP Variant**

File: `filters/openmp/my_filter_omp.h`
```cpp
#pragma once
#include "filter_engine.h"

class OmpMyFilter : public IFilter {
public:
    ImageBuffer apply(const ImageBuffer& input, int numThreads) override;
    std::string name() const override { return "My Filter (OpenMP)"; }
};
```

File: `filters/openmp/my_filter_omp.cpp`
```cpp
#include "my_filter_omp.h"
#include <omp.h>

ImageBuffer OmpMyFilter::apply(const ImageBuffer& input, int numThreads) {
    ImageBuffer output = input.clone();
    const int W = input.width, H = input.height, C = input.channels;
    
    omp_set_num_threads(numThreads);
    
    #pragma omp parallel for collapse(2)
    for (int r = 0; r < H; ++r) {
        for (int c = 0; c < W; ++c) {
            for (int ch = 0; ch < C; ++ch) {
                // Same filter logic (automatically parallelized)
            }
        }
    }
    
    return output;
}
```

**Step 3: Add to FilterType Enum**

File: `filters/filter_engine.h`
```cpp
enum class FilterType {
    // ... existing filters ...
    MY_FILTER,  // New filter
};
```

**Step 4: Register in FilterEngine**

File: `filters/filter_engine.cpp`
```cpp
#include "serial/my_filter_serial.h"
#include "openmp/my_filter_omp.h"

IFilter* FilterEngine::create(FilterType type, ExecutionMode mode) {
    bool useOmp = (mode == ExecutionMode::OPENMP || mode == ExecutionMode::MPI_OMP);
    
    if (useOmp) {
        switch (type) {
            case FilterType::MY_FILTER: return new OmpMyFilter();
            // ... existing cases ...
        }
    }
    
    // Default to Serial
    switch (type) {
        case FilterType::MY_FILTER: return new SerialMyFilter();
        // ... existing cases ...
    }
}
```

**Step 5: Add Filter Name**

File: `filters/filter_engine.h`
```cpp
inline std::string filterName(FilterType f) {
    switch (f) {
        case FilterType::MY_FILTER: return "My Filter";
        // ... existing cases ...
    }
    return "Unknown";
}
```

**Step 6: Update UI**

File: `gui/panels/filter_panel.cpp`
```cpp
void FilterPanel::render() {
    // In the filters list/dropdown:
    static const struct {
        const char* label;
        FilterType type;
        const char* desc;
    } kFilters[] = {
        // ... existing entries ...
        { "My Filter", FilterType::MY_FILTER, "Description of my filter" },
    };
    
    // ... rendering code ...
}
```

---

### Adding a New Execution Mode

**Step 1: Add to ExecutionMode Enum**

```cpp
enum class ExecutionMode {
    // ... existing modes ...
    MY_MODE,
};
```

**Step 2: Add Mode Name**

```cpp
inline std::string modeName(ExecutionMode m) {
    switch (m) {
        case ExecutionMode::MY_MODE: return "My Mode";
        // ... existing cases ...
    }
}
```

**Step 3: Implement in FilterEngine or ModeDispatcher**

Either extend `FilterEngine::create()` for built-in modes, or extend `ModeDispatcher::dispatch()` for complex routing.

**Step 4: Add UI Option**

```cpp
// In ModePanel::render()
static const char* modeOptions[] = {
    "Serial", "OpenMP", "CUDA", "MPI", "MPI+OpenMP", "MPI+CUDA", "My Mode"
};
```

---

### Performance Optimization Considerations

**When Optimizing**:

1. **Profile First**: Identify bottlenecks with benchmarking
2. **Minimal Changes**: Optimize one component at a time
3. **Preserve Correctness**: Verify results match serial baseline
4. **Test Scaling**: Verify improvement with different thread counts
5. **Document**: Add comments explaining optimization strategy

**Common Optimization Points**:
- Loop parallelization strategy (schedule type)
- Memory access patterns (cache locality)
- GPU kernel configuration (threads per block)
- MPI communication (overlapping with computation)
- Buffer management (reduce allocations)

---

## Summary

ParallelVisionSuite follows clean architecture principles with clear separation of concerns, enabling:

- **Portability**: Works on Windows, Linux, macOS
- **Scalability**: Supports 1 to 1000+ parallel workers (depending on mode)
- **Maintainability**: Modular design, easy to extend
- **Benchmarkability**: Precise performance measurement across all modes
- **Usability**: Both GUI and CLI interfaces

The combination of Factory, Strategy, and Dependency Injection patterns creates a flexible system for comparing parallel image processing techniques at scale.
