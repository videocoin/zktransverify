//
// Created by taras on 04.04.19.
//

#ifndef H264_MACROBLOCK_PREDICTION_H
#define H264_MACROBLOCK_PREDICTION_H

#define DC_PRED8x8             0
#define HOR_PRED8x8            1
#define VERT_PRED8x8           2
#define PLANE_PRED8x8          3

// DC edge
#define LEFT_DC_PRED8x8        4
#define TOP_DC_PRED8x8         5
#define DC_128_PRED8x8         6

// H264/SVQ3 (8x8) specific
#define ALZHEIMER_DC_L0T_PRED8x8  7
#define ALZHEIMER_DC_0LT_PRED8x8  8
#define ALZHEIMER_DC_L00_PRED8x8  9
#define ALZHEIMER_DC_0L0_PRED8x8 10

#define PIXEL_SPLAT_X4(x)	   ((x)*0x01010101U)

uint32_t AV_RN4PA(uint8_t *src)
{
    uint32_t v;
    v  = src[0];
    v |= src[1] << 8;
    v |= src[2] << 16
    v |= src[3] << 24;
    return v;
}

void AV_WN4PA(uint8_t *src, uint32_t v)
{
    src[0] = v & 0xFF;
    src[1] = (v >> 8) & 0xFF;
    src[2] = (v >> 16) & 0xFF;
    src[3] = (v >> 24) & 0xFF;
}

uint8_t clip_pixel(int a) {
    int v = a;
    if (a & (~0xFF))
        v = (-a) >> 31;

    return v;
}

void pred8x8_dc_8(uint8_t *src, int stride)
{
    int i;
    int dc0, dc1, dc2;
    uint32_t dc0splat, dc1splat, dc2splat, dc3splat;

    dc0=dc1=dc2=0;
    for(i=0;i<4; i++){
        dc0+= src[i*stride-1] + src[i-stride];
        dc1+= src[4+i-stride];
        dc2+= src[(i+4)*stride-1];
    }

    dc0splat = PIXEL_SPLAT_X4((dc0 + 4)>>3);
    dc1splat = PIXEL_SPLAT_X4((dc1 + 2)>>2);
    dc2splat = PIXEL_SPLAT_X4((dc2 + 2)>>2);
    dc3splat = PIXEL_SPLAT_X4((dc1 + dc2 + 4)>>3);

    for(i=0; i<4; i++){
        AV_WN4PA(src + i*stride, dc0splat);
        AV_WN4PA(src + i*stride + 4, dc1splat);
    }

    for(i=4; i<8; i++){
        AV_WN4PA(src + i*stride, dc2splat);
        AV_WN4PA(src + i*stride + 4, dc3splat);
    }
}

void pred8x8_horizontal_8(uint8_t *src, int stride)
{
    int i;
    pixel *src = (pixel*)_src;

    for(i=0; i<8; i++){
        uint32_t a = PIXEL_SPLAT_X4(src[i*stride - 1]);

        AV_WN4PA(src + i*stride, a);
        AV_WN4PA(src + i*stride + 4, a);
    }
}

void pred8x8_vertical_8(uint8_t *src, int stride)
{
    int i;
    const pixel *a = src-stride;

    for(i=0; i<8; i++){
        src[i*stride + 0] = a[0];
        src[i*stride + 1] = a[1];
        src[i*stride + 2] = a[2];
        src[i*stride + 3] = a[3];

        src[i*stride + 4] = a[4];
        src[i*stride + 5] = a[5];
        src[i*stride + 6] = a[6];
        src[i*stride + 7] = a[7];
    }
}

static void pred8x8_plane_8(uint8_t *src, int stride)
{
    int j, k;
    int a;

    const pixel * src0 = src +3-stride;
    const pixel * src1 = src +4*stride-1;
    const pixel * src2 = src1-2*stride;    // == src+2*stride-1;
    int H = src0[1] - src0[-1];
    int V = src1[0] - src2[ 0];
    for(k=2; k<=4; ++k) {
        src1 += stride; src2 -= stride;
        H += k*(src0[k] - src0[-k]);
        V += k*(src1[0] - src2[ 0]);
    }
    H = ( 17*H+16 ) >> 5;
    V = ( 17*V+16 ) >> 5;

    a = 16*(src1[0] + src2[8]+1) - 3*(V+H);
    for(j=8; j>0; --j) {
        int b = a;
        a += V;
        src[0] = clip_pixel((b    ) >> 5);
        src[1] = clip_pixel((b+  H) >> 5);
        src[2] = clip_pixel((b+2*H) >> 5);
        src[3] = clip_pixel((b+3*H) >> 5);
        src[4] = clip_pixel((b+4*H) >> 5);
        src[5] = clip_pixel((b+5*H) >> 5);
        src[6] = clip_pixel((b+6*H) >> 5);
        src[7] = clip_pixel((b+7*H) >> 5);
        src += stride;
    }
}

void pred8x8_left_dc_8(uint8_t *src, int stride)
{
    int i;
    int dc0, dc2;
    pixel4 dc0splat, dc2splat;

    dc0=dc2=0;
    for(i=0;i<4; i++){
        dc0+= src[i*stride-1];
        dc2+= src[(i+4)*stride-1];
    }
    dc0splat = PIXEL_SPLAT_X4((dc0 + 2)>>2);
    dc2splat = PIXEL_SPLAT_X4((dc2 + 2)>>2);

    for(i=0; i<4; i++){
        AV_WN4PA(src + i*stride, dc0splat);
        AV_WN4PA(src + i*stride + 4, dc0splat);
    }
    for(i=4; i<8; i++){
        AV_WN4PA(src+i*stride, dc2splat);
        AV_WN4PA(src+i*stride + 4, dc2splat);
    }
}

void pred8x8_top_dc_8(uint8_t *src, int stride)
{
    int i;
    int dc0, dc1;
    uint32_t dc0splat, dc1splat;

    dc0=dc1=0;
    for(i=0;i<4; i++){
        dc0+= src[i-stride];
        dc1+= src[4+i-stride];
    }
    dc0splat = PIXEL_SPLAT_X4((dc0 + 2)>>2);
    dc1splat = PIXEL_SPLAT_X4((dc1 + 2)>>2);

    for(i=0; i<4; i++){
        AV_WN4PA(src+i*stride, dc0splat);
        AV_WN4PA(src+i*stride+4, dc1splat);
    }
    for(i=4; i<8; i++){
        AV_WN4PA(src+i*stride, dc0splat);
        AV_WN4PA(src+i*stride+4, dc1splat);
    }
}

// TODO: Add remaining chroma prediction methods here

void pred8x8_chroma(int chroma_pred_mode, uint8_t *src, int linesize)
{
    switch (chroma_pred_mode)
    {
        case DC_PRED8x8:
            pred8x8_dc_8(src, linesize);
            break;
        case HOR_PRED8x8:
            pred8x8_horizontal_8(src, linesize);
            break;
        case VERT_PRED8x8:
            pred8x8_vertical_8(src, linesize);
            break;
        case PLANE_PRED8x8:
            pred8x8_plane_8(src, linesize);
            break;
        case LEFT_DC_PRED8x8:
            pred8x8_left_dc_8(src, linesize);
            break;
        case TOP_DC_PRED8x8:
            pred8x8_top_dc(src, linesize);
            break;
        default:
            // TODO: ADD remaining chroma predictions
    }
}

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

void pred4x4_vertical_8(uint8_t *src, const uint8_t *topright, int stride)
{
    uint32_t a = AV_RN4PA(src-stride);

    AV_WN4PA(src+0*stride, a);
    AV_WN4PA(src+1*stride, a);
    AV_WN4PA(src+2*stride, a);
    AV_WN4PA(src+3*stride, a);
}

void pred4x4_horizontal_8(uint8_t *src, const uint8_t *topright, int stride)
{
    AV_WN4PA(src+0*stride, PIXEL_SPLAT_X4(src[0*stride-1]));
    AV_WN4PA(src+1*stride, PIXEL_SPLAT_X4(src[1*stride-1]));
    AV_WN4PA(src+2*stride, PIXEL_SPLAT_X4(src[2*stride-1]));
    AV_WN4PA(src+3*stride, PIXEL_SPLAT_X4(src[3*stride-1]));
}

void pred4x4_dc_8(uint8_t *src, const uint8_t *topright, int stride)
{
    int dc= (  src[-stride] + src[1-stride] + src[2-stride] + src[3-stride]
                     + src[0*stride-1] + src[1*stride-1] + src[2*stride-1] + src[3*stride-1] + 4) >>3;
    uint32_t a = PIXEL_SPLAT_X4(dc);

    AV_WN4PA(src+0*stride, a);
    AV_WN4PA(src+1*stride, a);
    AV_WN4PA(src+2*stride, a);
    AV_WN4PA(src+3*stride, a);
}

// TODO: Add mission predictions here

void pred4x4(int direction, uint8_t *src, uint8_t  *topright, int linesize)
{
    switch (direction)
    {
        case VERT_PRED:
            pred4x4_vertical_8(src, topright, linesize);
            break;
        case HOR_PRED:
            pred4x4_horizontal_8(src, topright, linesize);
            break;
        case DC_PRED:
            pred4x4_dc_8(src, topright, linesize);
            break;
        default:
            // TODO: Add missing predictions here
    }
}

void pred8x8l(int direction, uint8_t *src, int topleft, int topright, int linesize)
{
    // TODO: Add missing predictions here
}


#define DC_PRED8x8             0
#define HOR_PRED8x8            1
#define VERT_PRED8x8           2
#define PLANE_PRED8x8          3

void pred16x16(int prediction_mode, uint8_t *src, int linesize)
{
    // TODO: Add prediction modes
}

#endif // H264_MACROBLOCK_PREDICTION_H
