#include "sobel_omp.h"
#include <omp.h>
#include <cmath>
#include <algorithm>

ImageBuffer OmpSobelFilter::apply(const ImageBuffer& in, int numThreads) {
    const int W = in.width, H = in.height, C = in.channels;
    ImageBuffer out = in.clone();
    static const int Gx[3][3] = {{-1,0,1},{-2,0,2},{-1,0,1}};
    static const int Gy[3][3] = {{-1,-2,-1},{0,0,0},{1,2,1}};

    omp_set_num_threads(numThreads);
    #pragma omp parallel for schedule(static) collapse(2)
    for (int r = 1; r < H - 1; ++r) {
        for (int c = 1; c < W - 1; ++c) {
            for (int ch = 0; ch < C; ++ch) {
                float gx = 0, gy = 0;
                for (int kr = -1; kr <= 1; ++kr)
                    for (int kc = -1; kc <= 1; ++kc) {
                        float px = in.at(r+kr, c+kc, ch);
                        gx += Gx[kr+1][kc+1] * px;
                        gy += Gy[kr+1][kc+1] * px;
                    }
                float mag = std::sqrt(gx*gx + gy*gy);
                out.at(r, c, ch) = static_cast<uint8_t>(
                    std::clamp(mag, 0.0f, 255.0f));
            }
        }
    }
    return out;
}