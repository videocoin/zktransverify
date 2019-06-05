//
// Created by taras on 05.06.19.
//

#ifndef TEMP_H264_MB_DECODER_H
#define TEMP_H264_MB_DECODER_H

#include <stdint.h>

typedef struct In {
    char *mb_data; // Macroblock DCT Coefficients
    // 3 planes of 16x16 32bit coefficients
    int  mb_size;

    int mb_type;
    int intra16x16_pred_mode;

    uint8_t         luma_decoded[16 * 16]; // will be used for debug. Circuit should generate identical data to this

    // This data will be used to decode macroblock
    uint8_t         luma_neighbour_top[16];
    uint8_t         luma_neighbour_left[16];
    int             luma_has_neighbour_top;
    int             luma_has_neighbour_left;
} In;

void decode_mb(In *in, uint8_t *luma);

#endif //TEMP_H264_MB_DECODER_H
