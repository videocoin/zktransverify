#ifndef H264_MACROBLOCK_INTRA4x4_DECODER_H
#define H264_MACROBLOCK_INTRA4x4_DECODER_H

#include "h264_macroblock_defs.h"

struct In {
    int chroma_qp[2];   // QPc

    int8_t intra4x4_pred_mode_cache[5 * 8];

    unsigned int topright_samples_available;

    int linesize, uvlinesize;

    int chroma_pred_mode;

    /**
     * non zero coeff count cache.
     * is 64 if not available.
     */
    uint8_t non_zero_count_cache[15 * 8];

    ///< as a dct coefficient is int32_t in high depth, we need to reserve twice the space.
    int16_t mb[16 * 48 * 2];

    // END OF SLICE CONTEXT

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

