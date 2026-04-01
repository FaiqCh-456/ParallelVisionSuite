#ifdef HAS_CUDA
#include "sobel_kernel.cuh"
#include "cuda_utils.cuh"
#include "gpu_manager.h"
#include <cuda_runtime.h>
#include <math.h>

__global__ void sobelKernel(const uint8_t* __restrict__ in,
                             uint8_t* __restrict__ out,
                             int W, int H, int C)
{
    int col = blockIdx.x * blockDim.x + threadIdx.x;
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    if (col < 1 || col >= W-1 || row < 1 || row >= H-1) {
        if (col < W && row < H)
            for (int ch = 0; ch < C; ++ch)
                out[(row*W+col)*C+ch] = in[(row*W+col)*C+ch];
        return;
    }

    static const int Gx[3][3] = {{-1,0,1},{-2,0,2},{-1,0,1}};
    static const int Gy[3][3] = {{-1,-2,-1},{0,0,0},{1,2,1}};

    for (int ch = 0; ch < C; ++ch) {
        float gx = 0, gy = 0;
        for (int kr = -1; kr <= 1; ++kr)
            for (int kc = -1; kc <= 1; ++kc) {
                float px = in[((row+kr)*W+(col+kc))*C+ch];
                gx += Gx[kr+1][kc+1] * px;
                gy += Gy[kr+1][kc+1] * px;
            }
        out[(row*W+col)*C+ch] = clamp255(sqrtf(gx*gx + gy*gy));
    }
}

ImageBuffer CudaSobelFilter::apply(const ImageBuffer& input, int) {
    auto& gpu = GPUManager::instance();
    ImageBuffer out = input.clone();
    size_t bytes = input.byteSize();

    uint8_t* d_in  = gpu.deviceAlloc(bytes);
    uint8_t* d_out = gpu.deviceAlloc(bytes);
    gpu.uploadToDevice(input.data.data(), d_in, bytes);

    dim3 block(16, 16);
    dim3 grid(divUp(input.width, 16), divUp(input.height, 16));
    sobelKernel<<<grid, block>>>(d_in, d_out,
                                  input.width, input.height,
                                  input.channels);
    gpu.sync();
    gpu.downloadToHost(d_out, out.data.data(), bytes);
    gpu.deviceFree(d_in);
    gpu.deviceFree(d_out);
    return out;
}
#endif