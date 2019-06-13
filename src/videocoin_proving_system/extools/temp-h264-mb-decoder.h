//
// Created by taras on 05.06.19.
//

#ifndef TEMP_H264_MB_DECODER_H
#define TEMP_H264_MB_DECODER_H

#include <stdint.h>

typedef struct In {
    int mb_type;
    int mb_x;
    int mb_y;
    int mb_xy;
    int mb_width;
    int mb_height;

    int dequant_coeff;
    int mb_field_decoding_flag;
    int deblocking_filter;
    int intra16x16_pred_mode;

    int16_t mb[16 * 16];
    int16_t mb_luma_dc[16];
    uint8_t non_zero_count_cache[15 * 8];

    // This data will be used to decode macroblock
    uint8_t top_border[8 + 16 + 8];
    uint8_t luma_top[8 + 16 + 8];
    uint8_t luma_left[16];
    uint8_t luma_decoded[16*16];
} In;

void decode_mb(In *in, uint8_t *luma);

#endif //TEMP_H264_MB_DECODER_H
