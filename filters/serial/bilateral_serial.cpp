#include "bilateral_serial.h"
#include <algorithm>
#include <cmath>
ImageBuffer SerialBilateralFilter::apply(const ImageBuffer& in, int) {
    ImageBuffer out = in.clone();
    const int W=in.width, H=in.height, C=in.channels, R=RADIUS;
    const float ss2 = 2.0f*SIGMA_SPACE*SIGMA_SPACE;
    const float sc2 = 2.0f*SIGMA_COLOR*SIGMA_COLOR;
    for (int r=0;r<H;++r) for (int c=0;c<W;++c) for (int ch=0;ch<C;++ch) {
        float wsum=0, acc=0;
        float center=(float)in.at(r,c,ch);
        for (int kr=-R;kr<=R;++kr) for (int kc=-R;kc<=R;++kc) {
            int sr=std::clamp(r+kr,0,H-1), sc_=std::clamp(c+kc,0,W-1);
            float pix=(float)in.at(sr,sc_,ch);
            float spatW = expf(-(kr*kr+kc*kc)/ss2);
            float colorW= expf(-((pix-center)*(pix-center))/sc2);
            float w = spatW*colorW;
            acc  += w*pix;
            wsum += w;
        }
        out.at(r,c,ch)=(uint8_t)std::clamp((int)(acc/wsum),0,255);
    }
    return out;
}
