#include "gaussian_serial.h"
#include <algorithm>
#include <cstdint>

// 5x5 Gaussian kernel  (unnormalised; KERNEL_SUM normalises it)
const float SerialGaussianFilter::KERNEL[5][5] = {
    {1,  4,  7,  4, 1},
    {4, 16, 26, 16, 4},
    {7, 26, 41, 26, 7},
    {4, 16, 26, 16, 4},
    {1,  4,  7,  4, 1}
};
const float SerialGaussianFilter::KERNEL_SUM = 273.0f;

ImageBuffer SerialGaussianFilter::apply(const ImageBuffer& in, int) {
    ImageBuffer out = in.clone();
    const int W = in.width, H = in.height, C = in.channels;
    const int radius = 2;  // half of 5x5

    for (int r = 0; r < H; ++r) {
        for (int c = 0; c < W; ++c) {
            for (int ch = 0; ch < C; ++ch) {
                float acc = 0.0f;
                for (int kr = -radius; kr <= radius; ++kr) {
                    for (int kc = -radius; kc <= radius; ++kc) {
                        int sr = std::clamp(r + kr, 0, H - 1);
                        int sc = std::clamp(c + kc, 0, W - 1);
                        float w = KERNEL[kr + radius][kc + radius];
                        acc += w * in.at(sr, sc, ch);
                    }
                }
                out.at(r, c, ch) = static_cast<uint8_t>(
                    std::clamp(acc / KERNEL_SUM, 0.0f, 255.0f));
            }
        }
    }
    return out;
}