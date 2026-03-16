#include "motion_blur_omp.h"
#include <omp.h>
#include <algorithm>
ImageBuffer OmpMotionBlurFilter::apply(const ImageBuffer& in, int numThreads) {
    ImageBuffer out = in.clone();
    const int W=in.width,H=in.height,C=in.channels,L=LENGTH;
    omp_set_num_threads(numThreads);
    #pragma omp parallel for schedule(static) collapse(2)
    for (int r=0;r<H;++r) for (int c=0;c<W;++c) for (int ch=0;ch<C;++ch) {
        int acc=0,cnt=0;
        for (int k=0;k<L;++k) {
            int sc=std::clamp(c-k,0,W-1);
            acc+=in.at(r,sc,ch); ++cnt;
        }
        out.at(r,c,ch)=(uint8_t)(acc/cnt);
    }
    return out;
}
