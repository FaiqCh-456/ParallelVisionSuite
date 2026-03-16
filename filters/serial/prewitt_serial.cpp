#include "prewitt_serial.h"
#include <cmath>
#include <algorithm>
ImageBuffer SerialPrewittFilter::apply(const ImageBuffer& in, int) {
    ImageBuffer out = in.clone();
    const int W=in.width, H=in.height, C=in.channels;
    static const int Gx[3][3]={{-1,0,1},{-1,0,1},{-1,0,1}};
    static const int Gy[3][3]={{-1,-1,-1},{0,0,0},{1,1,1}};
    for (int r=1;r<H-1;++r) for (int c=1;c<W-1;++c) for (int ch=0;ch<C;++ch) {
        float gx=0,gy=0;
        for (int kr=-1;kr<=1;++kr) for (int kc=-1;kc<=1;++kc) {
            float px=in.at(r+kr,c+kc,ch);
            gx+=Gx[kr+1][kc+1]*px; gy+=Gy[kr+1][kc+1]*px;
        }
        out.at(r,c,ch)=(uint8_t)std::clamp((int)std::sqrt(gx*gx+gy*gy),0,255);
    }
    return out;
}
