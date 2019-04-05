#ifndef H264_MACROBLOCK_DEFS_H
#define H264_MACROBLOCK_DEFS_H

#include <stdint.h>
#define pixel  uint8_t
#define pixel4 uint32_t

// YlCbCr  4:2:0
#define MB_L_WIDTH 16
#define MB_L_HEIGHT 16
#define MB_CR_WIDTH 4
#define MB_CR_HEIGHT 4

// TODO: this may be changed
#define BITS 8
#define PIXEL_SHIFT (BITS >> 4)

/*******************************************************
 * Output Macroblock in 4:2:0 format, Input structure
 * is different for macroblock types.
 *******************************************************/
struct Out {
    pixel dest_y[MB_L_WIDTH * MB_L_WIDTH];
    pixel dest_cb[MB_CR_WIDTH * MB_CR_HEIGHT];
    pixel dest_cr[MB_CR_WIDTH * MB_CR_HEIGHT];
};

// This table must be here because scan8[constant] must be known at compiletime
const uint8_t scan8[16 * 3 + 3] = {
        4 + 1 * 8, 5 + 1 * 8, 4 + 2 * 8, 5 + 2 * 8,
        6 + 1 * 8, 7 + 1 * 8, 6 + 2 * 8, 7 + 2 * 8,
        4 + 3 * 8, 5 + 3 * 8, 4 + 4 * 8, 5 + 4 * 8,
        6 + 3 * 8, 7 + 3 * 8, 6 + 4 * 8, 7 + 4 * 8,
        4 + 6 * 8, 5 + 6 * 8, 4 + 7 * 8, 5 + 7 * 8,
        6 + 6 * 8, 7 + 6 * 8, 6 + 7 * 8, 7 + 7 * 8,
        4 + 8 * 8, 5 + 8 * 8, 4 + 9 * 8, 5 + 9 * 8,
        6 + 8 * 8, 7 + 8 * 8, 6 + 9 * 8, 7 + 9 * 8,
        4 + 11 * 8, 5 + 11 * 8, 4 + 12 * 8, 5 + 12 * 8,
        6 + 11 * 8, 7 + 11 * 8, 6 + 12 * 8, 7 + 12 * 8,
        4 + 13 * 8, 5 + 13 * 8, 4 + 14 * 8, 5 + 14 * 8,
        6 + 13 * 8, 7 + 13 * 8, 6 + 14 * 8, 7 + 14 * 8,
        0 + 0 * 8, 0 + 5 * 8, 0 + 10 * 8
};

//#define PIXEL_SPLAT_X4(x)       ((x)*0x01010101U)

pixel4 PIXEL_SPLAT_X4(pixel4 x)
{
    return x * 0x01010101U;
}

pixel4 AV_RN4PA(uint8_t *src) {
    pixel4 v;
    v = src[0];
    v |= src[1] << 8;
    v |= src[2] << 16;
    v |= src[3] << 24;
    return v;
}

void AV_WN4PA(uint8_t *src, pixel4 v) {
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

#endif // H264_MACROBLOCK_DEFS_H