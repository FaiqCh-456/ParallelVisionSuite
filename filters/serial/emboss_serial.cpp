#include "emboss_serial.h"
#include <algorithm>
ImageBuffer SerialEmbossFilter::apply(const ImageBuffer& in, int) {
    ImageBuffer out = in.clone();
    const int W=in.width, H=in.height, C=in.channels;
    static const int K[3][3] = {{-2,-1,0},{-1,1,1},{0,1,2}};
    for (int r=1;r<H-1;++r) for (int c=1;c<W-1;++c) for (int ch=0;ch<C;++ch) {
        int acc=0;
        for (int kr=-1;kr<=1;++kr) for (int kc=-1;kc<=1;++kc)
            acc += K[kr+1][kc+1] * in.at(r+kr,c+kc,ch);
        out.at(r,c,ch) = (uint8_t)std::clamp(acc+128, 0, 255);
    }
    return out;
}
