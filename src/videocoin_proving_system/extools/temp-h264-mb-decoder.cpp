//
// Created by taras on 05.06.19.
//

#include "temp-h264-mb-decoder.h"
#include <assert.h>

#define pixel  uint8_t
#define pixel4 uint32_t

#define VERT_PRED16x16      0
#define HOR_PRED16x16       1
#define DC_PRED16x16        2
#define PLANE_PRED16x16     3
#define DC_LEFT_PRED16x16   4
#define DC_TOP_PRED16x16    5
#define DC_128_PRED16x16    6

pixel4 PIXEL_SPLAT_X4(pixel4 x)
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

pixel4 u8_to_u32(uint8_t *src) {
    pixel4 v;
    v = src[0];
    v |= src[1] << 8;
    v |= src[2] << 16;
    v |= src[3] << 24;
    return v;
}

void u32_to_u8(uint8_t *src, pixel4 v) {
    src[0] = v & 0xFF;
    src[1] = (v >> 8) & 0xFF;
    src[2] = (v >> 16) & 0xFF;
    src[3] = (v >> 24) & 0xFF;
}

void u32_to_u8(uint8_t *src, pixel4 v, int stride) {
    src[0*stride] = v & 0xFF;
    src[1*stride] = (v >> 8) & 0xFF;
    src[2*stride] = (v >> 16) & 0xFF;
    src[3*stride] = (v >> 24) & 0xFF;
}

uint8_t clip_pixel(int a) {
    int v = a;
    if (a & (~0xFF))
        v = (-a) >> 31;

    return v;
}

void pred16x16_vertical(uint8_t *top, uint8_t *res)
{
    int i, stride = 16;
    // read luma top row
    pixel4 a = u8_to_u32(top + 0);
    pixel4 b = u8_to_u32(top + 4);
    pixel4 c = u8_to_u32(top + 8);
    pixel4 d = u8_to_u32(top + 12);
    // propagate top row to all rows
    for (i=0; i<16; i++) {
        u32_to_u8(res + i * stride + 0, a);
        u32_to_u8(res + i * stride + 4, b);
        u32_to_u8(res + i * stride + 8, c);
        u32_to_u8(res + i * stride + 12, d);
    }
}

void pred16x16_horizontal(uint8_t *left, uint8_t *res)
{
    int i, stride = 16;
    // read column on the left
    pixel4 a = u8_to_u32(left + 0);
    pixel4 b = u8_to_u32(left + 4);
    pixel4 c = u8_to_u32(left + 8);
    pixel4 d = u8_to_u32(left + 12);
    // copy column on the left to other columns
    for (i=0; i<16; ++i) {
        u32_to_u8(res + 0 + i, a, stride);
        u32_to_u8(res + 4 + i, b, stride);
        u32_to_u8(res + 8 + i, c, stride);
        u32_to_u8(res + 12 + i, d, stride);
    }
}

void pred16x16_dc(uint8_t *left, uint8_t *top, uint8_t *res)
{
    int i, dc=0;
    pixel4 dcsplat;

    for(i=0;i<16; i++){
        dc+= left[i];
        dc+= top[i];
    }

    dcsplat = PIXEL_SPLAT_X4((dc+16)>>5);
    for(i=0; i<16; i++){
        u32_to_u8(res+ 0, dcsplat);
        u32_to_u8(res+ 4, dcsplat);
        u32_to_u8(res+ 8, dcsplat);
        u32_to_u8(res+12, dcsplat);
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

void pred16x16_left_dc(uint8_t *left, uint8_t *res)
{
    int i, dc = 0;
    pixel4 dcsplat;
    for (i = 0; i < 16; ++i) {
        dc += left[i];
    }

    dcsplat = PIXEL_SPLAT_X4((dc+8)>>4);
    for (i=0; i<16; i++){
        // work with the row
        u32_to_u8(res + 0, dcsplat);
        u32_to_u8(res + 4, dcsplat);
        u32_to_u8(res + 8, dcsplat);
        u32_to_u8(res + 12, dcsplat);
        // move to next row
        res += 16;
    }
}

void pred16x16_top_dc(uint8_t *top, uint8_t *res)
{
    int i, dc = 0;

    pixel4 dcsplat;
    for (i = 0; i < 16; ++i) {
        dc += top[i];
    }

    dcsplat = PIXEL_SPLAT_X4((dc+8)>>4);
    for (i=0; i<16; i++){
        u32_to_u8(res + 0, dcsplat);
        u32_to_u8(res + 4, dcsplat);
        u32_to_u8(res + 8, dcsplat);
        u32_to_u8(res + 12, dcsplat);
        res += 16;
    }
}

void pred16x16_128_dc(uint8_t *res)
{
    int i;
    pixel4 dcsplat;
    dcsplat = PIXEL_SPLAT_X4(8);

    for (i=0; i<16; i++){
        u32_to_u8(res + 0, dcsplat);
        u32_to_u8(res + 4, dcsplat);
        u32_to_u8(res + 8, dcsplat);
        u32_to_u8(res + 12, dcsplat);
        res += 16;
    }
}

void pred16x16(In *in, uint8_t *res)
{
    int prediction_mode = in->intra16x16_pred_mode;
    uint8_t *left = in->luma_neighbour_left;
    uint8_t *top = in->luma_neighbour_top;
    uint8_t left_top = in->luma_neighbour_left_top;

    if (prediction_mode == VERT_PRED16x16)
        pred16x16_vertical(top, res);
    else if (prediction_mode == HOR_PRED16x16)
        pred16x16_horizontal(left, res);
    else if (prediction_mode == DC_PRED16x16)
        pred16x16_dc(left, top, res);
    else if (prediction_mode == PLANE_PRED16x16)
        pred16x16_plane(left, top, left_top, res);
    else if (prediction_mode == DC_LEFT_PRED16x16)
        pred16x16_left_dc(left, res);
    else if (prediction_mode == DC_TOP_PRED16x16)
        pred16x16_top_dc(top, res);
    else if (prediction_mode == DC_128_PRED16x16)
        pred16x16_128_dc(res);
    else
        assert(false);
}

void decode_mb(In *in, uint8_t *luma) {
    pred16x16(in, luma);
}