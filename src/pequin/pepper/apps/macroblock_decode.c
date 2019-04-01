//
// Created by taras on 19.03.19.
//

#define pixel unsigned char
#define pixel4 unsigned int
#define uint8_t unsigned char
#define int8_t char
#define int16_t short
#define uint16_t unsigned short
#define int32_t int
#define uint32_t unsigned int
#define uint64_t unsigned long long


#define WIDTH 16
#define HEIGH 16

#define CHROMA_Y_SHIFT 4

#define BITS 8
#define PIXEL_SHIFT (BITS >> 4)

#define MB_TYPE_INTRA4x4   0x0001
#define MB_TYPE_INTRA_PCM  0x0004

/**
 * Prediction types
 */
//@{
#define VERT_PRED              0
#define HOR_PRED               1
#define DC_PRED                2
#define DIAG_DOWN_LEFT_PRED    3
#define DIAG_DOWN_RIGHT_PRED   4
#define VERT_RIGHT_PRED        5
#define HOR_DOWN_PRED          6
#define VERT_LEFT_PRED         7
#define HOR_UP_PRED            8


#define IS_INTRA_PCM(a)  (bool)((a) & MB_TYPE_INTRA_PCM)
#define IS_INTRA(a)      ((a) & 7)
#define IS_INTRA4x4(a)   ((a) & MB_TYPE_INTRA4x4)

#define MB_TYPE_8x8DCT     0x01000000
#define IS_8x8DCT(a)       ((a) & MB_TYPE_8x8DCT)

#define QP_MAX_NUM (51 + 6*6)           // The maximum supported qp

//typedef union {
//    uint64_t u64;
//    uint32_t u32[2];
//    uint16_t u16[4];
//    uint8_t  u8 [8];
//    double   f64;
//    float    f32[2];
//} av_alias av_alias64;

typedef struct {
    uint8_t  u8[8];
} av_alias64;

//typedef union {
//    uint32_t u32;
//    uint16_t u16[2];
//    uint8_t  u8 [4];
//    float    f32;
//} av_alias av_alias32;

typedef struct {
    uint8_t  u8[4];
} av_alias32;

#define FFSWAP(type,a,b) {type SWAP_tmp= b; b= a; a= SWAP_tmp;}
#define AV_SWAP(n, a, b) FFSWAP(av_alias##n, *(av_alias##n*)(a), *(av_alias##n*)(b))
#define AV_SWAP64(a, b) AV_SWAP(64, a, b)

#define AV_COPY(n, d, s) \
    (((av_alias##n*)(d))->u##n = ((const av_alias##n*)(s))->u##n)
#define AV_COPY64(d, s) AV_COPY(64, d, s)
#define AV_COPY128(d, s)                       \
    do {                                       \
        AV_COPY64(d, s);                       \
        AV_COPY64((char*)(d)+8, (char*)(s)+8); \
    } while(0)

#define LUMA_DC_BLOCK_INDEX   48

// This table must be here because scan8[constant] must be known at compiletime
const uint8_t scan8[16 * 3 + 3] = {
        4 +  1 * 8, 5 +  1 * 8, 4 +  2 * 8, 5 +  2 * 8,
        6 +  1 * 8, 7 +  1 * 8, 6 +  2 * 8, 7 +  2 * 8,
        4 +  3 * 8, 5 +  3 * 8, 4 +  4 * 8, 5 +  4 * 8,
        6 +  3 * 8, 7 +  3 * 8, 6 +  4 * 8, 7 +  4 * 8,
        4 +  6 * 8, 5 +  6 * 8, 4 +  7 * 8, 5 +  7 * 8,
        6 +  6 * 8, 7 +  6 * 8, 6 +  7 * 8, 7 +  7 * 8,
        4 +  8 * 8, 5 +  8 * 8, 4 +  9 * 8, 5 +  9 * 8,
        6 +  8 * 8, 7 +  8 * 8, 6 +  9 * 8, 7 +  9 * 8,
        4 + 11 * 8, 5 + 11 * 8, 4 + 12 * 8, 5 + 12 * 8,
        6 + 11 * 8, 7 + 11 * 8, 6 + 12 * 8, 7 + 12 * 8,
        4 + 13 * 8, 5 + 13 * 8, 4 + 14 * 8, 5 + 14 * 8,
        6 + 13 * 8, 7 + 13 * 8, 6 + 14 * 8, 7 + 14 * 8,
        0 +  0 * 8, 0 +  5 * 8, 0 + 10 * 8
};

struct In {
    int slice_num;

    int qscale;

    // deblock
    int deblocking_filter;          ///< disable_deblocking_filter_idc with 1 <-> 0

    int8_t intra4x4_pred_mode_cache[5 * 8];

    unsigned int topleft_samples_available;
    unsigned int top_samples_available;
    unsigned int topright_samples_available;
    unsigned int left_samples_available;

    int linesize, uvlinesize;

    int mb_x, mb_y;
    int mb_xy;
    // index of the first MB of the next slice
    int is_complex;
    int mb_type;

    int top_type;

    int mb_field_decoding_flag;

    /**
     * non zero coeff count cache.
     * is 64 if not available.
     */
    uint8_t non_zero_count_cache[15 * 8];

    uint8_t top_borders[2][(16 * 3) * 2];

    ///< as a dct coefficient is int32_t in high depth, we need to reserve twice the space.
    int16_t mb[16 * 48 * 2];
    int16_t mb_luma_dc[16 * 2];
    
    // END OF SLICE CONTEXT
    int mb_stride;
    int mb_height, mb_width;

    uint16_t slice_table[WIDTH*HEIGH]; // TODO: W and H are dummy values
    /**
     * block_offset[ 0..23] for frame macroblocks
     * block_offset[24..47] for field macroblocks
     */
    int block_offset[2 * (16 * 3)];
    uint32_t dequant4_coeff[QP_MAX_NUM + 1][16];

    pixel dest_y[WIDTH*HEIGH];
    pixel dest_cb[WIDTH*HEIGH / 4];
    pixel dest_cr[WIDTH*HEIGH / 4];
};

struct Out {
    pixel dest_y[WIDTH*HEIGH];
    pixel dest_cb[WIDTH*HEIGH / 4];
    pixel dest_cr[WIDTH*HEIGH / 4];
};

#define SRC(x,y) src[(x)+(y)*stride]
#define PL(y) \
    const int l##y = (SRC(-1,y-1) + 2*SRC(-1,y) + SRC(-1,y+1) + 2) >> 2;
#define PREDICT_8x8_LOAD_LEFT \
    const int l0 = ((has_topleft ? SRC(-1,-1) : SRC(-1,0)) \
                     + 2*SRC(-1,0) + SRC(-1,1) + 2) >> 2; \
    PL(1) PL(2) PL(3) PL(4) PL(5) PL(6) \
    const int l7 = (SRC(-1,6) + 3*SRC(-1,7) + 2) >> 2

#define PT(x) \
    const int t##x = (SRC(x-1,-1) + 2*SRC(x,-1) + SRC(x+1,-1) + 2) >> 2;
#define PREDICT_8x8_LOAD_TOP \
    const int t0 = ((has_topleft ? SRC(-1,-1) : SRC(0,-1)) \
                     + 2*SRC(0,-1) + SRC(1,-1) + 2) >> 2; \
    PT(1) PT(2) PT(3) PT(4) PT(5) PT(6) \
    const int t7 = ((has_topright ? SRC(8,-1) : SRC(7,-1)) \
                     + 2*SRC(7,-1) + SRC(6,-1) + 2) >> 2

#define PIXEL_SPLAT_X4(x) ((x)*0x01010101U)

#define AV_WNA(s, p, v) (((av_alias##s*)(p))->u##s = (v))
#define AV_WN32A(p, v) AV_WNA(32, p, v)
#define AV_WN4PA AV_WN32A
//
//#define PREDICT_8x8_DC(v) \
//    int y; \
//    for( y = 0; y < 8; y++ ) { \
//        AV_WN4PA(((pixel4*)src)+0, v); \
//        AV_WN4PA(((pixel4*)src)+1, v); \
//        src += stride; \
//    }
//
//#define AV_RNA(s, p)    (((const av_alias##s*)(p))->u##s)
//#define AV_RN16A(p) AV_RNA(16, p)
//#define AV_RN32A(p) AV_RNA(32, p)

void pred8x8l_dc(uint8_t *src, int has_topleft, int has_topright, int stride)
{

    PREDICT_8x8_LOAD_LEFT;
    PREDICT_8x8_LOAD_TOP;
    pixel4 v = (l0+l1+l2+l3+l4+l5+l6+l7+t0+t1+t2+t3+t4+t5+t6+t7+8) >> 4;
    pixel4 dc = v * 0x01010101U;
//    PREDICT_8x8_DC(dc);
    int y;
    for (y = 0; y < 8; ++y) {
        uint8_t *p = src;

        p[0] = dc & 0xFF;
        p[1] = (dc >> 8) & 0xFF;
        p[2] = (dc >> 16) & 0xFF;
        p[3] = (dc >> 24) & 0xFF;

        p[4] = dc & 0xFF;
        p[5] = (dc >> 8) & 0xFF;
        p[6] = (dc >> 16) & 0xFF;
        p[7] = (dc >> 24) & 0xFF;
        src += stride;
    }
}

void pred8x8_dc_rv40_c(uint8_t *src, int stride)
{
    int i;
    unsigned int dc0 = 0;

    for(i=0;i<4; i++){
        dc0+= src[-1+i*stride] + src[i-stride];
        dc0+= src[4+i-stride];
        dc0+= src[-1+(i+4)*stride];
    }
    dc0= 0x01010101*((dc0 + 8)>>4);

    for(i=0; i<4; i++){
//        ((uint32_t*)(src+i*stride))[0]= dc0;
        uint8_t *p = src + i * stride;
        p[0] = 0xFF & dc0;
        p[1] = (dc0 >> 8) & 0xFF;
        p[2] = (dc0 >> 16) & 0xFF;
        p[3] = (dc0 >> 24) & 0xFF;

//        ((uint32_t*)(src+i*stride))[1]= dc0;
        p[4] = 0xFF & dc0;
        p[5] = (dc0 >> 8) & 0xFF;
        p[6] = (dc0 >> 16) & 0xFF;
        p[7] = (dc0 >> 24) & 0xFF;

    }
    for(i=4; i<8; i++){
        uint8_t *p = src + i * stride;
//        ((uint32_t*)(src+i*stride))[0]= dc0;
        p[0] = 0xFF & dc0;
        p[1] = (dc0 >> 8) & 0xFF;
        p[2] = (dc0 >> 16) & 0xFF;
        p[3] = (dc0 >> 24) & 0xFF;
//        ((uint32_t*)(src+i*stride))[1]= dc0;
        p[4] = 0xFF & dc0;
        p[5] = (dc0 >> 8) & 0xFF;
        p[6] = (dc0 >> 16) & 0xFF;
        p[7] = (dc0 >> 24) & 0xFF;
    }
}

void pred4x4_dc(uint8_t *src, const uint8_t *topright,
                              int stride)
{
    const int dc= (  src[-stride] + src[1-stride] + src[2-stride] + src[3-stride]
                     + src[-1+0*stride] + src[-1+1*stride] + src[-1+2*stride] + src[-1+3*stride] + 4) >>3;
    const pixel4 a = dc * 0x01010101U;

//            AV_WN4PA(src+0*stride, a);
//            AV_WN4PA(src+1*stride, a);
//            AV_WN4PA(src+2*stride, a);
//            AV_WN4PA(src+3*stride, a);

    uint8_t *p = src;
    p[0] = a & 0xFF;
    p[1] = (a >> 8) & 0xFF;
    p[2] = (a >> 16) & 0xFF;
    p[3] = (a >> 24) & 0xFF;

    p += stride;
    p[0] = a & 0xFF;
    p[1] = (a >> 8) & 0xFF;
    p[2] = (a >> 16) & 0xFF;
    p[3] = (a >> 24) & 0xFF;

    p += stride * 2;
    p[0] = a & 0xFF;
    p[1] = (a >> 8) & 0xFF;
    p[2] = (a >> 16) & 0xFF;
    p[3] = (a >> 24) & 0xFF;

    p += stride * 3;
    p[0] = a & 0xFF;
    p[1] = (a >> 8) & 0xFF;
    p[2] = (a >> 16) & 0xFF;
    p[3] = (a >> 24) & 0xFF;
}

#define PREDICT_16x16_DC(v)\
    for(i=0; i<16; i++){\
        AV_WN4PA(src+ 0, v);\
        AV_WN4PA(src+ 4, v);\
        AV_WN4PA(src+ 8, v);\
        AV_WN4PA(src+12, v);\
        src += stride;\
    }

void pred16x16_dc(uint8_t *src, int stride)
{
    int i, dc=0;
    uint32_t dcsplat;

    for(i=0;i<16; i++){
//        dc+= src[-1+i*stride];
        uint8_t *src_offset = src + i * stride - 1;
        dc += src_offset[0];
    }

    for(i=0;i<16; i++){
//        dc+= src[i-stride];
        uint8_t *src_offset = src + i - stride;
        dc += src_offset[0];
    }

    dcsplat = PIXEL_SPLAT_X4((dc+16)>>5);

    for (i = 0; i < 16; i++) {
//          PREDICT_16x16_DC(dcsplat);
        uint8_t *p = src;
        p[0] = dcsplat & 0xFF;
        p[1] = (dcsplat >> 8) & 0xFF;
        p[2] = (dcsplat >> 16) & 0xFF;
        p[3] = (dcsplat >> 24) & 0xFF;

        p[4] = dcsplat & 0xFF;
        p[5] = (dcsplat >> 8) & 0xFF;
        p[6] = (dcsplat >> 16) & 0xFF;
        p[7] = (dcsplat >> 24) & 0xFF;

        p[8] = dcsplat & 0xFF;
        p[9] = (dcsplat >> 8) & 0xFF;
        p[10] = (dcsplat >> 16) & 0xFF;
        p[11] = (dcsplat >> 24) & 0xFF;

        p[12] = dcsplat & 0xFF;
        p[13] = (dcsplat >> 8) & 0xFF;
        p[14] = (dcsplat >> 16) & 0xFF;
        p[15] = (dcsplat >> 24) & 0xFF;

        src += stride;
    }
}

void ff_h264_luma_dc_dequant_idct(int16_t *output, int16_t *input, int qmul){
#define stride 16
    int i;
    int temp[16];
    static const uint8_t x_offset[4]={0, 2*stride, 8*stride, 10*stride};

    for(i=0; i<4; i++){
        const int z0= input[4*i+0] + input[4*i+1];
        const int z1= input[4*i+0] - input[4*i+1];
        const int z2= input[4*i+2] - input[4*i+3];
        const int z3= input[4*i+2] + input[4*i+3];

        temp[4*i+0]= z0+z3;
        temp[4*i+1]= z0-z3;
        temp[4*i+2]= z1-z2;
        temp[4*i+3]= z1+z2;
    }

    for(i=0; i<4; i++){
        const int offset= x_offset[i];
        const int z0= temp[4*0+i] + temp[4*2+i];
        const int z1= temp[4*0+i] - temp[4*2+i];
        const int z2= temp[4*1+i] - temp[4*3+i];
        const int z3= temp[4*1+i] + temp[4*3+i];

        output[stride* 0+offset]= ((((z0 + z3)*qmul + 128 ) >> 8));
        output[stride* 1+offset]= ((((z1 + z2)*qmul + 128 ) >> 8));
        output[stride* 4+offset]= ((((z1 - z2)*qmul + 128 ) >> 8));
        output[stride* 5+offset]= ((((z0 - z3)*qmul + 128 ) >> 8));
    }
#undef stride
}

uint8_t av_clip_pixel(int a)
{
    int v = a;
    if (a&(~0xFF))
        v = (-a)>>31;
//    if (a&(~0xFF)) return (-a)>>31;
//    else           return a;

    return v;
}

void ff_h264_idct8_dc_add(uint8_t *dst, int16_t *block, int stride) {
    int i, j;
    int dc = (block[0] + 32) >> 6;
    block[0] = 0;
    for( j = 0; j < 8; j++ )
    {
        for( i = 0; i < 8; i++ )
            dst[i] = av_clip_pixel( dst[i] + dc );
        dst += stride;
    }
}

void ff_h264_idct8_add(uint8_t *dst, int16_t *block, int stride){
    int i;

    block[0] += 32;

    for( i = 0; i < 8; i++ )
    {
        const int a0 =  block[i+0*8] + block[i+4*8];
        const int a2 =  block[i+0*8] - block[i+4*8];
        const int a4 = (block[i+2*8]>>1) - block[i+6*8];
        const int a6 = (block[i+6*8]>>1) + block[i+2*8];

        const int b0 = a0 + a6;
        const int b2 = a2 + a4;
        const int b4 = a2 - a4;
        const int b6 = a0 - a6;

        const int a1 = -block[i+3*8] + block[i+5*8] - block[i+7*8] - (block[i+7*8]>>1);
        const int a3 =  block[i+1*8] + block[i+7*8] - block[i+3*8] - (block[i+3*8]>>1);
        const int a5 = -block[i+1*8] + block[i+7*8] + block[i+5*8] + (block[i+5*8]>>1);
        const int a7 =  block[i+3*8] + block[i+5*8] + block[i+1*8] + (block[i+1*8]>>1);

        const int b1 = (a7>>2) + a1;
        const int b3 =  a3 + (a5>>2);
        const int b5 = (a3>>2) - a5;
        const int b7 =  a7 - (a1>>2);

        block[i+0*8] = b0 + b7;
        block[i+7*8] = b0 - b7;
        block[i+1*8] = b2 + b5;
        block[i+6*8] = b2 - b5;
        block[i+2*8] = b4 + b3;
        block[i+5*8] = b4 - b3;
        block[i+3*8] = b6 + b1;
        block[i+4*8] = b6 - b1;
    }
    for( i = 0; i < 8; i++ )
    {
        const int a0 =  block[0+i*8] + block[4+i*8];
        const int a2 =  block[0+i*8] - block[4+i*8];
        const int a4 = (block[2+i*8]>>1) - block[6+i*8];
        const int a6 = (block[6+i*8]>>1) + block[2+i*8];

        const int b0 = a0 + a6;
        const int b2 = a2 + a4;
        const int b4 = a2 - a4;
        const int b6 = a0 - a6;

        const int a1 = -block[3+i*8] + block[5+i*8] - block[7+i*8] - (block[7+i*8]>>1);
        const int a3 =  block[1+i*8] + block[7+i*8] - block[3+i*8] - (block[3+i*8]>>1);
        const int a5 = -block[1+i*8] + block[7+i*8] + block[5+i*8] + (block[5+i*8]>>1);
        const int a7 =  block[3+i*8] + block[5+i*8] + block[1+i*8] + (block[1+i*8]>>1);

        const int b1 = (a7>>2) + a1;
        const int b3 =  a3 + (a5>>2);
        const int b5 = (a3>>2) - a5;
        const int b7 =  a7 - (a1>>2);

        dst[i + 0*stride] = av_clip_pixel( dst[i + 0*stride] + ((b0 + b7) >> 6) );
        dst[i + 1*stride] = av_clip_pixel( dst[i + 1*stride] + ((b2 + b5) >> 6) );
        dst[i + 2*stride] = av_clip_pixel( dst[i + 2*stride] + ((b4 + b3) >> 6) );
        dst[i + 3*stride] = av_clip_pixel( dst[i + 3*stride] + ((b6 + b1) >> 6) );
        dst[i + 4*stride] = av_clip_pixel( dst[i + 4*stride] + ((b6 - b1) >> 6) );
        dst[i + 5*stride] = av_clip_pixel( dst[i + 5*stride] + ((b4 - b3) >> 6) );
        dst[i + 6*stride] = av_clip_pixel( dst[i + 6*stride] + ((b2 - b5) >> 6) );
        dst[i + 7*stride] = av_clip_pixel( dst[i + 7*stride] + ((b0 - b7) >> 6) );
    }

    // memset(block, 0, 64 * sizeof(dctcoef));
    for (i = 0; i < 64; ++i) {
        block[i] = 0;
    }
}

// assumes all AC coefs are 0
void ff_h264_idct_dc_add(uint8_t *dst, int16_t *block, int stride){
    int i, j;
    int dc = (block[0] + 32) >> 6;
    block[0] = 0;
    for( j = 0; j < 4; j++ )
    {
        for( i = 0; i < 4; i++ )
            dst[i] = av_clip_pixel( dst[i] + dc );
        dst += stride;
    }
}

void ff_h264_idct_add(uint8_t *dst, int16_t *block, int stride)
{
    int i;

    block[0] += 1 << 5;

    for(i=0; i<4; i++){
        const int z0=  block[i + 4*0]     +  block[i + 4*2];
        const int z1=  block[i + 4*0]     -  block[i + 4*2];
        const int z2= (block[i + 4*1]>>1) -  block[i + 4*3];
        const int z3=  block[i + 4*1]     + (block[i + 4*3]>>1);

        block[i + 4*0]= z0 + z3;
        block[i + 4*1]= z1 + z2;
        block[i + 4*2]= z1 - z2;
        block[i + 4*3]= z0 - z3;
    }

    for(i=0; i<4; i++){
        const int z0=  block[0 + 4*i]     +  block[2 + 4*i];
        const int z1=  block[0 + 4*i]     -  block[2 + 4*i];
        const int z2= (block[1 + 4*i]>>1) -  block[3 + 4*i];
        const int z3=  block[1 + 4*i]     + (block[3 + 4*i]>>1);

        dst[i + 0*stride]= av_clip_pixel(dst[i + 0*stride] + ((z0 + z3) >> 6));
        dst[i + 1*stride]= av_clip_pixel(dst[i + 1*stride] + ((z1 + z2) >> 6));
        dst[i + 2*stride]= av_clip_pixel(dst[i + 2*stride] + ((z1 - z2) >> 6));
        dst[i + 3*stride]= av_clip_pixel(dst[i + 3*stride] + ((z0 - z3) >> 6));
    }

    // memset(block, 0, 16 * sizeof(dctcoef));
    for (i = 0; i < 16; ++i) {
        block[i] = 0;
    }
}

int dctcoef_get(int16_t *mb, int high_bit_depth, int index)
{
    int32_t v = mb[index];
    if (high_bit_depth) {
//        return AV_RN32A(((int32_t *)mb) + index);
        index *= 2;
        int16_t *p_mb = mb + index;

        v  = p_mb[0];
        v |= p_mb[1] << 16;
    }

    return v;
}



void XCHG(uint8_t *a, uint8_t *b, int xchg, int pixel_shift) {
    int i;
    if (pixel_shift) {
        if (xchg) {
            uint8_t *t = b;
            b = a;
            a = b;

            tb = b + 8;
            ta = a + 8;
            t = tb;
            tb = ta;
            ta = t;
        } else {
            for (i = 0; i < 16; ++i) {
                b[i] = a[i];
            }
        }
    } else if (xchg) {
        uint8_t *t = b;
        b = a;
        a = b;
    }
    else {
        for (i = 0; i < 8; ++i) {
            b[i] = a[i];
        }
    }
}

void xchg_mb_border(struct In *input,
                    uint8_t *src_y,
                    uint8_t *src_cb, uint8_t *src_cr,
                    int xchg, int pixel_shift) {
    int deblock_topleft;
    int deblock_top;
    int top_idx = 1;
    uint8_t *top_border_m1;
    uint8_t *top_border;

    if (input->deblocking_filter == 2) {
        deblock_topleft = input->slice_table[input->mb_xy - 1 - input->mb_stride] == input->slice_num;
        deblock_top     = input->top_type;
    } else {
        deblock_topleft = (input->mb_x > 0);
        deblock_top     = (input->mb_y > (int)!!(bool)(input->mb_field_decoding_flag));
    }

    src_y  -= input->linesize   + 1 + pixel_shift;
    src_cb -= input->uvlinesize + 1 + pixel_shift;
    src_cr -= input->uvlinesize + 1 + pixel_shift;

    top_border_m1 = input->top_borders[top_idx][input->mb_x - 1];
    top_border    = input->top_borders[top_idx][input->mb_x];

    if (deblock_top) {
        if (deblock_topleft) {
            XCHG(top_border_m1 + (8 << pixel_shift), src_y - (7 << pixel_shift), 1, pixel_shift);
        }

        XCHG(top_border + (0 << pixel_shift), src_y + (1 << pixel_shift), xchg, pixel_shift);
        XCHG(top_border + (8 << pixel_shift), src_y + (9 << pixel_shift), 1, pixel_shift);

        if (input->mb_x + 1 < input->mb_width) {
            XCHG(input->top_borders[top_idx][input->mb_x + 1],
                 src_y + (17 << pixel_shift), 1, pixel_shift);
        }

        if (deblock_topleft) {
            XCHG(top_border_m1 + (16 << pixel_shift), src_cb - (7 << pixel_shift), 1, pixel_shift);
            XCHG(top_border_m1 + (24 << pixel_shift), src_cr - (7 << pixel_shift), 1, pixel_shift);
        }
        XCHG(top_border + (16 << pixel_shift), src_cb + 1 + pixel_shift, 1, pixel_shift);
        XCHG(top_border + (24 << pixel_shift), src_cr + 1 + pixel_shift, 1, pixel_shift);
    }
}

void hl_decode_mb_predict_luma(struct In *input,
                               int pixel_shift,
                               uint8_t *dest_y)
{
    int i;
    int qscale = input->qscale;
    if (IS_INTRA4x4(input->mb_type)) {
        if (IS_8x8DCT(input->mb_type)) {
            for (i = 0; i < 16; i += 4) {
                uint8_t *ptr = dest_y + input->block_offset[i];
                const int dir = input->intra4x4_pred_mode_cache[scan8[i]];
                const int nnz = input->non_zero_count_cache[scan8[i]];
//                // TODO: find right pred8x8l
//                // h->hpc.pred8x8l[dir](ptr, (sl->topleft_samples_available << i) & 0x8000,
//                //                                         (sl->topright_samples_available << i) & 0x4000, linesize);
//
                pred8x8l_dc(ptr, (input->topleft_samples_available << i) & 0x8000,
                        (input->topright_samples_available << i) & 0x4000, input->linesize);

                if (nnz) {
                    int index = (i * 16) << pixel_shift;
                    int16_t *mb = input->mb + index;
                    if (nnz == 1 && (bool)dctcoef_get(input->mb, pixel_shift, i * 16))
                        ff_h264_idct8_dc_add(ptr, mb, input->linesize);
                    else
                        ff_h264_idct8_add(ptr, mb, input->linesize);
                }
            }
        } else {
            for (i = 0; i < 16; i++) {
                uint8_t *ptr = dest_y + input->block_offset[i];
                const int dir      = input->intra4x4_pred_mode_cache[scan8[i]];

                uint8_t *topright;
                int nnz, tr;
                uint64_t tr_high;
                if (dir == DIAG_DOWN_LEFT_PRED || dir == VERT_LEFT_PRED) {
                    const int topright_avail = (input->topright_samples_available << i) & 0x8000;
//                    av_assert2(sl->mb_y || linesize <= block_offset[i]);
                    if (!(bool)topright_avail) {
                        if (pixel_shift) {
                            int index = (3 - input->linesize / 2) * 2;
                            uint16_t v = ptr[index];
                            v |= ptr[index + 1] << 0xFF;

//                            tr_high  = ((uint16_t *)ptr)[3 - input->linesize / 2] * 0x0001000100010001ULL;
                            tr_high = v * 0x0001000100010001ULL;
                            // TODO: NOT SURE THIS GOING TO WORK. Compiler is very capricious
//                            topright = (uint8_t *)&tr_high;
                            // topright = &tr_high;
                        } else {
                            uint8_t *t_ptr = ptr + 3 - input->linesize;
                            int v = t_ptr[0];
                            tr       = v * 0x01010101u;
                            // TODO: NOT SURE THIS GOING TO WORK. Compiler is very capricious
//                            topright = (uint8_t *)&tr;
                            // topright = &tr;
                        }
                    } else
                        topright = ptr + (4 << pixel_shift) - input->linesize;
                } else
                    topright = 0;

                // TODO: find right pred
//                h->hpc.pred4x4[dir](ptr, topright, linesize);
                pred4x4_dc(ptr, topright, input->linesize);
                nnz = input->non_zero_count_cache[scan8[i]];
                if (nnz) {
                    if (nnz == 1 && (bool)dctcoef_get(input->mb, pixel_shift, i * 16))
                        ff_h264_idct_dc_add(ptr, input->mb + (i * 16), input->linesize);
                    else
                        ff_h264_idct_add(ptr, input->mb + (i * 16), input->linesize);
                }
            }
        }
    } else {
////        h->hpc.pred16x16[sl->intra16x16_pred_mode](dest_y, linesize);
        pred16x16_dc(input->dest_y, input->linesize);
        if (input->non_zero_count_cache[scan8[LUMA_DC_BLOCK_INDEX]]) {
////                h->h264dsp.h264_luma_dc_dequant_idct(input->mb,
////                                                     input->mb_luma_dc,
////                                                     h->dequant4_coeff[p][qscale][0]);
            ff_h264_luma_dc_dequant_idct(input->mb, input->mb_luma_dc, input->dequant4_coeff[qscale][0] );
        }
    }
}


void hl_decode_mb_simple_8(struct In *input) {
    uint8_t *dest_y, *dest_cb, *dest_cr;

    if (IS_INTRA(input->mb_type)) {
        if (input->deblocking_filter)
            xchg_mb_border(input, input->dest_y, input->dest_cb, input->dest_cr, 1, PIXEL_SHIFT);

        // TODO: understand the right predicition functions
//        h->hpc.pred8x8[sl->chroma_pred_mode](dest_cb, uvlinesize);
//        h->hpc.pred8x8[sl->chroma_pred_mode](dest_cr, uvlinesize);
        pred8x8_dc_rv40_c(dest_cb, input->uvlinesize);
        pred8x8_dc_rv40_c(dest_cb, input->uvlinesize);


        hl_decode_mb_predict_luma(input, PIXEL_SHIFT, dest_y);

        if (input->deblocking_filter)
            xchg_mb_border(input, dest_y, dest_cb, dest_cr, 0, PIXEL_SHIFT);
    }
}

void compute(struct In *input, struct Out *output) {
    bool is_complex = (bool)input->is_complex ||
                     IS_INTRA_PCM(input->mb_type) || (input->qscale == 0);

    if (is_complex) {

    } else {
        hl_decode_mb_simple_8(input);
    }

    int i;
    for ( i = 0; i < WIDTH * HEIGH; ++i) {
        output->dest_y[i] = input->dest_y[i];
    }
    for (i = 0; i < WIDTH *HEIGH / 4; ++i) {
        output->dest_cb[i] = input->dest_cb[i];
        output->dest_cr[i] = input->dest_cr[i];
    }
}