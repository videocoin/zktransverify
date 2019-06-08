//
// Created by taras on 05.06.19.
//

#include "temp-h264-mb-decoder.h"
#include <assert.h>
#include <stdio.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

// awkward situation. h264 uses PRED8x8 numeration and dont have definition for PRED16x16
#define DC_PRED16x16        0
#define HOR_PRED16x16       1
#define VERT_PRED16x16      2
#define PLANE_PRED16x16     3
#define DC_LEFT_PRED16x16   4
#define DC_TOP_PRED16x16    5
#define DC_128_PRED16x16    6

uint32_t PIXEL_SPLAT_X4(uint32_t x)
{
    return x * 0x01010101U;
}

int32_t i16_a_to_i32(int16_t *src) {
    int32_t v;
    v = src[0];
    v |= src[1] << 16;
    return v;
}

void i32_to_i16_a(int16_t *dst, int32_t v) {
    dst[0] = v & 0xFFFF;
    dst[1] = v >> 16;
}

uint16_t u8_a_to_u16(uint8_t *src) {
    uint16_t v;
    v = src[0];
    v |= src[1] << 8;
    return v;
}

uint32_t u8_to_u32(uint8_t *src) {
    uint32_t v;
    v = src[0];
    v |= src[1] << 8;
    v |= src[2] << 16;
    v |= src[3] << 24;
    return v;
}

void u32_to_u8(uint8_t *src, uint32_t v) {
    src[0] = v & 0xFF;
    src[1] = (v >> 8) & 0xFF;
    src[2] = (v >> 16) & 0xFF;
    src[3] = (v >> 24) & 0xFF;
}

void u32_to_u8(uint8_t *src, uint32_t v, int stride) {
    src[0*stride] = v & 0xFF;
    src[1*stride] = (v >> 8) & 0xFF;
    src[2*stride] = (v >> 16) & 0xFF;
    src[3*stride] = (v >> 24) & 0xFF;
}

uint64_t u8_to_u64(uint8_t *src) {
    printf("[u8_to_u64]\n");
    uint64_t v;
    printf("src= ");
    for (int i = 0; i < 8; ++i) {
        printf("%02x ", src[i]);
    }
    printf("\n");
    v = src[0];
    v |= src[1] << 8;
    v |= src[2] << 16;
    v |= src[3] << 24;
    v |= (uint64_t)(src[4]) << 32;
    v |= (uint64_t)(src[5]) << 40;
    v |= (uint64_t)(src[6]) << 48;
    v |= (uint64_t)(src[7]) << 56;

    printf("v = %"PRIx64"\n", v);

    return v;
}

void u64_to_u8(uint8_t *src, uint64_t v) {
    printf("[u64_to_u8]\n");
    printf("v = %"PRIx64"\n", v);
    src[0] = v & 0xFF;
    src[1] = (v >> 8) & 0xFF;
    src[2] = (v >> 16) & 0xFF;
    src[3] = (v >> 24) & 0xFF;
    src[4] = (v >> 32) & 0xFF;
    src[5] = (v >> 40) & 0xFF;
    src[6] = (v >> 48) & 0xFF;
    src[7] = (v >> 56) & 0xFF;

    printf("src= ");
    for (int i = 0; i < 8; ++i) {
        printf("%02x ", src[i]);
    }
    printf("\n");
}

uint8_t clip_pixel(int a) {
    int v = a;
    if (a & (~0xFF))
        v = (-a) >> 31;

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
    uint32_t a = u8_to_u32(top + 0);
    uint32_t b = u8_to_u32(top + 4);
    uint32_t c = u8_to_u32(top + 8);
    uint32_t d = u8_to_u32(top + 12);
    // propagate top row to all rows
    for (i=0; i<16; i++) {
        u32_to_u8(res + i * stride + 0, a);
        u32_to_u8(res + i * stride + 4, b);
        u32_to_u8(res + i * stride + 8, c);
        u32_to_u8(res + i * stride + 12, d);
    }
}

/*
 * Tested on crowd_run_2160p50_40M.ts
 * [macro block decode] prediction type: 1
 * [macro block decode] x: 144  y: 133  xy: 32197
 */
void pred16x16_horizontal(uint8_t *left, uint8_t *res)
{
    int i, stride = 16;
    // read column on the left
    uint32_t a = u8_to_u32(left + 0);
    uint32_t b = u8_to_u32(left + 4);
    uint32_t c = u8_to_u32(left + 8);
    uint32_t d = u8_to_u32(left + 12);
    // copy column on the left to other columns
    for (i=0; i<16; ++i) {
        u32_to_u8(res + 0*stride + i, a, stride);
        u32_to_u8(res + 4*stride + i, b, stride);
        u32_to_u8(res + 8*stride + i, c, stride);
        u32_to_u8(res + 12*stride + i, d, stride);
    }
}

/*
 * Tested on crowd_run_2160p50_40M.ts
 * [macro block decode] prediction type: 0
 * [macro block decode] x: 073  y: 020  xy: 4893
 */
void pred16x16_dc(uint8_t *left, uint8_t *top, uint8_t *res)
{
    int i, dc=0, stride = 16;
    uint32_t dcsplat;

    for(i=0;i<16; i++){
        dc+= left[i];
        dc+= top[i];
    }

    dcsplat = PIXEL_SPLAT_X4((dc+16)>>5);
    for (i=0; i<16; ++i) {
        u32_to_u8(res + 0*stride + i, dcsplat, stride);
        u32_to_u8(res + 4*stride + i, dcsplat, stride);
        u32_to_u8(res + 8*stride + i, dcsplat, stride);
        u32_to_u8(res + 12*stride + i, dcsplat, stride);
    }
}

/*
 * Tested on crowd_run_2160p50_40M.ts
 * [macro block decode] prediction type: 3
 * [macro block decode] x: 137  y: 134  xy: 32431
 */
void pred16x16_plane(uint8_t *left, uint8_t *top, uint8_t left_top, uint8_t *res)
{
    int a, b, c, H, V;
    int x, y, stride;

    stride = 16;
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
}

/*
 * Tested on crowd_run_2160p50_40M.ts
 * [macro block decode] prediction type: 5
 * [macro block decode] x: 000  y: 110  xy: 26510
 */
void pred16x16_left_top_dc(uint8_t *src, uint8_t *res)
{
    int i, dc = 0, stride = 0;
    uint32_t dcsplat;
    for (i = 0; i < 16; ++i) {
        dc += src[i];
    }

    dcsplat = PIXEL_SPLAT_X4((dc+8)>>4);
    for (i=0; i<16; ++i) {
        u32_to_u8(res + 0*stride + i, dcsplat, stride);
        u32_to_u8(res + 4*stride + i, dcsplat, stride);
        u32_to_u8(res + 8*stride + i, dcsplat, stride);
        u32_to_u8(res + 12*stride + i, dcsplat, stride);
    }
}

void pred16x16_128_dc(uint8_t *res)
{
    int i, stride = 16;
    uint32_t dcsplat;
    dcsplat = PIXEL_SPLAT_X4(8);

    for (i=0; i<16; ++i) {
        u32_to_u8(res + 0*stride + i, dcsplat, stride);
        u32_to_u8(res + 4*stride + i, dcsplat, stride);
        u32_to_u8(res + 8*stride + i, dcsplat, stride);
        u32_to_u8(res + 12*stride + i, dcsplat, stride);
    }
}

void pred16x16(In *in, uint8_t *res)
{
    int prediction_mode = in->intra16x16_pred_mode;
    uint8_t *left = in->luma_left;
    uint8_t *top = in->luma_top + 8;

    if (prediction_mode == VERT_PRED16x16)
        pred16x16_vertical(top, res);
    else if (prediction_mode == HOR_PRED16x16)
        pred16x16_horizontal(left, res);
    else if (prediction_mode == DC_PRED16x16)
        pred16x16_dc(left, top, res);
    else if (prediction_mode == PLANE_PRED16x16)
        pred16x16_plane(left, top, top[-1], res);
    else if (prediction_mode == DC_LEFT_PRED16x16)
        pred16x16_left_top_dc(left, res);
    else if (prediction_mode == DC_TOP_PRED16x16)
        pred16x16_left_top_dc(top, res);
    else if (prediction_mode == DC_128_PRED16x16)
        pred16x16_128_dc(res);
    else
        assert(false);
}

void XCHG(uint8_t a[8], uint8_t b[8], int xchg)
{
    int i;
    if (xchg) {
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

    } else {
        for (i = 0; i < 8; ++i) {
            b[i] = a[i];
        }
    }
}

uint8_t top_borders[8+16+8];

void xchg_mb_border(In *in, int xchg)
{
    int deblock_topleft = (in->mb_x > 0);
    int deblock_top = (in->mb_y > (in->mb_field_decoding_flag != 0));
    uint8_t *top_border_m1 = top_borders;
    uint8_t *top_border = top_borders + 8;
    uint8_t *top_border_p1 = top_borders + 8 + 16;

    uint8_t *src_y  = in->luma_top + 7;
    if (deblock_top) {
        if (deblock_topleft) {
            XCHG(top_border_m1, src_y - 7, 1);
        }
        XCHG(top_border + 0, src_y + 1, xchg);
        XCHG(top_border + 8, src_y + 9, 1);

        if (in->mb_x + 1 < in->mb_width) {
            XCHG(top_border_p1, src_y + 17, 1);
        }
    }
}

void dump_mb(In *in, uint8_t *mb) {

    printf("[proof generator] prediction type: %d\n", in->intra16x16_pred_mode);
    printf("[proof generator] x: %d y: %d xy: %d\n", in->mb_x, in->mb_y, in->mb_xy);
    {
        // print top
        uint8_t *top = in->luma_top;

        for (int i = 0; i < 8; ++i) {
            printf("%02x ", top[i]);
        }
        printf(" ");

        for (int x = 8; x < 16+8+8; ++x) {
            printf("%02x ", top[x]);
        }
        printf("\n");
        printf("\n");
    }

    for (int y = 0; y < 16; ++y) {
        for (int i = 0; i < 7; ++i) {
            printf("   ");
        }
        printf("%02x  ", in->luma_left[y]);
        for (int x = 0; x < 16; ++x) {
            printf("%02x ", mb[x]);
        }
        printf("\n");
        mb += 16;
    }
}

void init_top_borders(In *in) {
    int i;
    for (i = 0; i < 32; ++i) {
        top_borders[i] = in->luma_top[i];
    }
}

void decode_mb(In *in, uint8_t *luma) {
    init_top_borders(in);
    dump_mb(in, luma);
    if (in->deblocking_filter) {
        xchg_mb_border(in, 1);
        dump_mb(in, luma);
    }
    pred16x16(in, luma);
    if (in->deblocking_filter) {
        dump_mb(in, luma);
        xchg_mb_border(in, 0);

    }
    dump_mb(in, luma);
}