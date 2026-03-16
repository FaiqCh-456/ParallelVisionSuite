#include "brightness_omp.h"
#include <omp.h>
#include <algorithm>
ImageBuffer OmpBrightnessFilter::apply(const ImageBuffer& in, int numThreads) {
    ImageBuffer out = in.clone();
    const int W=in.width,H=in.height,C=in.channels;
    float b=brightness, ct=contrast;
    omp_set_num_threads(numThreads);
    #pragma omp parallel for schedule(static) collapse(2)
    for (int r=0;r<H;++r) for (int c=0;c<W;++c) for (int ch=0;ch<C;++ch) {
        if (C==4&&ch==3){out.at(r,c,ch)=in.at(r,c,ch);continue;}
        float v=(float)in.at(r,c,ch);
        v=(v-128.0f)*ct+128.0f+b;
        out.at(r,c,ch)=(uint8_t)std::clamp((int)v,0,255);
    }
    return out;
}
