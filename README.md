# ParallelVisionSuite

A high-performance parallel image processing application demonstrating various parallelization techniques including **Serial**, **OpenMP**, **CUDA** (GPU), and **MPI** (distributed computing) implementations.

## 📋 Overview

ParallelVisionSuite is an educational and research-oriented tool for analyzing and comparing the performance of different image filtering algorithms across multiple execution paradigms. It provides both a graphical user interface (GUI) and command-line interfaces for benchmarking parallel image processing.

### Key Capabilities

- **13 Comprehensive Image Filters**: Including edge detection, blur, artistic effects, and enhancement filters
- **Multiple Execution Modes**: Serial, OpenMP (shared-memory), CUDA (GPU), MPI (distributed), and hybrid modes (MPI_OMP, MPI_CUDA)
- **Real-time Performance Monitoring**: Live visualization of execution time, speedup, efficiency, and FPS metrics
- **Automated Benchmarking**: Full suite benchmarking with multiple image sizes (512x512, 1024x1024, 2048x2048)
- **Interactive GUI**: ImGui-based interface with live image preview, video support, and performance panels
- **CSV Export**: Detailed benchmark results exportable for analysis
- **Advanced Visualization**: Architecture animation, thread utilization visualization, and performance charts via ImPlot
- **Video Support**: Process video files with real-time playback
- **Session Management**: Track and analyze multiple processing sessions

---

## 📖 Documentation Guide

This README is designed to help users at different levels:

### 👤 For New Users
1. Start with [System Requirements](#-system-requirements)
2. Follow [Build Instructions](#-build-instructions)
3. Try [GUI Mode](#-gui-usage-guide) with sample images
4. Run [Quick Verification](#-quick-verification-checklist)

### 🔬 For Researchers/Benchmarkers
1. Review [Supported Filters](#-supported-image-filters) to understand available algorithms
2. Learn [Execution Modes](#-execution-modes) to choose optimal configuration
3. Use [CLI Benchmarking](#-command-line-interface-cli) for automated testing
4. Analyze results using [Benchmark Output Format](#-benchmark-output-format)
5. Check [Performance Optimization Tips](#-performance-optimization-tips)

### 🏢 For Cluster/HPC Users
1. Focus on [MPI Modes](#4-mpi-mode) and [Hybrid Modes](#5-mpi_omp-mode-hybrid)
2. Review [Troubleshooting](#runtime-issues---parallel-execution) for MPI setup
3. Use [Performance Optimization](#mpi-optimization) for cluster tuning

### 🐛 For Developers
1. Examine [Project Structure](#-project-structure) for codebase organization
2. See [architecture.md](docs/architecture.md) for design details
3. Check [Contributing](#contributing) guidelines
4. Review [Feature Matrix](#-feature-matrix) for implementation coverage

---

## �️ Supported Image Filters

ParallelVisionSuite includes **13 distinct image processing filters**, each implemented in Serial, OpenMP, and (for core filters) CUDA versions:

### Edge Detection Filters

| Filter | Description | Use Case | Implementation |
|--------|-------------|----------|-----------------|
| **Sobel Edge Detection** | Detects horizontal and vertical edges using Sobel operators | Edge detection in computer vision, object boundary identification | Serial, OpenMP, CUDA |
| **Laplacian Edge Detection** | Second-order derivative edge detector | Fine-grained edge detection, corner and feature detection | Serial, OpenMP, CUDA |
| **Prewitt Edge Detection** | Similar to Sobel but with different kernel weights | Alternative edge detection with directional sensitivity | Serial, OpenMP |

### Blur Filters

| Filter | Description | Use Case | Implementation |
|--------|-------------|----------|-----------------|
| **Gaussian Blur** | Smooth blur using Gaussian kernel (5×5) | Noise reduction, preprocessing, image smoothing | Serial, OpenMP, CUDA |
| **Box Blur** | Uniform blur using simple averaging | Fast general-purpose blur, pixelation effects | Serial, OpenMP |
| **Median Filter** | Non-linear filter preserving edges while reducing noise | Salt-and-pepper noise removal, medical imaging | Serial, OpenMP, CUDA |
| **Bilateral Filter** | Preserves edges while smoothing within regions | Edge-aware smoothing, stylization | Serial, OpenMP |
| **Motion Blur** | Directional blur simulating motion | Artistic effects, motion simulation | Serial, OpenMP |

### Enhancement Filters

| Filter | Description | Use Case | Implementation |
|--------|-------------|----------|-----------------|
| **Sharpen** | Enhances fine details and edges | Increase image crispness, detail enhancement | Serial, OpenMP, CUDA |
| **Emboss** | Creates 3D relief effect | Artistic effects, texture highlighting | Serial, OpenMP |
| **Brightness/Contrast** | Adjusts image brightness and contrast levels | Exposure correction, image enhancement | Serial, OpenMP |

### Color/Tone Filters

| Filter | Description | Use Case | Implementation |
|--------|-------------|----------|-----------------|
| **Grayscale** | Converts RGB to grayscale | Color space conversion, preprocessing | Serial, OpenMP |
| **Sepia Tone** | Applies warm sepia color tone | Retro/vintage photo effects | Serial, OpenMP |

All filters support configurable parameters and real-time preview in the GUI. Each can be executed using any available execution mode for performance comparison.

---

### Hardware
- **CPU**: Multi-core processor (4+ cores recommended for OpenMP benchmarking)
- **GPU**: NVIDIA GPU with CUDA Compute Capability 3.5+ (optional, for CUDA mode)
- **RAM**: 4 GB minimum (8 GB+ recommended for large image processing)
- **Storage**: 2 GB for compilation and dependencies

### Software

| Component | Version | Required | Notes |
|-----------|---------|----------|-------|
| **C++ Compiler** | C++17 compatible | ✓ | MSVC (Visual Studio 2022), GCC 9+, or Clang 10+ |
| **CMake** | 3.20+ | ✓ | Build system configuration |
| **OpenCV** | 4.x | ✓ | Image I/O and processing utilities |
| **OpenGL** | 3.3+ | ✓ | Rendering backend for GUI |
| **GLFW3** | 3.3+ | ✓ | Window and input management |
| **OpenMP** | 4.x+ | ✓ | Shared-memory parallelization (parallel filters) |
| **MS-MPI** | 10.1+ | ✗ | Distributed computing (Windows, optional) |
| **OpenMPI** | 3.0+ | ✗ | Distributed computing (Linux/macOS, optional) |
| **CUDA Toolkit** | 12.x+ | ✗ | GPU acceleration (requires compatible NVIDIA GPU) |

---

## 📦 Dependencies Installation

### Windows (MSYS2/MinGW64)

```bash
# Update package manager
pacman -Syu

# Install compiler and build tools
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake

# Install OpenCV
pacman -S mingw-w64-x86_64-opencv

# Install OpenGL and GLFW3
pacman -S mingw-w64-x86_64-glfw mingw-w64-x86_64-glew

# Install OpenMP
pacman -S mingw-w64-x86_64-openmp
```

### Windows (Visual Studio)

```bash
# Using vcpkg package manager
vcpkg install opencv:x64-windows glfw3:x64-windows
```

### Linux (Ubuntu/Debian)

```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    libopencv-dev \
    libglfw3-dev \
    libglew-dev \
    libomp-dev \
    libopenmpi-dev
```

### macOS

```bash
brew install cmake opencv glfw3 open-mpi
```

---

## 🏗️ Build Instructions

### Quick Start (Windows)

```bash
# Navigate to project directory
cd ParallelVisionSuite

# Configure with CMake
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

# Build (using 8 parallel jobs)
cmake --build build --config Release -j8

# Verify build success
ls build/ParallelVisionSuite.exe  # Should exist
```

### Using CMake Presets (Recommended)

```bash
# List available presets
cmake --list-presets

# Configure using preset (Windows)
cmake --preset windows-release

# Build using preset
cmake --build --preset windows-release
```

### Build Variants

```bash
# Release Build (Optimized, better performance)
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release -j8

# Debug Build (Slower, detailed debugging info)
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug -j4

# Clean Build (Remove previous build artifacts)
rm -rf build  # or rmdir /s build on Windows
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release -j8
```

---

## 🚀 Execution Commands

### GUI Mode (Default)

Launch the interactive graphical interface:

```bash
# Windows
./build/ParallelVisionSuite.exe

# Linux/macOS
./build/ParallelVisionSuite

# Auto-launch with run_app.bat (Windows)
./run_app.bat
```

**GUI Features:**
- Load and process images in real-time from disk
- Load and play video files with frame-by-frame processing
- Select from 13 different image filters (Gaussian, Sobel, Median, Laplacian, Sharpen, Box Blur, Emboss, Sepia, Grayscale, Brightness/Contrast, Bilateral, Motion Blur, Prewitt)
- Choose execution mode (Serial, OpenMP, CUDA, MPI, MPI_OMP, MPI_CUDA)
- Adjust thread count for OpenMP-based modes
- View real-time performance metrics (execution time, speedup, efficiency, FPS)
- Run full benchmark suite across all filters and modes
- Export results to CSV for analysis
- Visualize thread utilization in real-time
- View architecture animation showing parallel execution patterns
- Monitor performance logs and debug information
- Session management with full history tracking

---

## ⚙️ Execution Modes

ParallelVisionSuite supports **6 execution modes** for flexible performance analysis:

### 1. Serial Mode
Single-threaded baseline execution for performance comparison.
- **Best for**: Establishing baseline performance metrics
- **CLI**: `./build/ParallelVisionSuite.exe --serial`
- **All 13 filters supported**

### 2. OpenMP Mode
Multi-threaded shared-memory parallelization using OpenMP directives.
- **Best for**: Multi-core CPU systems with shared memory
- **CLI**: `./build/ParallelVisionSuite.exe --omp <threads>`
- **All 13 filters supported**
- **Thread count**: 1-64 (typically matches physical CPU cores)
- **Example**: `./build/ParallelVisionSuite.exe --omp 8`

### 3. CUDA Mode
GPU acceleration using NVIDIA CUDA compute kernels.
- **Best for**: Large image processing with compatible NVIDIA GPUs
- **Requirements**: CUDA Toolkit 12.x+, NVIDIA GPU (Compute Capability 3.5+)
- **CLI**: `./build/ParallelVisionSuite.exe --cuda`
- **Supported filters**: Gaussian, Sobel, Median, Laplacian, Sharpen (core filters with optimized kernels)
- **Fallback**: Automatically falls back to Serial mode if GPU unavailable

### 4. MPI Mode
Distributed computing across multiple processes/machines using Message Passing Interface.
- **Best for**: Large-scale computing across multiple nodes
- **Requirements**: MS-MPI (Windows) or OpenMPI (Linux/macOS)
- **CLI**: `mpiexec -n <ranks> ./build/ParallelVisionSuite.exe --mpi`
- **Example**: `mpiexec -n 4 ./build/ParallelVisionSuite.exe --mpi`
- **All 13 filters supported**

### 5. MPI_OMP Mode (Hybrid)
Combines MPI distributed computing with OpenMP shared-memory parallelization.
- **Best for**: Large clusters with multi-core nodes
- **CLI**: `mpiexec -n <ranks> ./build/ParallelVisionSuite.exe --mpi-omp <threads>`
- **Example**: `mpiexec -n 4 ./build/ParallelVisionSuite.exe --mpi-omp 4`
- **Scaling**: 4 MPI ranks × 4 OpenMP threads = 16 total parallel workers
- **All 13 filters supported**

### 6. MPI_CUDA Mode (Hybrid)
Combines MPI distributed computing with GPU acceleration per node.
- **Best for**: GPU clusters with multiple compute nodes
- **Requirements**: MPI + CUDA compatible environments
- **CLI**: `mpiexec -n <ranks> ./build/ParallelVisionSuite.exe --mpi-cuda`
- **Example**: `mpiexec -n 4 ./build/ParallelVisionSuite.exe --mpi-cuda`
- **Supported filters**: Core filters with CUDA kernels
- **Automatic fallback**: Uses OpenMP or Serial if GPU unavailable

---

---

## 🏗️ Architecture Overview

ParallelVisionSuite follows a modular architecture with clear separation of concerns:

### Core Components

**1. Filter Engine** (`filters/filter_engine.cpp`)
- Central abstraction for all image processing filters
- Factory pattern: Creates appropriate filter implementation (Serial/OpenMP/CUDA) based on execution mode
- Unified interface (`IFilter` base class) for all filter implementations
- Handles timing and performance measurement

**2. Execution Orchestrator** (`src/orchestrator.cpp`)
- Routes execution to correct parallelization backend
- Manages resource allocation (threads, GPU memory, MPI ranks)
- Coordinates between different execution modes
- Handles fallback scenarios (e.g., CUDA→OpenMP→Serial)

**3. Mode Dispatcher** (`src/mode_dispatcher.cpp`)
- Parses command-line arguments
- Determines execution mode and parameters
- Delegates to appropriate handler (GUI, CLI, Benchmark)
- Validates mode-specific requirements

**4. Session Manager** (`src/session_manager.cpp`)
- Maintains session history with multiple filter runs
- Tracks performance metrics across operations
- Enables session save/load functionality
- Supports undo/redo operations

### Execution Paths

```
Input Image
    ↓
[Mode Dispatcher] ← Determines execution mode from CLI/GUI
    ↓
[Orchestrator] ← Manages overall execution
    ↓
┌───────────────────────────────────────────────┐
│ Filter Engine (Factory Pattern)              │
├───────────────────────────────────────────────┤
│  ├─ Serial Implementation (1 thread)          │
│  ├─ OpenMP Implementation (N threads)         │
│  ├─ CUDA Implementation (GPU)                 │
│  ├─ MPI Implementation (Multiple ranks)       │
│  └─ Hybrid Implementations (MPI+OpenMP/CUDA)  │
└───────────────────────────────────────────────┘
    ↓
[Performance Logger] ← Records execution metrics
    ↓
Result Image + Performance Metrics
```

### Data Flow

1. **Image Loading**: `ImageLoader` reads file → `ImageBuffer` in-memory representation
2. **Filter Processing**: `FilterEngine` selects implementation → Computation happens
3. **Result Output**: Filtered `ImageBuffer` → Display in GUI or export
4. **Metrics Collection**: `PerformanceLogger` records time, speedup, efficiency

### Memory Management

- **Serial/OpenMP**: Stack-allocated `ImageBuffer` or heap with RAII
- **CUDA**: GPU memory managed by `gpu_manager.cpp` with automatic cleanup
- **MPI**: Distributed memory on worker ranks with synchronization barriers

---

## 🔄 Data Structures

### ImageBuffer
```cpp
struct ImageBuffer {
    int width, height, channels;  // Dimensions
    std::vector<float> data;      // Flattened pixel data (0-255 range)
    
    // Methods
    float at(int row, col, channel) const;
    void set(int row, col, channel, float val);
    ImageBuffer clone() const;
};
```

### PerformanceMetrics
```cpp
struct Metrics {
    double execution_time_ms;
    double serial_time_ms;
    double speedup;      // = serial_time / parallel_time
    double efficiency;   // = speedup / num_threads
    double fps;          // = 1000 / execution_time
};
```

### FilterType Enum
Defines all 13 available filters (Gaussian, Sobel, Median, Laplacian, Sharpen, Box Blur, Emboss, Sepia, Grayscale, Brightness, Bilateral, MotionBlur, Prewitt)

### ExecutionMode Enum
Defines 6 execution modes: Serial, OpenMP, CUDA, MPI, MPI_OMP, MPI_CUDA

---

## 🔌 extensibility

### Adding a New Filter

1. Create header file: `filters/serial/your_filter_serial.h`
   ```cpp
   class SerialYourFilter : public IFilter {
   public:
       ImageBuffer apply(const ImageBuffer& in, int threads) override;
       std::string name() const override { return "Your Filter (Serial)"; }
   };
   ```

2. Implement: `filters/serial/your_filter_serial.cpp`

3. Create OpenMP variant: `filters/openmp/your_filter_omp.h/cpp`

4. Register in `FilterEngine::create()` method (filter_engine.cpp)

5. Add UI entry in `FilterPanel::render()` (gui/panels/filter_panel.cpp)

### Adding a New Execution Mode

1. Implement executor class inheriting from `IExecutor` interface
2. Add to `ExecutionMode` enum (filter_engine.h)
3. Implement mode logic in `Orchestrator::execute()`
4. Register in `ModeDispatcher`
5. Add CLI argument handling in `main.cpp`

---

## 📂 Project Structure

```
ParallelVisionSuite/
├── CMakeLists.txt                    # Main build configuration
├── CMakePresets.json                 # CMake preset configurations
├── README.md                         # Comprehensive documentation
├── run_app.bat                       # Quick launcher script (Windows)
│
├── src/                              # Core application logic
│   ├── main.cpp                      # Entry point and argument parsing
│   ├── orchestrator.cpp/h            # Execution mode coordinator
│   ├── mode_dispatcher.cpp/h         # Route to correct execution path
│   └── session_manager.cpp/h         # Session state and history
│
├── filters/                          # Image filtering algorithms
│   ├── filter_engine.cpp/h           # Unified filter interface
│   ├── serial/                       # Single-threaded implementations
│   │   ├── gaussian_serial.cpp/h
│   │   ├── sobel_serial.cpp/h
│   │   ├── median_serial.cpp/h
│   │   ├── laplacian_serial.cpp/h
│   │   └── sharpen_serial.cpp/h
│   └── openmp/                       # OpenMP parallel implementations
│       ├── gaussian_omp.cpp/h
│       ├── sobel_omp.cpp/h
│       ├── median_omp.cpp/h
│       ├── laplacian_omp.cpp/h
│       └── sharpen_omp.cpp/h
│
├── cuda/                             # NVIDIA GPU acceleration (CUDA kernels)
│   ├── gpu_manager.cpp/h             # CUDA device management
│   ├── gaussian_kernel.cu/cuh        # Gaussian blur CUDA kernel
│   ├── sobel_kernel.cu/cuh           # Sobel edge detection kernel
│   ├── median_kernel.cu/cuh          # Median filter kernel
│   ├── laplacian_kernel.cu/cuh       # Laplacian filter kernel
│   └── sharpen_kernel.cu/cuh         # Sharpen filter kernel
│
├── mpi/                              # Distributed computing (MPI)
│   ├── mpi_manager.cpp/h             # MPI initialization and control
│   └── mpi_worker.cpp/h              # MPI worker process logic
│
├── gui/                              # Graphical User Interface
│   ├── gui.cpp/h                     # Main GUI framework
│   ├── gl_texture.cpp/h              # OpenGL texture management
│   └── panels/                       # UI components
│       ├── image_preview_panel.cpp/h    # Image display
│       ├── video_preview_panel.cpp/h    # Video player
│       ├── filter_panel.cpp/h           # Filter selection
│       ├── mode_panel.cpp/h             # Execution mode selector
│       ├── benchmark_panel.cpp/h        # Benchmark controls
│       ├── log_panel.cpp/h              # Performance and debug logs
│       ├── thread_viz_panel.cpp/h       # Thread utilization visualization
│       └── arch_anim_panel.cpp/h        # Architecture animation
│
├── benchmark/                        # Performance testing framework
│   ├── benchmark_runner.cpp/h        # Orchestrates benchmark runs
│   └── performance_logger.cpp/h      # Metrics collection and logging
│
├── utils/                            # Utility functions
│   ├── image_loader.cpp/h            # PNG/JPG image I/O
│   ├── video_loader.cpp/h            # Video file support
│   ├── image_buffer.h                # Image data structure
│   ├── timer.h                       # Millisecond-precision timing
│   ├── csv_writer.cpp/h              # CSV output for results
│   └── asset_generator.cpp/h         # Procedural asset generation
│
├── third_party/                      # External libraries (bundled)
│   ├── imgui/                        # Dear ImGui GUI framework
│   ├── implot/                       # ImPlot - Chart visualization
│   ├── glad/                         # OpenGL loader
│   └── stb/                          # Single-header utility libraries
│
├── docs/                             # Documentation
│   ├── architecture.md               # System design
│   └── user_guide.md                 # User documentation
│
├── assets/                           # Static resources
│   ├── fonts/                        # TrueType fonts for GUI
│   └── images/                       # Sample images and icons
│
└── build/                            # Compilation artifacts (generated)
    ├── ParallelVisionSuite.exe       # Final executable
    ├── CMakeCache.txt                # CMake configuration cache
    └── CMakeFiles/                   # Build system internals
```

---

## 💻 Command-Line Interface (CLI)

### Getting Help

```bash
./build/ParallelVisionSuite.exe --help
```

Shows all available command-line options and usage examples.

---

### Serial Benchmark

Run image filters in single-threaded mode:

```bash
./build/ParallelVisionSuite.exe --serial
```

**Output:**
- Processes all 13 filters on standard image sizes (512x512, 1024x1024, 2048x2048)
- Each filter runs 3 iterations, averages the timings
- Generates `benchmark_results.csv` with results
- Prints timing data and performance analysis to console

---

### OpenMP Benchmark

Benchmark using multi-threaded shared-memory parallelization:

```bash
# Run with 4 threads
./build/ParallelVisionSuite.exe --omp 4

# Run with 8 threads (typical for octa-core systems)
./build/ParallelVisionSuite.exe --omp 8

# Auto-detect CPU core count and use all available cores
./build/ParallelVisionSuite.exe --omp $(nproc)     # Linux/macOS
./build/ParallelVisionSuite.exe --omp %NUMBER_OF_PROCESSORS%  # Windows PowerShell
```

**Thread Count Recommendations:**
- **2 threads**: Dual-core CPUs or for overhead testing
- **4 threads**: Typical quad-core systems
- **8 threads**: Octa-core systems or servers
- **16+ threads**: High-end workstations or servers
- **Best practice**: Set to number of physical CPU cores (not hyperthreading count)

**Performance Notes:**
- OpenMP overhead is noticeable on small images (<512x512)
- Scales better with larger images (1024x1024+)
- Parallel efficiency typically 70-95% on modern CPUs

---

### CUDA Benchmark

GPU-accelerated filtering (requires NVIDIA GPU and CUDA Toolkit):

```bash
./build/ParallelVisionSuite.exe --cuda
```

**Requirements:**
- NVIDIA GPU with Compute Capability 3.5 or higher
- CUDA Toolkit 12.x installed
- Nvidia drivers up to date

**Supported Filters (with optimized kernels):**
- Gaussian Blur
- Sobel Edge Detection
- Median Filter
- Laplacian Edge Detection
- Sharpen

**Performance Notes:**
- GPU excels on large images (2048x2048+)
- Significant speedup (10-100x) depending on GPU and problem size
- Initial data transfer overhead (~1-2ms) amortized over larger workloads
- Fallback to Serial mode if GPU unavailable

**GPU Verification:**
```bash
nvidia-smi          # Check GPU availability
nvcc --version      # Verify CUDA Toolkit
```

---

### Complete Automated Benchmark Suite

Run comprehensive benchmark across all available execution modes and image sizes:

```bash
./build/ParallelVisionSuite.exe --benchmark
```

**Suite Configuration:**
- Serial baseline execution
- OpenMP: 2, 4, 6, 8 threads (auto-scaled to CPU cores)
- CUDA: If GPU available, includes optimized kernel tests
- All 13 filters tested
- Image sizes: 512×512, 1024×1024, 2048×2048
- Multiple iterations per configuration (averages taken)

**Output:**
- `benchmark_results.csv` - Comprehensive results matrix (columns: filter, mode, image dimensions, time, speedup, efficiency)
- Console output with speedup analysis and efficiency metrics
- Total benchmark time: ~5-30 minutes depending on system

---

### MPI Distributed Mode

Run application in distributed computing mode across multiple nodes:

```bash
# Single machine with 4 MPI ranks
mpiexec -n 4 ./build/ParallelVisionSuite.exe --mpi

# Multiple machines
mpiexec -hosts host1,host2,host3,host4 -n 4 ./build/ParallelVisionSuite.exe --mpi

# With verbose output for debugging
mpiexec -n 4 -v ./build/ParallelVisionSuite.exe --mpi
```

**MPI Architecture:**
- Rank 0: Master process (coordinates, distributes work, aggregates results)
- Ranks 1-N: Worker processes (process image tiles in parallel)
- Automatic load balancing across ranks
- Synchronization barriers between phases

**Scalability Notes:**
- Optimal with 2, 4, 8, or 16 ranks
- Network latency becomes significant with >32 ranks
- Best for large images (2048x2048+) to amortize communication overhead

---

### Hybrid MPI+OpenMP Mode

Combine distributed computing with shared-memory parallelization:

```bash
# 4 MPI ranks × 4 OpenMP threads = 16 total parallel workers
mpiexec -n 4 ./build/ParallelVisionSuite.exe --mpi-omp 4

# 8 MPI ranks × 2 OpenMP threads = 16 total parallel workers
mpiexec -n 8 ./build/ParallelVisionSuite.exe --mpi-omp 2
```

**Best For:**
- Clusters with multi-core nodes (common architecture)
- Scaling to 100+ cores across multiple machines
- Testing hierarchical parallelism

**Scaling Strategy:**
- Use 1 MPI rank per node, OpenMP threads = CPU cores per node
- Or use multiple MPI ranks per node for finer-grained parallelism
- Balance load to avoid bottlenecks

---

### Hybrid MPI+CUDA Mode

Combine distributed computing with GPU acceleration per node:

```bash
# Each of 4 nodes processes with its GPU
mpiexec -n 4 ./build/ParallelVisionSuite.exe --mpi-cuda
```

**Best For:**
- GPU clusters with multiple compute nodes
- Processing large-scale image datasets
- Research computing with heterogeneous architectures

---



### Step 1: Launch Application
```bash
./build/ParallelVisionSuite.exe
```

Window opens with multiple panels arranged in a layout.

### Step 2: Load an Image
1. Click **File → Open Image** (or Ctrl+O)
2. Enter full path to an image file:
   ```
   assets/images/sample.jpg
   C:\Users\YourName\Pictures\photo.png
   /home/user/images/test.jpg
   ```
3. Click **Load** — image appears in Image Preview panel

### Step 3: Configure Filter
1. In **Filter Panel**:
   - Select from 13 available filters:
     - **Edge Detection**: Gaussian Blur, Sobel Edge, Laplacian Edge, Prewitt Edge
     - **Blur**: Box Blur, Median Filter, Bilateral Filter, Motion Blur
     - **Enhancement**: Sharpen, Emboss, Brightness/Contrast
     - **Tone**: Grayscale, Sepia Tone
   - Adjust filter-specific parameters (kernel size, sigma, angle, etc.)
   - Enable/disable edge preservation modes

### Step 4: Select Execution Mode
1. In **Mode Panel**:
   - Choose execution mode: Serial | OpenMP | CUDA | MPI | MPI_OMP | MPI_CUDA
   - For OpenMP-based modes: set thread count (recommended: 2, 4, 8, or physical CPU cores)
   - For MPI mode: configure number of MPI ranks
   - View recommended settings based on system capabilities

### Step 5: Process Image
1. Click **Process Image** button
2. Processing starts (progress bar shows status)
3. Result displays in Image Preview
4. Performance metrics shown in real-time:
   - Execution time (ms)
   - Speedup vs. serial baseline
   - Efficiency (speedup / thread count)
   - FPS (frames per second)

### Step 6: Benchmark (Optional)
1. Click **Benchmark → Run Full Suite**
2. Suite tests all combinations:
   - Each filter with each mode
   - Image sizes: 512x512, 1024x1024, 2048x2048
   - Multiple runs (average taken)

### Step 7: Export Results
1. Click **File → Export Results** or **Benchmark → Export CSV**
2. Results saved to `benchmark_results.csv`
3. Open in spreadsheet software for analysis

### Step 8: View Logs
1. **Log Panel** shows:
   - Processing status messages
   - Performance statistics
   - Debug information
   - Compilation and runtime messages
   - Filter execution details
   - Memory allocation/deallocation events

### Advanced GUI Features

**Image Preview Panel:**
- Real-time preview of processed images
- Zoom in/out functionality
- Pan and scroll for large images
- Format support: PNG, JPG, BMP, TIFF
- Side-by-side before/after comparison

**Video Preview Panel:**
- Load and play video files
- Frame-by-frame processing capability
- Progress bar with time scrubbing
- Supported formats: MP4, AVI, MOV
- Frame rate display and control

**Performance Visualization:**
- Real-time speedup graph (Serial vs Parallel)
- Efficiency metric display
- FPS counter with historical data
- Memory usage tracking

**Thread Utilization Visualization:**
- Per-thread workload display
- Color-coded thread states (idle, busy, blocked)
- Synchronization point visualization
- Load balancing analysis

**Architecture Animation Panel:**
- Visual representation of parallel execution flow
- Color-coded execution phases
- Communication pattern visualization
- Performance bottleneck highlighting

---

## 📊 Performance Metrics Explained

### Execution Time
Measured in milliseconds (ms) from filter start to completion.

- **Serial time**: Single-threaded baseline
- **Parallel time**: Multi-threaded/GPU-accelerated time
- **Includes**: Data loading, computation, result aggregation
- **Excludes**: Initial application startup, I/O operations (in CLI mode)

### Speedup
The ratio of serial execution time to parallel execution time.

- **Formula**: `Speedup = T_serial / T_parallel`
- **Ideal speedup on N cores**: N (linear scaling)
- **Speedup > N**: Indicates cache effects or methodology differences
- **Typical achievable**: 60-95% of theoretical maximum on modern hardware
- **Example**: 8-core system processing 1024×1024 image with Gaussian filter typically achieves 6.5-7.5× speedup

### Efficiency
Measure of how effectively parallel resources are being utilized.

- **Formula**: `Efficiency = (Speedup / Number_of_Cores) × 100%`
- **100% efficiency**: Perfect linear scaling (ideal case)
- **Practical range**: 50-95% on modern multi-core systems (rest lost to synchronization, cache coherency, etc.)
- **Lower efficiency with more cores**: Memory bandwidth becomes bottleneck
- **Example**: 8-core system with 7× speedup = 87.5% efficiency

### Frames Per Second (FPS)
Throughput metric showing how many images can be processed per second.

- **Formula**: `FPS = 1000 / Execution_Time_ms`
- **Higher FPS**: Better throughput for real-time or batch processing
- **Use for comparison**: Compare FPS across different modes with same image/filter combo
- **Example**: 10ms execution time = 100 FPS

### Memory Bandwidth Utilization
Shows how much of the system's memory bandwidth is being used.

- Critical for data-intensive filters (Median, Bilateral)
- GPU memory bandwidth much higher (100-900 GB/s) than CPU (50-100 GB/s)
- Affects scaling efficiency on large images

---

## 📈 Benchmark Output Format

The `benchmark_results.csv` file contains detailed results with the following columns:

| Column | Description | Example |
|--------|-------------|---------|
| `filter_name` | Algorithm name | "Gaussian Blur", "Sobel Edge" |
| `mode_name` | Execution mode | "Serial", "OpenMP_4", "CUDA", "MPI_4" |
| `image_width` | Image width in pixels | 512, 1024, 2048 |
| `image_height` | Image height in pixels | 512, 1024, 2048 |
| `parallel_time_ms` | Execution time in milliseconds | 15.5, 25.3, 104.2 |
| `serial_time_ms` | Serial baseline time | 100.0, 400.0, 1600.0 |
| `speedup` | Parallel speedup ratio | 6.45, 15.8, 15.4 |
| `efficiency` | Parallel efficiency percentage | 80.6%, 98.8%, 96.3% |
| `fps` | Frames per second throughput | 64.5, 39.5, 9.6 |
| `iterations` | Number of runs averaged | 3, 3, 3 |

**Sample Output:**
```
filter_name,mode_name,image_width,image_height,parallel_time_ms,serial_time_ms,speedup,efficiency,fps
Gaussian Blur,Serial,1024,1024,150.5,150.5,1.00,100.0%,6.64
Gaussian Blur,OpenMP_4,1024,1024,40.8,150.5,3.69,92.3%,24.5
Gaussian Blur,OpenMP_8,1024,1024,20.2,150.5,7.45,93.1%,49.5
Gaussian Blur,CUDA,1024,1024,3.5,150.5,43.0,N/A,285.7
```

---

---

## 🐛 Troubleshooting

### Build Issues

#### Error: CMake not found
```bash
# Windows
choco install cmake          # Using Chocolatey

# Linux
sudo apt-get install cmake

# macOS
brew install cmake
```

#### Error: OpenCV headers not found
```bash
# Ensure OpenCV is installed
pacman -S mingw-w64-x86_64-opencv  # MSYS2
sudo apt-get install libopencv-dev  # Ubuntu

# Set OpenCV_DIR if CMake can't find it
cmake -S . -B build -DOpenCV_DIR=/path/to/opencv/CMakeModules
```

#### Error: GLFW3 not found
```bash
pacman -S mingw-w64-x86_64-glfw  # MSYS2
sudo apt-get install libglfw3-dev  # Ubuntu
brew install glfw3  # macOS
```

#### Linker errors with static libraries
```bash
# Try release build with dynamic linking
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release -j8
```

#### Missing compiler (GCC/LLVM not found)
```bash
# Windows (MSYS2)
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-gcc-fortran

# Linux (Ubuntu/Debian)
sudo apt-get install gcc g++ build-essential

# Linux (Fedora/RHEL)
sudo dnf groupinstall "Development Tools"

# macOS
xcode-select --install
brew install gcc llvm
```

#### Missing OpenMP
```bash
# Windows (MSYS2)
pacman -S mingw-w64-x86_64-openmp

# Linux
sudo apt-get install libomp-dev

# macOS
brew install libomp
```

### Runtime Issues - GUI

#### GUI Won't Display / Black Window
- Update graphics drivers to latest version
- Try software rendering:
  ```bash
  set LIBGL_ALWAYS_INDIRECT=1
  ./build/ParallelVisionSuite.exe
  ```
- Check OpenGL version: `glxinfo | grep "OpenGL version"` (Linux)
- Try running with different monitor/resolution
- Check available GPU memory (run GPU benchmark separately)

#### GUI Crashes on Startup
- Ensure all dependencies are properly installed
- Check CMakeLists.txt CMake configuration
- Try Debug build for more error information:
  ```bash
  cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
  cmake --build build --config Debug
  ```

#### Window Resize / Layout Issues
- GUI layout is defined in `imgui_layout.ini` and `imgui.ini`
- Delete these files to reset to default layout:
  ```bash
  rm imgui_layout.ini imgui.ini
  ./build/ParallelVisionSuite.exe
  ```

### Runtime Issues - Image/Video Processing

#### Image File Not Found
- Use absolute paths (not relative)
- Verify file exists and is readable
- Check file permissions: `ls -la image.jpg`
- Supported image formats: **PNG, JPG, BMP, TIFF**
- Avoid spaces in file paths on Windows (or use quotes)

#### Video File Won't Load
- Check supported formats: **MP4, AVI, MOV**
- Ensure file is not corrupted: `ffprobe file.mp4`
- Verify codec compatibility
- Try converting with FFmpeg: `ffmpeg -i input.avi -c:v libx264 output.mp4`

#### Image Processing Produces Black Output
- Check if filter parameters are out of range
- Verify input image is loaded correctly (check Image Preview panel)
- Try simpler filter first (e.g., Grayscale) to isolate issue
- Check if image colorspace is RGB/RGBA (not HSV, Lab, etc.)

### Runtime Issues - Parallel Execution

#### Performance Worse with OpenMP on Small Images
- This is **expected behavior**: parallelization overhead > benefits for small images
- Solution: Use larger images (1024×1024 or larger for noticeable speedup)
- For testing with small images, use Serial mode
- Tip: `./build/ParallelVisionSuite.exe --omp 2` uses fewer threads (less overhead)

#### Low OpenMP Speedup
- Check actual number of CPU cores: `nproc` or `Get-ComputerInfo | grep Processors`
- Verify thread count matches physical cores (not hyperthreads)
- Close background applications consuming CPU
- Try with larger images
- Run multiple times and take average (first run may have overhead)

#### CUDA not Found / GPU Disabled
- Verify NVIDIA GPU present:
  ```bash
  nvidia-smi
  ```
- Check CUDA installation:
  ```bash
  nvcc --version
  ```
- If `nvcc` not found, install CUDA Toolkit: https://developer.nvidia.com/cuda-downloads
- Verify GPU Compute Capability ≥ 3.5:
  ```bash
  nvidia-smi -q | grep "Compute Capability"
  ```
- Check NVIDIA drivers are up-to-date: `nvidia-smi`
- If GPU detected but not used, check CMakeLists.txt for CUDA configuration

#### MPI Initialization Error
**Windows:**
- Install MS-MPI: https://www.microsoft.com/en-us/download/details.aspx?id=105289
- Restart system after installation
- Verify installation: `mpiexec --version`

**Linux/macOS:**
```bash
# Ubuntu/Debian
sudo apt-get install openmpi-bin libopenmpi-dev

# Fedora/RHEL
sudo dnf install openmpi openmpi-devel

# macOS
brew install open-mpi
```

**MPI Rank Communication Issues:**
- Check network connectivity between nodes
- Use verbose mode: `mpiexec -n 4 -v ./build/ParallelVisionSuite.exe --mpi`
- Try running on single machine first: `mpiexec -n 4 ./build/ParallelVisionSuite.exe --mpi`
- Check firewall doesn't block MPI ports
- Verify hostfile syntax if using multi-node setup

### Runtime Issues - Benchmarking

#### Benchmark Runs Very Slowly
- This is expected for large image sizes and many modes
- Full benchmark can take 5-30 minutes depending on system
- Run specific mode instead: `./build/ParallelVisionSuite.exe --omp 4`
- Use smaller image sizes for faster testing

#### CSV File Empty or Malformed
- Check `benchmark_results.csv` is writeable to current directory
- Ensure no antivirus software blocking file writes
- Check terminal output for error messages
- Try running from different directory:
  ```bash
  cd /tmp
  /path/to/build/ParallelVisionSuite.exe --serial
  ```

#### High Variance in Benchmark Results
- Close background applications
- Disable CPU frequency scaling if possible
- Run multiple iterations (averages reduce variance)
- Avoid running other benchmarks simultaneously
- Use identical system state for repeated benchmarks

---

## 🎯 Performance Optimization Tips

### General Best Practices
1. **Use larger images**: Parallelization overhead noticeable on small images (<512x512)
   - Ideal: 1024×1024 or larger for meaningful speedup measurements
2. **Close background applications**: Reduces CPU contention
3. **Run multiple iterations**: Take average of 3+ runs to reduce variance
4. **Pin threads to cores**: Improves cache locality
   ```bash
   # Linux example
   taskset -c 0-7 ./build/ParallelVisionSuite.exe --omp 8
   ```
5. **Disable frequency scaling** during benchmarks (requires admin):
   ```bash
   # Linux
   echo performance | sudo tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor
   ```

### OpenMP Optimization
- Thread count = number of physical CPU cores (not hyperthreading)
- Use `export OMP_PROC_BIND=close` for better cache affinity
- For NUMA systems: `export OMP_PROC_BIND=spread,close`
- Monitor with `top` or `htop` to verify thread utilization

### CUDA Optimization
- GPU best for **large images** (2048×2048+)
- Check GPU memory: `nvidia-smi | grep "Memory-Usage"`
- Monitor GPU utilization: `nvidia-smi --query-gpu=utilization.gpu --loop=500`
- Increase batch size if GPU utilization low
- For multiple GPUs, use: `CUDA_VISIBLE_DEVICES=0,1,2 ./app`

### MPI Optimization
- Use balanced rank distribution: 2, 4, 8, or 16 ranks
- Minimize network latency (local cluster preferred over WAN)
- Use **larger images** to amortize communication overhead (~1-2ms per message)
- Monitor inter-node communication: `mpiexec -n 4 -v ./app --mpi`
- For multi-node: Set up passwordless SSH for efficient launching

### Hybrid Mode Optimization (MPI + OpenMP/CUDA)
- **Recommended layout**: 1 MPI rank per node, OpenMP threads = cores per node
  ```bash
  # Example: 4 nodes, 8 cores each
  mpiexec -n 4 ./app --mpi-omp 8
  ```
- Avoid oversubscription: Total threads ≤ physical CPU cores
- Monitor both MPI communication and thread behavior

---

## 📚 Additional Resources

### Official Documentation
- **Architecture & Design**: See [docs/architecture.md](docs/architecture.md)
- **User Guide**: See [docs/user_guide.md](docs/user_guide.md)

### External References
- **CMake Documentation**: https://cmake.org/documentation/
- **OpenMP Specification**: https://www.openmp.org/
- **CUDA Programming Guide**: https://docs.nvidia.com/cuda/cuda-c-programming-guide/
- **MPI Tutorial**: https://computing.llnl.gov/tutorials/mpi/
- **ImGui Documentation**: https://github.com/ocornut/imgui
- **opencv.org**: https://opencv.org/

---

## ⚡ Feature Matrix

Quick reference for which filters are available in each execution mode:

| Filter | Serial | OpenMP | CUDA | MPI | MPI_OMP | MPI_CUDA |
|--------|--------|--------|------|-----|---------|----------|
| Gaussian Blur | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ |
| Sobel Edge | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ |
| Median Filter | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ |
| Laplacian Edge | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ |
| Sharpen | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ |
| Box Blur | ✓ | ✓ | - | ✓ | ✓ | - |
| Emboss | ✓ | ✓ | - | ✓ | ✓ | - |
| Sepia Tone | ✓ | ✓ | - | ✓ | ✓ | - |
| Grayscale | ✓ | ✓ | - | ✓ | ✓ | - |
| Brightness/Contrast | ✓ | ✓ | - | ✓ | ✓ | - |
| Bilateral Filter | ✓ | ✓ | - | ✓ | ✓ | - |
| Motion Blur | ✓ | ✓ | - | ✓ | ✓ | - |
| Prewitt Edge | ✓ | ✓ | - | ✓ | ✓ | - |

**Legend**: ✓ = Supported, - = Falls back to Serial/OpenMP

---

## ✅ Quick Verification Checklist

After successful build, verify all major features:

### Build & Startup
- [ ] Build succeeds: `cmake --build build --config Release -j8`
- [ ] No linker errors or missing dependencies
- [ ] Executable created: `build/ParallelVisionSuite.exe` (Windows) or `build/ParallelVisionSuite` (Linux)
- [ ] GUI launches without crashes: `./build/ParallelVisionSuite.exe`

### Basic Functionality
- [ ] GUI windows and panels render correctly
- [ ] Can load image files (PNG, JPG, BMP, TIFF)
- [ ] Image displays in Image Preview panel
- [ ] All 13 filters available in Filter Panel dropdown

### Execution Modes (CLI)
- [ ] Serial mode works: `./build/ParallelVisionSuite.exe --serial`
- [ ] OpenMP mode works: `./build/ParallelVisionSuite.exe --omp 4`
- [ ] Benchmark mode works: `./build/ParallelVisionSuite.exe --benchmark`

### Output & Results
- [ ] CSV files generated: `benchmark_results.csv` exists and has data
- [ ] CSV format correct: columns match expected format
- [ ] Speedup and efficiency values are reasonable (>0)
- [ ] FPS values calculated correctly

### Performance Metrics
- [ ] Execution time displayed in milliseconds (ms)
- [ ] Speedup values shown for parallel modes (should be >1)
- [ ] Efficiency percentages calculated (expect 50-95%)
- [ ] Real-time metrics update during processing

### Advanced Features (if available)
- [ ] CUDA mode works: `./build/ParallelVisionSuite.exe --cuda` (if GPU present)
- [ ] Video file loads: Load MP4, AVI, or MOV file
- [ ] Video playback works in Video Preview panel
- [ ] Performance visualization (graphs, charts) displays

### Optional: MPI Features
- [ ] MPI installed: `mpiexec --version` returns version
- [ ] MPI mode initializes: `mpiexec -n 2 ./build/ParallelVisionSuite.exe --mpi --help`

**All tests passed?** ParallelVisionSuite is ready for benchmarking and analysis!

---

## 📝 Version Information

- **Project**: ParallelVisionSuite v1.0.0
- **Standard**: C++17
- **Total Filters**: 13 (Serial, OpenMP, CUDA variants)
- **Execution Modes**: 6 (Serial, OpenMP, CUDA, MPI, MPI_OMP, MPI_CUDA)
- **GUI Framework**: ImGui + ImPlot
- **License**: Academic/Educational

---

## 🚀 Quick Start

**For GUI Mode:**
```bash
./build/ParallelVisionSuite.exe
# Or on Windows:
./run_app.bat
```

**For Quick Benchmarking:**
```bash
# Serial baseline
./build/ParallelVisionSuite.exe --serial

# Multi-threaded comparison
./build/ParallelVisionSuite.exe --omp 4
./build/ParallelVisionSuite.exe --omp 8

# Full comprehensive benchmark (5-30 min)
./build/ParallelVisionSuite.exe --benchmark
```

**Analyze Results:**
1. Open generated `benchmark_results.csv` in Excel/LibreOffice
2. Create pivot tables or graphs for speedup/efficiency analysis
3. Compare modes and filters
4. Identify optimal configuration for your workload

---

**Ready to start?** Follow the build instructions above or launch with `./run_app.bat` (Windows).

For detailed documentation, see [docs/architecture.md](docs/architecture.md) and [docs/user_guide.md](docs/user_guide.md).

