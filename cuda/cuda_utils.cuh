#pragma once
#ifdef HAS_CUDA
#include <cuda_runtime.h>
#include <cstdint>

// Error-checking macro for use inside .cu files
#define CUDA_CHECK(call)                                                  \
    do {                                                                  \
        cudaError_t _e = (call);                                          \
        if (_e != cudaSuccess) {                                          \
            printf("CUDA error %s at %s:%d\n",                           \
                   cudaGetErrorString(_e), __FILE__, __LINE__);           \
            return;                                                       \
        }                                                                 \
    } while(0)

// Clamp a float to [0, 255] and cast to uint8_t
__device__ inline uint8_t clamp255(float v) {
    return (uint8_t)fminf(fmaxf(v, 0.0f), 255.0f);
}

// Clamp row/col to image boundary (mirror-pad free clamping)
__device__ inline int clampIdx(int x, int maxVal) {
    return x < 0 ? 0 : (x >= maxVal ? maxVal - 1 : x);
}

// 2-D grid / block launch helper
// Usage: dim3 block(BX,BY); dim3 grid(divUp(W,BX), divUp(H,BY));
inline int divUp(int n, int b) { return (n + b - 1) / b; }

#endif // HAS_CUDA