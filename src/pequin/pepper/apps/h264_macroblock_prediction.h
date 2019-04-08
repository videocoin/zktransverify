//
// Created by taras on 04.04.19.
//

#ifndef H264_MACROBLOCK_PREDICTION_H
#define H264_MACROBLOCK_PREDICTION_H

#ifdef CHROMA_PRED

// Chroma prediciton modes
#define DC_PRED8x8             0
#define HOR_PRED8x8            1
#define VERT_PRED8x8           2
#define PLANE_PRED8x8          3


void pred8x8_dc(uint8_t *src, int stride)
{
    int i;
    unsigned int dc0, dc1, dc2;
    pixel4 dc0splat, dc1splat, dc2splat, dc3splat;
    uint8_t *p;

    dc0=dc1=dc2=0;
    for(i=0;i<4; i++){
        p = src + i*stride-1;
        dc0 += p[0];

        p = src + i-stride;
        dc0 += p[0];

        p = src + 4+i-stride;
        dc1+= p[0];

        p = src + (i+4)*stride-1;
        dc2 += p[0];
    }

    dc0splat = PIXEL_SPLAT_X4((dc0 + 4)>>3);
    dc1splat = PIXEL_SPLAT_X4((dc1 + 2)>>2);
    dc2splat = PIXEL_SPLAT_X4((dc2 + 2)>>2);
    dc3splat = PIXEL_SPLAT_X4((dc1 + dc2 + 4)>>3);

    for(i=0; i<4; i++){
        uint32_to_bytes(src + i*stride, dc0splat);
        uint32_to_bytes(src + i*stride + 4, dc1splat);
    }

    for(i=4; i<8; i++){
        uint32_to_bytes(src + i*stride, dc2splat);
        uint32_to_bytes(src + i*stride + 4, dc3splat);
    }
}

void pred8x8_horizontal(uint8_t *src, int stride)
{
    int i;
    uint8_t *p;

    for(i=0; i<8; i++){
        p = src + i*stride-1;
        pixel4 a = PIXEL_SPLAT_X4(p[0]);

        uint32_to_bytes(src + i*stride, a);
        uint32_to_bytes(src + i*stride + 4, a);
    }
}

void pred8x8_vertical(uint8_t *src, int stride)
{
    int i;
    pixel4 a= bytes_to_uint32(src-stride+0);
    pixel4 b= bytes_to_uint32(src-stride+4);

    for(i=0; i<8; i++){
        uint32_to_bytes(src+i*stride+0, a);
        uint32_to_bytes(src+i*stride+1, b);
    }
}

void pred8x8_plane(uint8_t *src, int stride)
{
    int j, k;
    int a;

    pixel * src0 = src +3-stride;
    pixel * src1 = src +4*stride-1;
    pixel * src2 = src1-2*stride;    // == src+2*stride-1;
    int H = src0[1] + src0[-1] * (-1);
    int V = src1[0] + src2[ 0] * (-1);
    for(k=2; k<=4; ++k) {
        src1 += stride; src2 -= stride;
        H += k*(src0[k] + src0[-k] * (-1));
        V += k*(src1[0] + src2[ 0] * (-1));
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

void pred8x8_chroma(int chroma_pred_mode, uint8_t src[MB_CR_IN_LENGTH], int linesize)
{
    if (chroma_pred_mode == DC_PRED8x8)
        pred8x8_dc(src, linesize);
    else if (chroma_pred_mode == HOR_PRED8x8)
        pred8x8_horizontal(src, linesize);
    else if (chroma_pred_mode == VERT_PRED8x8)
        pred8x8_vertical(src, linesize);
    else if (chroma_pred_mode == PLANE_PRED8x8)
        pred8x8_plane(src, linesize);
}

#endif // CHROMA_PRED

#ifdef PRED4x4
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

void pred4x4_vertical(uint8_t *src, uint8_t *topright, int stride)
{
    pixel4 a = bytes_to_uint32(src-stride);

    uint32_to_bytes(src+0*stride, a);
    uint32_to_bytes(src+1*stride, a);
    uint32_to_bytes(src+2*stride, a);
    uint32_to_bytes(src+3*stride, a);
}

void pred4x4_horizontal(uint8_t *src, uint8_t *topright, int stride)
{
    uint32_to_bytes(src+0*stride, PIXEL_SPLAT_X4(src[0*stride-1]));
    uint32_to_bytes(src+1*stride, PIXEL_SPLAT_X4(src[1*stride-1]));
    uint32_to_bytes(src+2*stride, PIXEL_SPLAT_X4(src[2*stride-1]));
    uint32_to_bytes(src+3*stride, PIXEL_SPLAT_X4(src[3*stride-1]));
}

void pred4x4_dc(uint8_t *src, uint8_t *topright, int stride)
{
    int dc= (  src[-stride] + src[1-stride] + src[2-stride] + src[3-stride]
                     + src[0*stride-1] + src[1*stride-1] + src[2*stride-1] + src[3*stride-1] + 4) >>3;
    pixel4 a = PIXEL_SPLAT_X4(dc);

    uint32_to_bytes(src+0*stride, a);
    uint32_to_bytes(src+1*stride, a);
    uint32_to_bytes(src+2*stride, a);
    uint32_to_bytes(src+3*stride, a);
}

#define LOAD_TOP_RIGHT_EDGE\
    unsigned int t4 = topright[0];\
    unsigned int t5 = topright[1];\
    unsigned int t6 = topright[2];\
    unsigned int t7 = topright[3];\

#define LOAD_DOWN_LEFT_EDGE\
    unsigned int l4 = src[4*stride-1];\
    unsigned int l5 = src[5*stride-1];\
    unsigned int l6 = src[6*stride-1];\
    unsigned int l7 = src[7*stride-1];\

#define LOAD_LEFT_EDGE\
    unsigned int l0 = src[0*stride-1];\
    unsigned int l1 = src[1*stride-1];\
    unsigned int l2 = src[2*stride-1];\
    unsigned int l3 = src[3*stride-1];\

#define LOAD_TOP_EDGE\
    unsigned int t0 = src[ 0-1*stride];\
    unsigned int t1 = src[ 1-1*stride];\
    unsigned int t2 = src[ 2-1*stride];\
    unsigned int t3 = src[ 3-1*stride];\

void pred4x4_down_left(uint8_t *src, uint8_t *topright, int stride)
{
    LOAD_TOP_EDGE
    LOAD_TOP_RIGHT_EDGE
//    LOAD_LEFT_EDGE

    src[0+0*stride]=(t0 + t2 + 2*t1 + 2)>>2;
    src[1+0*stride]=
    src[0+1*stride]=(t1 + t3 + 2*t2 + 2)>>2;
    src[2+0*stride]=
    src[1+1*stride]=
    src[0+2*stride]=(t2 + t4 + 2*t3 + 2)>>2;
    src[3+0*stride]=
    src[2+1*stride]=
    src[1+2*stride]=
    src[0+3*stride]=(t3 + t5 + 2*t4 + 2)>>2;
    src[3+1*stride]=
    src[2+2*stride]=
    src[1+3*stride]=(t4 + t6 + 2*t5 + 2)>>2;
    src[3+2*stride]=
    src[2+3*stride]=(t5 + t7 + 2*t6 + 2)>>2;
    src[3+3*stride]=(t6 + 3*t7 + 2)>>2;
}

void pred4x4_down_right(uint8_t *src, uint8_t *topright, int stride)
{
    int lt= src[-1-1*stride];
    LOAD_TOP_EDGE
    LOAD_LEFT_EDGE

    src[0+3*stride]=(l3 + 2*l2 + l1 + 2)>>2;
    src[0+2*stride]=
    src[1+3*stride]=(l2 + 2*l1 + l0 + 2)>>2;
    src[0+1*stride]=
    src[1+2*stride]=
    src[2+3*stride]=(l1 + 2*l0 + lt + 2)>>2;
    src[0+0*stride]=
    src[1+1*stride]=
    src[2+2*stride]=
    src[3+3*stride]=(l0 + 2*lt + t0 + 2)>>2;
    src[1+0*stride]=
    src[2+1*stride]=
    src[3+2*stride]=(lt + 2*t0 + t1 + 2)>>2;
    src[2+0*stride]=
    src[3+1*stride]=(t0 + 2*t1 + t2 + 2)>>2;
    src[3+0*stride]=(t1 + 2*t2 + t3 + 2)>>2;
}

void pred4x4_vertical_right(uint8_t *src, uint8_t *topright, int stride)
{
    int lt= src[-1-1*stride];
    LOAD_TOP_EDGE
    LOAD_LEFT_EDGE

    src[0+0*stride]=
    src[1+2*stride]=(lt + t0 + 1)>>1;
    src[1+0*stride]=
    src[2+2*stride]=(t0 + t1 + 1)>>1;
    src[2+0*stride]=
    src[3+2*stride]=(t1 + t2 + 1)>>1;
    src[3+0*stride]=(t2 + t3 + 1)>>1;
    src[0+1*stride]=
    src[1+3*stride]=(l0 + 2*lt + t0 + 2)>>2;
    src[1+1*stride]=
    src[2+3*stride]=(lt + 2*t0 + t1 + 2)>>2;
    src[2+1*stride]=
    src[3+3*stride]=(t0 + 2*t1 + t2 + 2)>>2;
    src[3+1*stride]=(t1 + 2*t2 + t3 + 2)>>2;
    src[0+2*stride]=(lt + 2*l0 + l1 + 2)>>2;
    src[0+3*stride]=(l0 + 2*l1 + l2 + 2)>>2;
}

void pred4x4_vertical_left(uint8_t *src, uint8_t *topright, int stride)
{
    LOAD_TOP_EDGE
    LOAD_TOP_RIGHT_EDGE

    src[0+0*stride]=(t0 + t1 + 1)>>1;
    src[1+0*stride]=
    src[0+2*stride]=(t1 + t2 + 1)>>1;
    src[2+0*stride]=
    src[1+2*stride]=(t2 + t3 + 1)>>1;
    src[3+0*stride]=
    src[2+2*stride]=(t3 + t4+ 1)>>1;
    src[3+2*stride]=(t4 + t5+ 1)>>1;
    src[0+1*stride]=(t0 + 2*t1 + t2 + 2)>>2;
    src[1+1*stride]=
    src[0+3*stride]=(t1 + 2*t2 + t3 + 2)>>2;
    src[2+1*stride]=
    src[1+3*stride]=(t2 + 2*t3 + t4 + 2)>>2;
    src[3+1*stride]=
    src[2+3*stride]=(t3 + 2*t4 + t5 + 2)>>2;
    src[3+3*stride]=(t4 + 2*t5 + t6 + 2)>>2;
}

void pred4x4_horizontal_up(uint8_t *src, uint8_t *topright, int stride)
{
    LOAD_LEFT_EDGE

    src[0+0*stride]=(l0 + l1 + 1)>>1;
    src[1+0*stride]=(l0 + 2*l1 + l2 + 2)>>2;
    src[2+0*stride]=
    src[0+1*stride]=(l1 + l2 + 1)>>1;
    src[3+0*stride]=
    src[1+1*stride]=(l1 + 2*l2 + l3 + 2)>>2;
    src[2+1*stride]=
    src[0+2*stride]=(l2 + l3 + 1)>>1;
    src[3+1*stride]=
    src[1+2*stride]=(l2 + 2*l3 + l3 + 2)>>2;
    src[3+2*stride]=
    src[1+3*stride]=
    src[0+3*stride]=
    src[2+2*stride]=
    src[2+3*stride]=
    src[3+3*stride]=l3;
}

void pred4x4_horizontal_down(uint8_t *src, uint8_t *topright, int stride)
{
    int lt= src[-1-1*stride];
    LOAD_TOP_EDGE
    LOAD_LEFT_EDGE

    src[0+0*stride]=
    src[2+1*stride]=(lt + l0 + 1)>>1;
    src[1+0*stride]=
    src[3+1*stride]=(l0 + 2*lt + t0 + 2)>>2;
    src[2+0*stride]=(lt + 2*t0 + t1 + 2)>>2;
    src[3+0*stride]=(t0 + 2*t1 + t2 + 2)>>2;
    src[0+1*stride]=
    src[2+2*stride]=(l0 + l1 + 1)>>1;
    src[1+1*stride]=
    src[3+2*stride]=(lt + 2*l0 + l1 + 2)>>2;
    src[0+2*stride]=
    src[2+3*stride]=(l1 + l2+ 1)>>1;
    src[1+2*stride]=
    src[3+3*stride]=(l0 + 2*l1 + l2 + 2)>>2;
    src[0+3*stride]=(l2 + l3 + 1)>>1;
    src[1+3*stride]=(l1 + 2*l2 + l3 + 2)>>2;
}

void pred4x4(int direction, uint8_t *src, uint8_t  *topright, int linesize)
{
    if (direction == VERT_PRED)
        pred4x4_vertical(src, topright, linesize);
    else if (direction == HOR_PRED)
        pred4x4_horizontal(src, topright, linesize);
    else if (direction == DC_PRED)
        pred4x4_dc(src, topright, linesize);
    else if (direction == DIAG_DOWN_LEFT_PRED)
        pred4x4_down_left(src, topright, linesize);
    else if (direction == DIAG_DOWN_RIGHT_PRED)
        pred4x4_down_right(src, topright, linesize);
    else if (direction == VERT_RIGHT_PRED)
        pred4x4_vertical_right(src, topright, linesize);
    else if (direction == HOR_DOWN_PRED)
        pred4x4_horizontal_down(src, topright, linesize);
    else if (direction == VERT_LEFT_PRED)
        pred4x4_vertical_left(src, topright, linesize);
    else if (direction == HOR_UP_PRED)
        pred4x4_horizontal_up(src, topright, linesize);
}

#endif // PRED4x4

#ifdef PRED8x8DCT
void pred8x8l(int direction, uint8_t *src, int topleft, int topright, int linesize)
{
    // TODO: Add missing predictions here
}
#endif

#ifdef PRED16x16

#define VERT_PRED16x16      0
#define HOR_PRED16x16       1
#define DC_PRED16x16        2
#define PLANE_PRED16x16     3

void pred16x16_vertical(uint8_t *src, int stride)
{
    int i;
    pixel4 a = bytes_to_uint32(src-stride+0);
    pixel4 b = bytes_to_uint32(src-stride+4);
    pixel4 c = bytes_to_uint32(src-stride+8);
    pixel4 d = bytes_to_uint32(src-stride+12);

    for(i=0; i<16; i++){
        uint32_to_bytes(src+i*stride+0, a);
        uint32_to_bytes(src+i*stride+4, b);
        uint32_to_bytes(src+i*stride+8, c);
        uint32_to_bytes(src+i*stride+12, d);
    }
}

void pred16x16_horizontal(uint8_t *src, int stride)
{
    int i;

    for(i=0; i<16; i++){
        pixel4 a = PIXEL_SPLAT_X4(src[i*stride-1]);

        uint32_to_bytes(src+i*stride+0, a);
        uint32_to_bytes(src+i*stride+4, a);
        uint32_to_bytes(src+i*stride+8, a);
        uint32_to_bytes(src+i*stride+12, a);
    }
}

void pred16x16_dc(uint8_t *src, int stride)
{
    int i, dc=0;
    pixel4 dcsplat;

    for(i=0;i<16; i++){
        dc+= src[i*stride-1];
    }

    for(i=0;i<16; i++){
        dc+= src[i-stride];
    }

    dcsplat = PIXEL_SPLAT_X4((dc+16)>>5);
    for(i=0; i<16; i++){
        uint32_to_bytes(src+ 0, v);
        uint32_to_bytes(src+ 4, v);
        uint32_to_bytes(src+ 8, v);
        uint32_to_bytes(src+12, v);
        src += stride;
    }
}

void pred16x16_plane(uint8_t *src, int stride)
{
    int i, j, k;
    int a;

    pixel * src0 = src +7-stride;
    pixel *       src1 = src +8*stride-1;
    pixel *       src2 = src1-2*stride;    // == src+6*stride-1;
    int H = src0[1] - src0[-1];
    int V = src1[0] - src2[ 0];
    for(k=2; k<=8; ++k) {
        src1 += stride; src2 -= stride;
        H += k*(src0[k] - src0[-k]);
        V += k*(src1[0] - src2[ 0]);
    }

    H = ( 5*H+32 ) >> 6;
    V = ( 5*V+32 ) >> 6;

    a = 16*(src1[0] + src2[16] + 1) - 7*(V+H);
    for(j=16; j>0; --j) {
        int b = a;
        a += V;
        for(i=-16; i<0; i+=4) {
            src[16+i] = clip_pixel((b    ) >> 5);
            src[17+i] = clip_pixel((b+  H) >> 5);
            src[18+i] = clip_pixel((b+2*H) >> 5);
            src[19+i] = clip_pixel((b+3*H) >> 5);
            b += 4*H;
        }
        src += stride;
    }
}

void pred16x16(int prediction_mode, uint8_t *src, int linesize)
{
    switch (prediction_mode)
    {
        case VERT_PRED16x16:
            pred16x16_vertical(src, linesize);
            break;
        case HOR_PRED16x16:
            pred16x16_horizontal(src, linesize);
            break;
        case DC_PRED16x16:
            pred16x16_dc(src, linesize);
            break;
        case PLANE_PRED16x16:
            pred16x16_plane(src, linesize);
            break;
    }
}

#endif // PRED16x16

#endif // H264_MACROBLOCK_PREDICTION_H
