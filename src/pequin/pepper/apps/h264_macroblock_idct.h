//
// Created by taras on 05.04.19.
//

#ifndef H264_MACROBLOCK_IDCT_H
#define H264_MACROBLOCK_IDCT_H

// assumes all AC coefs are 0
void idct_dc_add(uint8_t *dst, int16_t *block, int stride){
    int i, j;
    int dc = (block[0] + 32) >> 6;
    block[0] = 0;
    for( j = 0; j < 4; j++ )
    {
        for( i = 0; i < 4; i++ )
            dst[i] = clip_pixel( dst[i] + dc );
        dst += stride;
    }
}

void idct_add(uint8_t *dst, int16_t *block, int stride)
{
//    int i;
//
//    block[0] += 1 << 5;
//
//    for(i=0; i<4; i++){
//        int z0=  block[i + 4*0]     +  block[i + 4*2];
//        int z1=  block[i + 4*0]     -  block[i + 4*2];
//        int z2= (block[i + 4*1]>>1) -  block[i + 4*3];
//        int z3=  block[i + 4*1]     + (block[i + 4*3]>>1);
//
//        block[i + 4*0]= z0 + z3;
//        block[i + 4*1]= z1 + z2;
//        block[i + 4*2]= z1 - z2;
//        block[i + 4*3]= z0 - z3;
//    }
//
//    for(i=0; i<4; i++){
//        int z0=  block[0 + 4*i]     +  block[2 + 4*i];
//        int z1=  block[0 + 4*i]     -  block[2 + 4*i];
//        int z2= (block[1 + 4*i]>>1) -  block[3 + 4*i];
//        int z3=  block[1 + 4*i]     + (block[3 + 4*i]>>1);
//
//        dst[i + 0*stride]= clip_pixel(dst[i + 0*stride] + ((z0 + z3) >> 6));
//        dst[i + 1*stride]= clip_pixel(dst[i + 1*stride] + ((z1 + z2) >> 6));
//        dst[i + 2*stride]= clip_pixel(dst[i + 2*stride] + ((z1 - z2) >> 6));
//        dst[i + 3*stride]= clip_pixel(dst[i + 3*stride] + ((z0 - z3) >> 6));
//    }
//
//    // memset(block, 0, 16 * sizeof(dctcoef));
//    for (i = 0; i < 16; ++i) {
//        block[i] = 0;
//    }
}

void chroma_dc_dequant_idct(int16_t *block, int qmul)
{
//    int stride= 16*2;
//    int xStride= 16;
//    int a,b,c,d,e;
//    dctcoef *block = (dctcoef*)_block;
//
//    a= block[stride*0 + xStride*0];
//    b= block[stride*0 + xStride*1];
//    c= block[stride*1 + xStride*0];
//    d= block[stride*1 + xStride*1];
//
//    e= a-b;
//    a= a+b;
//    b= c-d;
//    c= c+d;
//
//    block[stride*0 + xStride*0]= ((a+c)*qmul) >> 7;
//    block[stride*0 + xStride*1]= ((e+b)*qmul) >> 7;
//    block[stride*1 + xStride*0]= ((a-c)*qmul) >> 7;
//    block[stride*1 + xStride*1]= ((e-b)*qmul) >> 7;
}

void idct_add8(uint8_t *dest_cb, uint8_t  *dest_cr, int *block_offset, int16_t *block, int stride, uint8_t nnzc[15*8]){
    int i, j;
    uint8_t *dest[2] = { dest_cb, dest_cr };
    for(j=1; j<3; j++){
        uint8_t *p = j == 1 ? dest_cb : dest_cr;

        for(i=j*16; i<j*16+4; i++){
            int offset = i * 16;
            if((bool)nnzc[ scan8[i] ])
                idct_add(p + block_offset[i], block + offset, stride);
            else {
                uint32_t dctcoef = block[i*16*2];
                dctcoef |= block[i*16*2+1];
                if((bool)dctcoef)
                    idct_dc_add(p + block_offset[i], block + offset, stride);
            }
        }
    }
}

#endif // H264_MACROBLOCK_IDCT_H
