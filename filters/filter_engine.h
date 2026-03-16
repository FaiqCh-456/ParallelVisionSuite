#pragma once
#include "image_buffer.h"
#include <string>

enum class FilterType {
    // Original 5
    GAUSSIAN,
    SOBEL,
    MEDIAN,
    LAPLACIAN,
    SHARPEN,
    // New 8
    BOX_BLUR,
    EMBOSS,
    SEPIA,
    GRAYSCALE,
    BRIGHTNESS_CONTRAST,
    BILATERAL,
    MOTION_BLUR,
    PREWITT
};

enum class ExecutionMode {
    SERIAL,
    OPENMP,
    CUDA,
    MPI,
    MPI_OMP,
    MPI_CUDA
};

inline std::string filterName(FilterType f) {
    switch(f) {
        case FilterType::GAUSSIAN:            return "Gaussian Blur";
        case FilterType::SOBEL:               return "Sobel Edge";
        case FilterType::MEDIAN:              return "Median Filter";
        case FilterType::LAPLACIAN:           return "Laplacian Edge";
        case FilterType::SHARPEN:             return "Sharpen";
        case FilterType::BOX_BLUR:            return "Box Blur";
        case FilterType::EMBOSS:              return "Emboss";
        case FilterType::SEPIA:               return "Sepia Tone";
        case FilterType::GRAYSCALE:           return "Grayscale";
        case FilterType::BRIGHTNESS_CONTRAST: return "Brightness/Contrast";
        case FilterType::BILATERAL:           return "Bilateral Filter";
        case FilterType::MOTION_BLUR:         return "Motion Blur";
        case FilterType::PREWITT:             return "Prewitt Edge";
    }
    return "Unknown";
}

inline std::string modeName(ExecutionMode m) {
    switch(m) {
        case ExecutionMode::SERIAL:   return "Serial";
        case ExecutionMode::OPENMP:   return "OpenMP";
        case ExecutionMode::CUDA:     return "CUDA";
        case ExecutionMode::MPI:      return "MPI";
        case ExecutionMode::MPI_OMP:  return "MPI+OpenMP";
        case ExecutionMode::MPI_CUDA: return "MPI+CUDA";
    }
    return "Unknown";
}

class IFilter {
public:
    virtual ~IFilter() = default;
    virtual ImageBuffer apply(const ImageBuffer& input, int numThreads = 1) = 0;
    virtual std::string name() const = 0;
};

class FilterEngine {
public:
    static IFilter* create(FilterType type, ExecutionMode mode);
    static ImageBuffer applyTimed(const ImageBuffer& input,
                                  FilterType type,
                                  ExecutionMode mode,
                                  int numThreads,
                                  double& outTimeMs);
};
