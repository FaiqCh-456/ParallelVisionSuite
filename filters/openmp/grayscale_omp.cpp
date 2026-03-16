#include "grayscale_omp.h"
#include <omp.h>
#include <algorithm>
ImageBuffer OmpGrayscaleFilter::apply(const ImageBuffer& in, int numThreads) {
    ImageBuffer out = in.clone();
    const int W=in.width,H=in.height,C=in.channels;
    omp_set_num_threads(numThreads);
    #pragma omp parallel for schedule(static) collapse(2)
    for (int r=0;r<H;++r) for (int c=0;c<W;++c) {
        float grey = (C>=3)
            ? 0.299f*in.at(r,c,0)+0.587f*in.at(r,c,1)+0.114f*in.at(r,c,2)
            : (float)in.at(r,c,0);
        uint8_t g=(uint8_t)std::clamp((int)grey,0,255);
        for (int ch=0;ch<std::min(C,3);++ch) out.at(r,c,ch)=g;
        if (C==4) out.at(r,c,3)=in.at(r,c,3);
    }
    return out;
}
