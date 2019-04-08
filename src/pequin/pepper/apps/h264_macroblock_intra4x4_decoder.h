#ifndef H264_MACROBLOCK_INTRA4x4_DECODER_H
#define H264_MACROBLOCK_INTRA4x4_DECODER_H

#include "h264_macroblock_defs.h"

struct In {
    int slice_num;

    int qscale;
    int chroma_qp[2];   // QPc

    int8_t intra4x4_pred_mode_cache[5 * 8];

    unsigned int topleft_samples_available;
    unsigned int top_samples_available;
    unsigned int topright_samples_available;
    unsigned int left_samples_available;

    int linesize, uvlinesize;

    int mb_x, mb_y;
    int mb_xy;
    int mb_type;

    int top_type;

    int mb_field_decoding_flag;

    int chroma_pred_mode;
    int intra16x16_pred_mode;

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

    int pixel_shift;    ///< 0 for 8-bit H264, 1 for high-bit-depth H264
    int chroma_y_shift;

    uint16_t slice_table[1023 * 1024]; // TODO: W and H are dummy values
    /**
     * block_offset[ 0..23] for frame macroblocks
     * block_offset[24..47] for field macroblocks
     */
    int block_offset[2 * (16 * 3)];

    uint32_t dequant4_coeff[2][2];

    pixel dest_y[MB_L_IN_LENGTH];
    pixel dest_cb[MB_CR_IN_LENGTH];
    pixel dest_cr[MB_CR_IN_LENGTH];
};

#endif

