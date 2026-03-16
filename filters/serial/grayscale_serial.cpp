#include "grayscale_serial.h"
#include <algorithm>
ImageBuffer SerialGrayscaleFilter::apply(const ImageBuffer& in, int) {
    ImageBuffer out = in.clone();
    const int W=in.width, H=in.height, C=in.channels;
    for (int r=0;r<H;++r) for (int c=0;c<W;++c) {
        float grey;
        if (C>=3)
            grey = 0.299f*in.at(r,c,0) + 0.587f*in.at(r,c,1) + 0.114f*in.at(r,c,2);
        else
            grey = (float)in.at(r,c,0);
        uint8_t g = (uint8_t)std::clamp((int)grey, 0, 255);
        for (int ch=0;ch<std::min(C,3);++ch) out.at(r,c,ch) = g;
        if (C==4) out.at(r,c,3) = in.at(r,c,3);
    }
    return out;
}
