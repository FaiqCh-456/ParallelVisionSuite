#include "brightness_serial.h"
#include <algorithm>
ImageBuffer SerialBrightnessFilter::apply(const ImageBuffer& in, int) {
    ImageBuffer out = in.clone();
    const int W=in.width, H=in.height, C=in.channels;
    for (int r=0;r<H;++r) for (int c=0;c<W;++c) for (int ch=0;ch<C;++ch) {
        if (C==4 && ch==3) { out.at(r,c,ch)=in.at(r,c,ch); continue; }
        float v = (float)in.at(r,c,ch);
        v = (v - 128.0f) * contrast + 128.0f + brightness;
        out.at(r,c,ch) = (uint8_t)std::clamp((int)v, 0, 255);
    }
    return out;
}
