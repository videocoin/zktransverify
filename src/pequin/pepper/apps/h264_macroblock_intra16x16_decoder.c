//
// Created by taras on 05.04.19.
//

#include "h264_macroblock_intra16x16_decoder.h"

#define CHROMA_PRED
#define PRED16x16
#include "h264_macroblock_prediction.h"
#include "h264_macroblock_idct.h"


void decode_mb(struct In *in)
{
    uint8_t *dest_y, *dest_cb, *dest_cr;
    int linesize, uvlinesize;
    int i, j;

    dest_y  = in->dest_y;
    dest_cb = in->dest_cb;
    dest_cr = in->dest_cr;

    linesize   = in->linesize;
    uvlinesize = in->uvlinesize;

    pred8x8_chroma(in->chroma_pred_mode, dest_cb, uvlinesize);
    pred8x8_chroma(in->chroma_pred_mode, dest_cr, uvlinesize);

    pred16x16(in->intra16x16_pred_mode, dest_y, linesize);
    luma_dc_dequant_idct(in->mb,
                         in->mb_luma_dc,
                         in->dequant4_coeff[0][in->qscale]);

    int qp[2];
    qp[0] = in->chroma_qp[0];
    qp[1] = in->chroma_qp[1];
    if (in->non_zero_count_cache[scan8[CHROMA_DC_BLOCK_INDEX + 0]]) {
        chroma_dc_dequant_idct(in->mb + 16 * 16 * 1, in->dequant4_coeff[1][qp[0]]);
    }
    if (in->non_zero_count_cache[scan8[CHROMA_DC_BLOCK_INDEX + 0]]) {
        chroma_dc_dequant_idct(in->mb + 16 * 16 * 2, in->dequant4_coeff[2][qp[1]]);
    }

    idct_add8(dest_cb, dest_cr, in->block_offset, in->mb, uvlinesize, in->non_zero_count_cache);
}

void compute(struct In *input, struct Out *output) {
    decode_mb(input);
}