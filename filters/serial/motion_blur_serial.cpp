#include "motion_blur_serial.h"
#include <algorithm>
ImageBuffer SerialMotionBlurFilter::apply(const ImageBuffer& in, int) {
    ImageBuffer out = in.clone();
    const int W=in.width, H=in.height, C=in.channels, L=LENGTH;
    for (int r=0;r<H;++r) for (int c=0;c<W;++c) for (int ch=0;ch<C;++ch) {
        int acc=0, cnt=0;
        for (int k=0;k<L;++k) {
            int sc=std::clamp(c-k,0,W-1);
            acc += in.at(r,sc,ch);
            ++cnt;
        }
        out.at(r,c,ch)=(uint8_t)(acc/cnt);
    }
    return out;
}
