#ifndef H264_MACROBLOCK_DECODER_H
#define H264_MACROBLOCK_DECODER_H

#include <stdint.h>
#define pixel uint8_t

// YlCbCr  4:2:0
#define MB_L_WIDTH 16
#define MB_L_HEIGHT 16
#define MB_CR_WIDTH 4
#define MB_CR_HEIGHT 4


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

// TODO: Remove unsupported types.
#define MB_TYPE_INTRA4x4   0x0001
#define MB_TYPE_INTRA16x16 0x0002 //FIXME H.264-specific
#define MB_TYPE_INTRA_PCM  0x0004 //FIXME H.264-specific
#define MB_TYPE_16x16      0x0008
#define MB_TYPE_16x8       0x0010
#define MB_TYPE_8x16       0x0020
#define MB_TYPE_8x8        0x0040
#define MB_TYPE_INTERLACED 0x0080
#define MB_TYPE_DIRECT2    0x0100 //FIXME
#define MB_TYPE_ACPRED     0x0200
#define MB_TYPE_GMC        0x0400
#define MB_TYPE_SKIP       0x0800
#define MB_TYPE_P0L0       0x1000
#define MB_TYPE_P1L0       0x2000
#define MB_TYPE_P0L1       0x4000
#define MB_TYPE_P1L1       0x8000
#define MB_TYPE_L0         (MB_TYPE_P0L0 | MB_TYPE_P1L0)
#define MB_TYPE_L1         (MB_TYPE_P0L1 | MB_TYPE_P1L1)
#define MB_TYPE_L0L1       (MB_TYPE_L0   | MB_TYPE_L1)
#define MB_TYPE_QUANT      0x00010000
#define MB_TYPE_CBP        0x00020000

#define IS_INTRA4x4(a)   ((bool)((a) & MB_TYPE_INTRA4x4))
#define IS_INTRA_PCM(a)  ((bool)((a) & MB_TYPE_INTRA_PCM))
#define IS_INTRA(a)      ((bool)((a) & 7))

#define MB_TYPE_8x8DCT     0x01000000
#define IS_8x8DCT(a)       ((bool)((a) & MB_TYPE_8x8DCT))

// TODO: if complex frame pixel shift should be defined in input
#define BITS 8
#define PIXEL_SHIFT (BITS >> 4)

// TODO: can we remove this?
#define QP_MAX_NUM (51 + 6*6)           // The maximum supported qp

/******************************************************************
 * Structure contains parameters from H264 Context and Slice context.
 * Predictions:
 *      YCbCr 4:2:0
 *      Picture Type FRAME(3)
 *      Deblocking filter is not included
 ******************************************************************/
struct In {
    int slice_num;

    int qscale;

    int8_t intra4x4_pred_mode_cache[5 * 8];

    unsigned int topleft_samples_available;
    unsigned int top_samples_available;
    unsigned int topright_samples_available;
    unsigned int left_samples_available;

    int linesize, uvlinesize;

    int mb_x, mb_y;
    int mb_xy;
;
    int mb_type;

    int top_type;

    int mb_field_decoding_flag;

    int chroma_pred_mode;

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

    uint16_t slice_table[1023 * 1024]; // TODO: W and H are dummy values
    /**
     * block_offset[ 0..23] for frame macroblocks
     * block_offset[24..47] for field macroblocks
     */
    int block_offset[2 * (16 * 3)];
    uint32_t dequant4_coeff[QP_MAX_NUM + 1][16];

    pixel dest_y[MB_L_WIDTH * MB_L_WIDTH];
    pixel dest_cb[MB_CR_WIDTH * MB_CR_HEIGHT];
    pixel dest_cr[MB_CR_WIDTH * MB_CR_HEIGHT];
};

struct Out {
    pixel dest_y[MB_L_WIDTH * MB_L_WIDTH];
    pixel dest_cb[MB_CR_WIDTH * MB_CR_HEIGHT];
    pixel dest_cr[MB_CR_WIDTH * MB_CR_HEIGHT];
};

#endif  // H264_MACROBLOCK_DECODER_H