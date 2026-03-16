#include "box_blur_serial.h"
#include <algorithm>
ImageBuffer SerialBoxBlurFilter::apply(const ImageBuffer& in, int) {
    ImageBuffer out = in.clone();
    const int W=in.width, H=in.height, C=in.channels, R=RADIUS;
    int count = (2*R+1)*(2*R+1);
    for (int r=0;r<H;++r) for (int c=0;c<W;++c) for (int ch=0;ch<C;++ch) {
        int acc=0;
        for (int kr=-R;kr<=R;++kr) for (int kc=-R;kc<=R;++kc) {
            int sr=std::clamp(r+kr,0,H-1), sc=std::clamp(c+kc,0,W-1);
            acc += in.at(sr,sc,ch);
        }
        out.at(r,c,ch) = (uint8_t)(acc/count);
    }
    return out;
}
