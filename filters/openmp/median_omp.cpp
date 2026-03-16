#include "median_omp.h"
#include <omp.h>
#include <algorithm>
#include <vector>

ImageBuffer OmpMedianFilter::apply(const ImageBuffer& in, int numThreads) {
    const int W = in.width, H = in.height, C = in.channels;
    ImageBuffer out = in.clone();
    const int R = RADIUS;

    omp_set_num_threads(numThreads);
    #pragma omp parallel
    {
        std::vector<uint8_t> window((2*R+1)*(2*R+1));
        #pragma omp for schedule(dynamic, 2) collapse(2)
        for (int r = 0; r < H; ++r) {
            for (int c = 0; c < W; ++c) {
                for (int ch = 0; ch < C; ++ch) {
                    int cnt = 0;
                    for (int kr = -R; kr <= R; ++kr)
                        for (int kc = -R; kc <= R; ++kc) {
                            int sr = std::clamp(r+kr, 0, H-1);
                            int sc = std::clamp(c+kc, 0, W-1);
                            window[cnt++] = in.at(sr, sc, ch);
                        }
                    std::nth_element(window.begin(),
                                     window.begin() + cnt/2,
                                     window.begin() + cnt);
                    out.at(r, c, ch) = window[cnt/2];
                }
            }
        }
    }
    return out;
}