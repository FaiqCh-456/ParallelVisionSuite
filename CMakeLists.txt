cmake_minimum_required(VERSION 3.20)
project(ParallelVisionSuite VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── MinGW Static Linking Fix ─────────────────────────────────────────
if(MINGW)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")
    add_link_options("-Wl,--allow-multiple-definition")
endif()

# ── OpenMP, MPI, OpenCV ──────────────────────────────────────────────
find_package(OpenMP REQUIRED)
set(MSMPI_INCLUDE "C:/Program Files (x86)/Microsoft SDKs/MPI/Include")
set(MSMPI_LIB     "C:/Program Files (x86)/Microsoft SDKs/MPI/Lib/x64/msmpi.lib")
set(OpenCV_DIR "C:/msys64/mingw64/lib/cmake/opencv4")
find_package(OpenCV REQUIRED)

set(HAS_CUDA FALSE)

# ── OpenGL + GLFW ────────────────────────────────────────────────────
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED HINTS "C:/msys64/mingw64/lib/cmake/glfw3")

# ── Sources ──────────────────────────────────────────────────────────
set(CORE_SOURCES
    src/main.cpp
    src/orchestrator.cpp
    src/session_manager.cpp
    src/mode_dispatcher.cpp

    utils/image_loader.cpp
    utils/video_loader.cpp
    utils/csv_writer.cpp
    utils/asset_generator.cpp

    filters/filter_engine.cpp

    # ── Serial filters ────────────────────────────────────────────────
    filters/serial/gaussian_serial.cpp
    filters/serial/sobel_serial.cpp
    filters/serial/median_serial.cpp
    filters/serial/laplacian_serial.cpp
    filters/serial/sharpen_serial.cpp
    filters/serial/box_blur_serial.cpp
    filters/serial/emboss_serial.cpp
    filters/serial/sepia_serial.cpp
    filters/serial/grayscale_serial.cpp
    filters/serial/brightness_serial.cpp
    filters/serial/bilateral_serial.cpp
    filters/serial/motion_blur_serial.cpp
    filters/serial/prewitt_serial.cpp

    # ── OpenMP filters ────────────────────────────────────────────────
    filters/openmp/gaussian_omp.cpp
    filters/openmp/sobel_omp.cpp
    filters/openmp/median_omp.cpp
    filters/openmp/laplacian_omp.cpp
    filters/openmp/sharpen_omp.cpp
    filters/openmp/box_blur_omp.cpp
    filters/openmp/emboss_omp.cpp
    filters/openmp/sepia_omp.cpp
    filters/openmp/grayscale_omp.cpp
    filters/openmp/brightness_omp.cpp
    filters/openmp/bilateral_omp.cpp
    filters/openmp/motion_blur_omp.cpp
    filters/openmp/prewitt_omp.cpp

    cuda/gpu_manager.cpp
    mpi/mpi_manager.cpp
    mpi/mpi_worker.cpp

    benchmark/benchmark_runner.cpp
    benchmark/performance_logger.cpp

    gui/gui.cpp
    gui/gl_texture.cpp
    gui/panels/image_preview_panel.cpp
    gui/panels/video_preview_panel.cpp
    gui/panels/filter_panel.cpp
    gui/panels/mode_panel.cpp
    gui/panels/benchmark_panel.cpp
    gui/panels/log_panel.cpp
    gui/panels/thread_viz_panel.cpp
    gui/panels/arch_anim_panel.cpp

    third_party/imgui/imgui.cpp
    third_party/imgui/imgui_draw.cpp
    third_party/imgui/imgui_tables.cpp
    third_party/imgui/imgui_widgets.cpp
    third_party/imgui/backends/imgui_impl_glfw.cpp
    third_party/imgui/backends/imgui_impl_opengl3.cpp

    third_party/implot/implot.cpp
    third_party/implot/implot_items.cpp

    third_party/glad/src/glad.c
)

add_executable(ParallelVisionSuite ${CORE_SOURCES})

# Compile glad.c as C++ (fixes mixed-runtime issues on MinGW)
set_source_files_properties(third_party/glad/src/glad.c PROPERTIES LANGUAGE CXX)

target_compile_definitions(ParallelVisionSuite PRIVATE
    _UCRT
    __STDC_WANT_LIB_EXT1__=1
    __USE_MINGW_ANSI_STDIO=1
    NOMINMAX
    WIN32_LEAN_AND_MEAN
    _CRT_SECURE_NO_WARNINGS
)

target_include_directories(ParallelVisionSuite PRIVATE
    src
    filters
    filters/serial
    filters/openmp
    cuda
    mpi
    gui
    benchmark
    utils
    third_party
    third_party/imgui
    third_party/imgui/backends
    third_party/implot
    third_party/glad/include
    third_party/stb
    "${MSMPI_INCLUDE}"
    "${MSMPI_INCLUDE}/x64"
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(ParallelVisionSuite PRIVATE
    OpenGL::GL
    glfw
    OpenMP::OpenMP_CXX
    "${MSMPI_LIB}"
    ${OpenCV_LIBS}
    -lpthread
    -lws2_32
    -lgdi32
    -lcomdlg32
)

target_compile_options(ParallelVisionSuite PRIVATE -Wall -O2)