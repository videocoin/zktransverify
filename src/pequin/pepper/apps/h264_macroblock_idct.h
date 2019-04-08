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
    int i;

    block[0] += 1 << 5;

    for(i=0; i<4; i++){
        int z0=  block[i + 4*0]     +  block[i + 4*2];
        int z1=  block[i + 4*0]     +  block[i + 4*2] * (-1);
        int z2= (block[i + 4*1]>>1) +  block[i + 4*3] * (-1);
        int z3=  block[i + 4*1]     + (block[i + 4*3]>>1);

        block[i + 4*0]= z0 + z3;
        block[i + 4*1]= z1 + z2;
        block[i + 4*2]= z1 - z2;
        block[i + 4*3]= z0 - z3;
    }

    for(i=0; i<4; i++){
        int z0=  block[0 + 4*i]     +  block[2 + 4*i];
        int z1=  block[0 + 4*i]     +  block[2 + 4*i] * (-1);
        int z2= (block[1 + 4*i]>>1) +  block[3 + 4*i] * (-1);
        int z3=  block[1 + 4*i]     + (block[3 + 4*i]>>1);

        dst[i + 0*stride]= clip_pixel(dst[i + 0*stride] + ((z0 + z3) >> 6));
        dst[i + 1*stride]= clip_pixel(dst[i + 1*stride] + ((z1 + z2) >> 6));
        dst[i + 2*stride]= clip_pixel(dst[i + 2*stride] + ((z1 - z2) >> 6));
        dst[i + 3*stride]= clip_pixel(dst[i + 3*stride] + ((z0 - z3) >> 6));
    }

    // memset(block, 0, 16 * sizeof(dctcoef));
    for (i = 0; i < 16; ++i) {
        block[i] = 0;
    }
}

void chroma_dc_dequant_idct(int16_t *block, int qmul)
{
    int stride= 16*2;
    int xStride= 16;
    int a,b,c,d,e;

    a = i16_a_to_i32(block + (stride*0 + xStride*0)*2);
    b = i16_a_to_i32(block + (stride*0 + xStride*1)*2);
    c = i16_a_to_i32(block + (stride*1 + xStride*0)*2);
    d = i16_a_to_i32(block + (stride*1 + xStride*1)*2);

    e= a-b;
    a= a+b;
    b= c-d;
    c= c+d;

    i32_to_i16_a(block + (stride*0 + xStride*0)*2, ((a+c)*qmul) >> 7);
    i32_to_i16_a(block + (stride*0 + xStride*1)*2, ((e+b)*qmul) >> 7);
    i32_to_i16_a(block + (stride*1 + xStride*0)*2, ((a-c)*qmul) >> 7);
    i32_to_i16_a(block + (stride*1 + xStride*1)*2, ((e-b)*qmul) >> 7);
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
            else if((bool)bytes_to_uint32(block + i*16*2))
                idct_dc_add(p + block_offset[i], block + offset, stride);
        }
    }
}

void luma_dc_dequant_idct(int16_t *_output, int16_t *input, int qmul){
#define stride 16
    int i;
    int temp[16];
    uint8_t x_offset[4]={0, 2*stride, 8*stride, 10*stride};
//    dctcoef *input = (dctcoef*)_input;
//    dctcoef *output = (dctcoef*)_output;
//
    for(i=0; i<4; i++){
        int z0= i16_a_to_i32(input + (4*i+0)*2) + i16_a_to_i32(input + (4*i+1)*2);
        int z1= i16_a_to_i32(input + (4*i+0)*2) + i16_a_to_i32(input + (4*i+1)*2) * (-1);
        int z2= i16_a_to_i32(input + (4*i+2)*2) + i16_a_to_i32(input + (4*i+3)*2) * (-1);
        int z3= i16_a_to_i32(input + (4*i+2)*2) + i16_a_to_i32(input + (4*i+3)*2);

        temp[4*i+0]= z0+z3;
        temp[4*i+1]= z0-z3;
        temp[4*i+2]= z1-z2;
        temp[4*i+3]= z1+z2;
    }

    for(i=0; i<4; i++){
        int offset= x_offset[i];
        int z0= temp[4*0+i] + temp[4*2+i];
        int z1= temp[4*0+i] + temp[4*2+i] * (-1);
        int z2= temp[4*1+i] + temp[4*3+i] * (-1);
        int z3= temp[4*1+i] + temp[4*3+i];

        i32_to_i16_a(output + (stride* 0+offset)*2, ((((z0 + z3)*qmul + 128 ) >> 8)));
        i32_to_i16_a(output + (stride* 1+offset)*2, ((((z1 + z2)*qmul + 128 ) >> 8)));
        i32_to_i16_a(output + (stride* 4+offset)*2, ((((z1 - z2)*qmul + 128 ) >> 8)));
        i32_to_i16_a(output + (stride* 5+offset)*2, ((((z0 - z3)*qmul + 128 ) >> 8)));
    }
#undef stride
}

#endif // H264_MACROBLOCK_IDCT_H
