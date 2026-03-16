#include "sepia_omp.h"
#include <omp.h>
#include <algorithm>
ImageBuffer OmpSepiaFilter::apply(const ImageBuffer& in, int numThreads) {
    ImageBuffer out = in.clone();
    const int W=in.width,H=in.height,C=in.channels;
    omp_set_num_threads(numThreads);
    #pragma omp parallel for schedule(static) collapse(2)
    for (int r=0;r<H;++r) for (int c=0;c<W;++c) {
        float R,G,B;
        if (C>=3){R=(float)in.at(r,c,0);G=(float)in.at(r,c,1);B=(float)in.at(r,c,2);}
        else {R=G=B=(float)in.at(r,c,0);}
        int nr=(int)(R*0.393f+G*0.769f+B*0.189f);
        int ng=(int)(R*0.349f+G*0.686f+B*0.168f);
        int nb=(int)(R*0.272f+G*0.534f+B*0.131f);
        if (C>=3){
            out.at(r,c,0)=(uint8_t)std::clamp(nr,0,255);
            out.at(r,c,1)=(uint8_t)std::clamp(ng,0,255);
            out.at(r,c,2)=(uint8_t)std::clamp(nb,0,255);
        } else out.at(r,c,0)=(uint8_t)std::clamp(nr,0,255);
        if (C==4) out.at(r,c,3)=in.at(r,c,3);
    }
    return out;
}
