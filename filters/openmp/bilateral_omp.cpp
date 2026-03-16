#include "bilateral_omp.h"
#include <omp.h>
#include <cmath>
#include <algorithm>
ImageBuffer OmpBilateralFilter::apply(const ImageBuffer& in, int numThreads) {
    ImageBuffer out = in.clone();
    const int W=in.width,H=in.height,C=in.channels,R=RADIUS;
    const float ss2=2.0f*SIGMA_SPACE*SIGMA_SPACE;
    const float sc2=2.0f*SIGMA_COLOR*SIGMA_COLOR;
    omp_set_num_threads(numThreads);
    #pragma omp parallel for schedule(dynamic,4) collapse(2)
    for (int r=0;r<H;++r) for (int c=0;c<W;++c) for (int ch=0;ch<C;++ch) {
        float wsum=0,acc=0,center=(float)in.at(r,c,ch);
        for (int kr=-R;kr<=R;++kr) for (int kc=-R;kc<=R;++kc) {
            int sr=std::clamp(r+kr,0,H-1),sc_=std::clamp(c+kc,0,W-1);
            float pix=(float)in.at(sr,sc_,ch);
            float w=expf(-(kr*kr+kc*kc)/ss2)*expf(-((pix-center)*(pix-center))/sc2);
            acc+=w*pix; wsum+=w;
        }
        out.at(r,c,ch)=(uint8_t)std::clamp((int)(acc/wsum),0,255);
    }
    return out;
}
