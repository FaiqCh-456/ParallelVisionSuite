#ifdef HAS_CUDA
#include "median_kernel.cuh"
#include "cuda_utils.cuh"
#include "gpu_manager.h"
#include <cuda_runtime.h>

// Insertion sort on small window — suitable for GPU
__device__ void insertionSort(uint8_t* arr, int n) {
    for (int i = 1; i < n; ++i) {
        uint8_t key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) { arr[j+1] = arr[j]; --j; }
        arr[j+1] = key;
    }
}

__global__ void medianKernel(const uint8_t* __restrict__ in,
                              uint8_t* __restrict__ out,
                              int W, int H, int C, int R)
{
    int col = blockIdx.x * blockDim.x + threadIdx.x;
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    if (col >= W || row >= H) return;

    int winSize = (2*R+1)*(2*R+1);
    uint8_t window[25]; // max R=2 → 25 elements

    for (int ch = 0; ch < C; ++ch) {
        int cnt = 0;
        for (int kr = -R; kr <= R; ++kr) {
            for (int kc = -R; kc <= R; ++kc) {
                int sr = clampIdx(row+kr, H);
                int sc = clampIdx(col+kc, W);
                window[cnt++] = in[(sr*W+sc)*C+ch];
            }
        }
        insertionSort(window, winSize);
        out[(row*W+col)*C+ch] = window[winSize/2];
    }
}

ImageBuffer CudaMedianFilter::apply(const ImageBuffer& input, int) {
    auto& gpu = GPUManager::instance();
    ImageBuffer out = input.clone();
    size_t bytes = input.byteSize();

    uint8_t* d_in  = gpu.deviceAlloc(bytes);
    uint8_t* d_out = gpu.deviceAlloc(bytes);
    gpu.uploadToDevice(input.data.data(), d_in, bytes);

    dim3 block(16, 16);
    dim3 grid(divUp(input.width, 16), divUp(input.height, 16));
    medianKernel<<<grid, block>>>(d_in, d_out,
                                   input.width, input.height,
                                   input.channels, 2);
    gpu.sync();
    gpu.downloadToHost(d_out, out.data.data(), bytes);
    gpu.deviceFree(d_in);
    gpu.deviceFree(d_out);
    return out;
}
#endif