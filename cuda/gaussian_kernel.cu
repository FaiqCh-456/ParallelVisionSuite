#ifdef HAS_CUDA
#include "gaussian_kernel.cuh"
#include "cuda_utils.cuh"
#include "gpu_manager.h"
#include <cuda_runtime.h>

// 5x5 Gaussian kernel stored in constant memory (fast broadcast)
__constant__ float d_gaussKernel[25];

static const float h_gaussKernel[25] = {
    1,  4,  7,  4, 1,
    4, 16, 26, 16, 4,
    7, 26, 41, 26, 7,
    4, 16, 26, 16, 4,
    1,  4,  7,  4, 1
};
static const float GAUSS_SUM = 273.0f;

__global__ void gaussianKernel(const uint8_t* __restrict__ in,
                                uint8_t* __restrict__ out,
                                int W, int H, int C)
{
    int col = blockIdx.x * blockDim.x + threadIdx.x;
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    if (col >= W || row >= H) return;

    for (int ch = 0; ch < C; ++ch) {
        float acc = 0.0f;
        for (int kr = -2; kr <= 2; ++kr) {
            for (int kc = -2; kc <= 2; ++kc) {
                int sr = clampIdx(row + kr, H);
                int sc = clampIdx(col + kc, W);
                float w = d_gaussKernel[(kr+2)*5 + (kc+2)];
                acc += w * in[(sr * W + sc) * C + ch];
            }
        }
        out[(row * W + col) * C + ch] = clamp255(acc / GAUSS_SUM);
    }
}

ImageBuffer CudaGaussianFilter::apply(const ImageBuffer& input, int) {
    auto& gpu = GPUManager::instance();
    ImageBuffer out = input.clone();
    size_t bytes = input.byteSize();

    // Upload constant kernel
    cudaMemcpyToSymbol(d_gaussKernel, h_gaussKernel, 25 * sizeof(float));

    uint8_t* d_in  = gpu.deviceAlloc(bytes);
    uint8_t* d_out = gpu.deviceAlloc(bytes);
    gpu.uploadToDevice(input.data.data(), d_in, bytes);

    dim3 block(16, 16);
    dim3 grid(divUp(input.width, 16), divUp(input.height, 16));
    gaussianKernel<<<grid, block>>>(d_in, d_out,
                                    input.width, input.height,
                                    input.channels);
    gpu.sync();
    gpu.downloadToHost(d_out, out.data.data(), bytes);

    gpu.deviceFree(d_in);
    gpu.deviceFree(d_out);
    return out;
}
#endif // HAS_CUDA