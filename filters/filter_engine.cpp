#include "filter_engine.h"
#include "serial/gaussian_serial.h"
#include "serial/sobel_serial.h"
#include "serial/median_serial.h"
#include "serial/laplacian_serial.h"
#include "serial/sharpen_serial.h"
#include "serial/box_blur_serial.h"
#include "serial/emboss_serial.h"
#include "serial/sepia_serial.h"
#include "serial/grayscale_serial.h"
#include "serial/brightness_serial.h"
#include "serial/bilateral_serial.h"
#include "serial/motion_blur_serial.h"
#include "serial/prewitt_serial.h"
#include "openmp/gaussian_omp.h"
#include "openmp/sobel_omp.h"
#include "openmp/median_omp.h"
#include "openmp/laplacian_omp.h"
#include "openmp/sharpen_omp.h"
#include "openmp/box_blur_omp.h"
#include "openmp/emboss_omp.h"
#include "openmp/sepia_omp.h"
#include "openmp/grayscale_omp.h"
#include "openmp/brightness_omp.h"
#include "openmp/bilateral_omp.h"
#include "openmp/motion_blur_omp.h"
#include "openmp/prewitt_omp.h"
#include "timer.h"
#include <stdexcept>

IFilter* FilterEngine::create(FilterType type, ExecutionMode mode) {
    bool useOmp = (mode == ExecutionMode::OPENMP || mode == ExecutionMode::MPI_OMP);

    if (useOmp) {
        switch(type) {
            case FilterType::GAUSSIAN:            return new OmpGaussianFilter();
            case FilterType::SOBEL:               return new OmpSobelFilter();
            case FilterType::MEDIAN:              return new OmpMedianFilter();
            case FilterType::LAPLACIAN:           return new OmpLaplacianFilter();
            case FilterType::SHARPEN:             return new OmpSharpenFilter();
            case FilterType::BOX_BLUR:            return new OmpBoxBlurFilter();
            case FilterType::EMBOSS:              return new OmpEmbossFilter();
            case FilterType::SEPIA:               return new OmpSepiaFilter();
            case FilterType::GRAYSCALE:           return new OmpGrayscaleFilter();
            case FilterType::BRIGHTNESS_CONTRAST: return new OmpBrightnessFilter();
            case FilterType::BILATERAL:           return new OmpBilateralFilter();
            case FilterType::MOTION_BLUR:         return new OmpMotionBlurFilter();
            case FilterType::PREWITT:             return new OmpPrewittFilter();
        }
    }

    // Default: Serial (also used for CUDA fallback when no GPU)
    switch(type) {
        case FilterType::GAUSSIAN:            return new SerialGaussianFilter();
        case FilterType::SOBEL:               return new SerialSobelFilter();
        case FilterType::MEDIAN:              return new SerialMedianFilter();
        case FilterType::LAPLACIAN:           return new SerialLaplacianFilter();
        case FilterType::SHARPEN:             return new SerialSharpenFilter();
        case FilterType::BOX_BLUR:            return new SerialBoxBlurFilter();
        case FilterType::EMBOSS:              return new SerialEmbossFilter();
        case FilterType::SEPIA:               return new SerialSepiaFilter();
        case FilterType::GRAYSCALE:           return new SerialGrayscaleFilter();
        case FilterType::BRIGHTNESS_CONTRAST: return new SerialBrightnessFilter();
        case FilterType::BILATERAL:           return new SerialBilateralFilter();
        case FilterType::MOTION_BLUR:         return new SerialMotionBlurFilter();
        case FilterType::PREWITT:             return new SerialPrewittFilter();
    }
    throw std::runtime_error("FilterEngine: unknown filter type");
}

ImageBuffer FilterEngine::applyTimed(const ImageBuffer& input,
                                      FilterType type,
                                      ExecutionMode mode,
                                      int numThreads,
                                      double& outTimeMs)
{
    IFilter* filter = create(type, mode);
    Timer t;
    t.start();
    ImageBuffer result = filter->apply(input, numThreads);
    t.stop();
    outTimeMs = t.elapsedMs();
    delete filter;
    return result;
}
