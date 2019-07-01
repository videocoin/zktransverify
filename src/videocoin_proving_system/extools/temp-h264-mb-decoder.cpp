//
// Created by taras on 05.06.19.
//

#include "temp-h264-mb-decoder.h"
#include <stdio.h>
#include <memory.h>

// awkward situation. h264 uses PRED8x8 numeration and dont have definition for PRED16x16
#define DC_PRED16x16        0
#define HOR_PRED16x16       1
#define VERT_PRED16x16      2
#define PLANE_PRED16x16     3
#define DC_LEFT_PRED16x16   4
#define DC_TOP_PRED16x16    5
#define DC_128_PRED16x16    6

// This table must be here because scan8[constant] must be known at compiletime
uint8_t scan8[16 * 3 + 3] = {
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

int block_offset[16] = {
        0, 4,
        64, 68,
        8, 12,
        72, 76,
        128, 132,
        192, 196,
        136, 140,
        200, 204
};

void print_coeff(int16_t *coeff) {
    int i;
    for (i = 0; i < 16 * 16; ++i) {
        printf("PRINTF in computation_p 1:\n");
        printf("\"%d\" \n", coeff[i]);
    }
}

uint32_t PIXEL_SPLAT_X4(uint32_t x)
{
    return x * 0x01010101U;
}

uint32_t u8arr_to_u32(uint8_t *src) {
    uint32_t v;
    v = src[0];
    v |= src[1] << 8;
    v |= src[2] << 16;
    v |= src[3] << 24;
    return v;
}

void u32_to_u8arr(uint8_t *src, uint32_t v) {
    src[0] = v & 0xFF;
    src[1] = (v >> 8) & 0xFF;
    src[2] = (v >> 16) & 0xFF;
    src[3] = (v >> 24) & 0xFF;
}

void u32_to_u8arr2(uint8_t *src, uint32_t v, int stride) {
    src[0*stride] = v & 0xFF;
    src[1*stride] = (v >> 8) & 0xFF;
    src[2*stride] = (v >> 16) & 0xFF;
    src[3*stride] = (v >> 24) & 0xFF;
}

uint8_t clip_pixel(int a) {
    int v = a;
    if (v > 255)
        v = 255;
    else if (v < 0)
        v = 0;

    return v;
}

/*
 * Tested on crowd_run_2160p50_40M.ts
 * [macro block decode] prediction type: 2
 * [macro block decode] x: 239  y: 122  xy: 29641
 */
void pred16x16_vertical(uint8_t *top, uint8_t *res)
{
    int i, stride = 16;
    // read luma top row
    uint32_t a = u8arr_to_u32(top + 0);
    uint32_t b = u8arr_to_u32(top + 4);
    uint32_t c = u8arr_to_u32(top + 8);
    uint32_t d = u8arr_to_u32(top + 12);
    // propagate top row to all rows
    for (i=0; i<16; i++) {
        u32_to_u8arr(res + i * stride + 0, a);
        u32_to_u8arr(res + i * stride + 4, b);
        u32_to_u8arr(res + i * stride + 8, c);
        u32_to_u8arr(res + i * stride + 12, d);
    }
}

/*
 * Tested on crowd_run_2160p50_40M.ts
 * [macro block decode] prediction type: 1
 * [macro block decode] x: 144  y: 133  xy: 32197
 */
void pred16x16_horizontal(uint8_t *left, uint8_t *res)
{
#define stride 16
    int i;
    // read column on the left
    uint32_t a = u8arr_to_u32(left + 0);
    uint32_t b = u8arr_to_u32(left + 4);
    uint32_t c = u8arr_to_u32(left + 8);
    uint32_t d = u8arr_to_u32(left + 12);
    // copy column on the left to other columns
    for (i=0; i<16; ++i) {
        u32_to_u8arr2(res + 0 * stride + i, a, stride);
        u32_to_u8arr2(res + 4 * stride + i, b, stride);
        u32_to_u8arr2(res + 8 * stride + i, c, stride);
        u32_to_u8arr2(res + 12 * stride + i, d, stride);
    }
#undef stride
}

/*
 * Tested on crowd_run_2160p50_40M.ts
 * [macro block decode] prediction type: 0
 * [macro block decode] x: 073  y: 020  xy: 4893
 */
void pred16x16_dc(uint8_t *left, uint8_t *top, uint8_t *res)
{
#define stride 16
    int i, dc=0;
    uint32_t dcsplat;

    for(i=0;i<16; i++){
        dc+= left[i];
        dc+= top[i];
    }

    dcsplat = PIXEL_SPLAT_X4((dc+16)>>5);
    for (i=0; i<16; ++i) {
        u32_to_u8arr2(res + 0 * stride + i, dcsplat, stride);
        u32_to_u8arr2(res + 4 * stride + i, dcsplat, stride);
        u32_to_u8arr2(res + 8 * stride + i, dcsplat, stride);
        u32_to_u8arr2(res + 12 * stride + i, dcsplat, stride);
    }
#undef stride
}

/*
 * Tested on crowd_run_2160p50_40M.ts
 * [macro block decode] prediction type: 3
 * [macro block decode] x: 137  y: 134  xy: 32431
 */
void pred16x16_plane(uint8_t *left, uint8_t *top, uint8_t left_top, uint8_t *res)
{
#define stride 16
    int a, b, c, H, V;
    int x, y;

    H = 8 * (top[15] - left_top);
    V = 8 * (left[15] - left_top);
    for (x = 0; x < 7; ++x) {
        H += (x + 1)*(top[8+x] - top[6-x]);
        V += (x + 1)*(left[8+x] - left[6-x]);
    }

    a = 16 * (left[15] + top[15]);
    b = (5 * H + 32) >> 6;
    c = (5 * V + 32) >> 6;

    for (x = 0; x < 16; ++x) {
        for (y = 0; y < 16; ++y) {
            res[x + y*stride] = clip_pixel((a + b * (x - 7) + c * (y - 7) + 16)>>5);
        }
    }
#undef stride
}

void pred16x16_left_dc(uint8_t *left, uint8_t *res)
{
#define stride 16
    int i, dc = 0;
    uint32_t dcsplat;
    for (i = 0; i < 16; ++i) {
        dc += left[i];
    }

    dcsplat = PIXEL_SPLAT_X4((dc+8)>>4);
    for (i=0; i<16; ++i) {
        u32_to_u8arr2(res + 0 * stride + i, dcsplat, stride);
        u32_to_u8arr2(res + 4 * stride + i, dcsplat, stride);
        u32_to_u8arr2(res + 8 * stride + i, dcsplat, stride);
        u32_to_u8arr2(res + 12 * stride + i, dcsplat, stride);
    }
#undef stride
}

/*
 * Tested on crowd_run_2160p50_40M.ts
 * [macro block decode] prediction type: 5
 * [macro block decode] x: 000  y: 110  xy: 26510
 */
void pred16x16_top_dc(uint8_t *src, uint8_t *res)
{
#define stride 16
    int i, dc = 0;
    uint32_t dcsplat;
    for (i = 0; i < 16; ++i) {
        dc += src[i];
    }

    dcsplat = PIXEL_SPLAT_X4((dc+8)>>4);
    for (i=0; i<16; ++i) {
        u32_to_u8arr(res + i * stride + 0, dcsplat);
        u32_to_u8arr(res + i * stride + 4, dcsplat);
        u32_to_u8arr(res + i * stride + 8, dcsplat);
        u32_to_u8arr(res + i * stride + 12, dcsplat);
    }
#undef stride
}

void pred16x16_128_dc(uint8_t *res)
{
#define stride 16
    int i;
    uint32_t dcsplat = 0x80808080;

    for (i=0; i<16; ++i) {
        u32_to_u8arr2(res + 0 * stride + i, dcsplat, stride);
        u32_to_u8arr2(res + 4 * stride + i, dcsplat, stride);
        u32_to_u8arr2(res + 8 * stride + i, dcsplat, stride);
        u32_to_u8arr2(res + 12 * stride + i, dcsplat, stride);
    }
#undef stride
}

void pred16x16(struct In *in, uint8_t *res)
{
    int prediction_mode = in->intra16x16_pred_mode;
    uint8_t *left = in->luma_left;
    uint8_t *top = in->luma_top + 8;
    uint8_t top_m1 = in->luma_top[7];

    if (prediction_mode == PLANE_PRED16x16)
        pred16x16_plane(left, top, top_m1, res);

    if (prediction_mode == VERT_PRED16x16)
        pred16x16_vertical(top, res);

    if (prediction_mode == HOR_PRED16x16)
        pred16x16_horizontal(left, res);

    if (prediction_mode == DC_PRED16x16)
        pred16x16_dc(left, top, res);

    if (prediction_mode == DC_LEFT_PRED16x16)
        pred16x16_left_dc(left, res);

    if (prediction_mode == DC_TOP_PRED16x16)
        pred16x16_top_dc(top, res);

    if (prediction_mode == DC_128_PRED16x16)
        pred16x16_128_dc(res);
}

void XCHG(uint8_t a[8], uint8_t b[8])
{
    int i;
    uint8_t temp[8];

    for (i = 0; i < 8; ++i) {
        temp[i] = a[i];
    }

    for (i = 0; i < 8; ++i) {
        a[i] = b[i];
    }

    for (i = 0; i < 8; ++i) {
        b[i] = temp[i];
    }
}

void xchg_mb_border(struct In *in)
{
    bool deblock_topleft = (bool)(in->mb_x > 0);
    bool deblock_top = (bool)(in->mb_y > (in->mb_field_decoding_flag != 0));
    uint8_t *top_border_m1 = in->top_border;
    uint8_t *top_border = in->top_border + 8;
    uint8_t *top_border_p1 = in->top_border + 8 + 16;

    uint8_t *src_y  = in->luma_top + 7;
    if (deblock_top) {
        if (deblock_topleft) {
            XCHG(top_border_m1, src_y - 7);
        }
        XCHG(top_border + 0, src_y + 1);
        XCHG(top_border + 8, src_y + 9);

        if (in->mb_x + 1 < in->mb_width) {
            XCHG(top_border_p1, src_y + 17);
        }
    }
}

void luma_dc_dequant_idct(int16_t *output, int16_t *input, int qmul){
#define stride 16
    int i;
    int temp[16];
    uint8_t x_offset[4]={0, 2*stride, 8*stride, 10*stride};

    for(i=0; i<4; i++){
        int z0= input[4*i+0] + input[4*i+1];
        int z1= input[4*i+0] + input[4*i+1] * (-1);
        int z2= input[4*i+2] + input[4*i+3] * (-1);
        int z3= input[4*i+2] + input[4*i+3];

        temp[4*i+0]= z0+z3;
        temp[4*i+1]= z0-z3;
        temp[4*i+2]= z1-z2;
        temp[4*i+3]= z1+z2;
    }

    for(i=0; i<4; i++){
        int offset = x_offset[i];
        int z0= temp[4*0+i] + temp[4*2+i];
        int z1= temp[4*0+i] + temp[4*2+i] * (-1);
        int z2= temp[4*1+i] + temp[4*3+i] * (-1);
        int z3= temp[4*1+i] + temp[4*3+i];

        output[stride* 0+offset]= (int)((z0 + z3)*qmul + 128 ) >> 8;
        output[stride* 1+offset]= (int)((z1 + z2)*qmul + 128 ) >> 8;
        output[stride* 4+offset]= (int)((z1 - z2)*qmul + 128 ) >> 8;
        output[stride* 5+offset]= (int)((z0 - z3)*qmul + 128 ) >> 8;
    }
#undef stride
}

void h264_idct_dc_add(uint8_t *dst, int16_t *block, int stride)
{
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

void h264_idct_add(uint8_t *dst, int16_t *block, int stride)
{
    int i;

    block[0] += 32;

    for(i=0; i<4; i++){
        int z0=  block[i + 4*0]     +  block[i + 4*2];
        int z1=  block[i + 4*0]     +  block[i + 4*2] * (-1);
        int z2 = block[i + 4*1]; z2 = z2 >> 1; z2 += block[i + 4*3] * (-1);
        int z3=  block[i + 4*3]; z3 = z3 >> 1; z3 += block[i + 4*1];

        block[i + 4*0]= z0 + z3;
        block[i + 4*1]= z1 + z2;
        block[i + 4*2]= z1 - z2;
        block[i + 4*3]= z0 - z3;
    }

    for(i=0; i<4; i++){
        int z0=  block[0 + 4*i]     +  block[2 + 4*i];
        int z1=  block[0 + 4*i]     -  block[2 + 4*i];
        int z2= (block[1 + 4*i]>>1) -  block[3 + 4*i];
        int z3=  block[1 + 4*i]     + (block[3 + 4*i]>>1);

        dst[i + 0*stride]= clip_pixel(dst[i + 0*stride] + ((int)(z0 + z3) >> 6));
        dst[i + 1*stride]= clip_pixel(dst[i + 1*stride] + ((int)(z1 + z2) >> 6));
        dst[i + 2*stride]= clip_pixel(dst[i + 2*stride] + ((int)(z1 - z2) >> 6));
        dst[i + 3*stride]= clip_pixel(dst[i + 3*stride] + ((int)(z0 - z3) >> 6));
    }
}

void h264_idct_add16intra(uint8_t *dst, int16_t *block, uint8_t nnzc[15*8]){
    int stride = 16;
    int i;
    for (i=0; i<16; i++) {
        bool use_idct_add = nnzc[ scan8[i] ];
        bool use_idct_dc_add = block[i*16];

        if (use_idct_add) h264_idct_add(dst + block_offset[i], block + i*16, stride);
        if (!use_idct_add && use_idct_dc_add) h264_idct_dc_add(dst + block_offset[i], block + i*16, stride);
    }
}

#define DUMP_CHANGE(format, a, b) \
    int changed = (a) != (b); \
    if (changed) printf("\033[0;34m"); \
    printf(format, (a)); \
    if (changed) { \
        printf("\033[0m"); \
        (b) = (a); \
    }

void dump_mb(In *in, uint8_t *mb, int reset_cache) {

    static uint8_t luma_m1_cache[16];
    static uint8_t luma_cache[16*16];
    static uint8_t top_border_cache[16*2];
    static uint8_t top_cache[16*2];
    static uint8_t left_cache[16];

    if (reset_cache) {
        memset(luma_m1_cache, 0, sizeof(luma_m1_cache));
        memset(luma_cache, 0, sizeof(luma_cache));
        memset(top_border_cache, 0, sizeof(top_border_cache));
        memset(top_cache, 0, sizeof(top_cache));
        memset(left_cache, 0, sizeof(left_cache));
    }

    printf("[proof generator] prediction type: %d\n", in->intra16x16_pred_mode);
    printf("[proof generator] x: %d y: %d xy: %d\n", in->mb_x, in->mb_y, in->mb_xy);
    {
        for (int i = 0; i < 8; ++i) {
            DUMP_CHANGE("%02x ", in->top_border[i], top_border_cache[i])
        }
        printf(" ");
        for (int i = 8; i < 16+8+8; ++i) {
            DUMP_CHANGE("%02x ", in->top_border[i], top_border_cache[i])
        }
        printf("\n");
        // print top
        uint8_t *top = in->luma_top;

        for (int i = 0; i < 8; ++i) {
            DUMP_CHANGE("%02x ", top[i], top_cache[i])
        }
        printf(" ");

        for (int x = 8; x < 16+8+8; ++x) {
            DUMP_CHANGE("%02x ", top[x], top_cache[x])
        }
        printf("\n");
        printf("\n");
    }

    for (int y = 0; y < 16; ++y) {
        for (int i = 0; i < 7; ++i) {
            printf("   ");
        }
        DUMP_CHANGE("%02x  ", in->luma_left[y], luma_m1_cache[y])
        for (int x = 0; x < 16; ++x) {
            DUMP_CHANGE("%02x ", mb[x], luma_cache[y + x*16])
        }
        printf("\n");
        mb += 16;
    }
}

void dump_coefficients(In *in, int reset_cache)
{
    static int16_t cache[16 * 16];
    static int16_t mb_luma_dc_cache[16];
    if (reset_cache) {
        memset(cache, 0, sizeof(cache));
        memset(mb_luma_dc_cache, 0, sizeof(mb_luma_dc_cache));
    }

    printf("[proof generator coefficients] prediction type: %d\n", in->intra16x16_pred_mode);
    printf("[proof generator coefficients] x: %03d  y: %03d  xy: %03d\n", in->mb_x, in->mb_y, in->mb_xy);
    printf("[coefficients] dequant coeff: %03d\n", in->dequant_coeff);
    printf("[coefficients] mb_luma_dc:\n");

    for (int x = 0; x < 16; ++x) {
        DUMP_CHANGE("%05d ", in->mb_luma_dc[x], mb_luma_dc_cache[x])
    }
    printf("\n\n\n");

    for (int x = 0; x < 16; ++x) {
        for (int y = 0; y < 16; ++y) {
            DUMP_CHANGE("%05d ", in->mb[y+x*16], cache[y+x*16])
        }
        printf("\n\n");
    }
}

void print_luma(uint8_t *luma) {
    int i;
    for (i = 0; i < 16; ++i)
    {
        printf("%x %x %x %x %x %x %x %x\n",
               luma[i*16], luma[i*16+1], luma[i*16+2], luma[i*16+3], luma[i*16+4], luma[i*16+5], luma[i*16+6], luma[i*16+7]);
        printf("%x %x %x %x %x %x %x %x\n",
               luma[i*16+8], luma[i*16+9], luma[i*16+10], luma[i*16+11], luma[i*16+12], luma[i*16+13], luma[i*16+14], luma[i*16+15]);
    }
}

void decode_mb(In *in, uint8_t *luma) {
    dump_mb(in, luma, 1);
    if (in->deblocking_filter) {
        xchg_mb_border(in);
        dump_mb(in, luma, 0);
    }
    pred16x16(in, luma);
    dump_mb(in, luma, 0);
    if (in->non_zero_count_cache[0]) {
        dump_coefficients(in, 1);
        luma_dc_dequant_idct(in->mb, in->mb_luma_dc, in->dequant_coeff);
        dump_coefficients(in, 0);
    }
    h264_idct_add16intra(luma, in->mb, in->non_zero_count_cache);
//    dump_coefficients(in, 0);
    print_luma(luma);
//    dump_mb(in, luma, 0);
}