//
// Created by taras on 05.04.19.
//

#include "h264_macroblock_intra4x4_decoder.h"

#define CHROMA_PRED
#define PRED4x4
#include "h264_macroblock_prediction.h"
#include "h264_macroblock_idct.h"

int dctcoef_get(int16_t *mb, int index)
{
    int v = mb[index];
    return v;
}

void decode_mb_predict_luma(struct In *in,
                               int linesize,
                               uint8_t *dest_y)
{
    int i;

    for (i = 0; i < 16; i++) {
        uint8_t *ptr  = dest_y + in->block_offset[i];
        int dir = in->intra4x4_pred_mode_cache[scan8[i]];

        uint8_t *topright;
        int nnz, tr;
        uint8_t tr_as_bytes[4];
        if (dir == DIAG_DOWN_LEFT_PRED || dir == VERT_LEFT_PRED) {
            int topright_avail = (in->topright_samples_available << i) & 0x8000;

            if (!(bool)topright_avail) {
                tr = ptr[3 - linesize] * 0x01010101u;

                // topright = (uint8_t *)&tr;
                tr_as_bytes[0] = tr & 0xFF;
                tr_as_bytes[1] = (tr << 8) & 0xFF;
                tr_as_bytes[2] = (tr << 16) & 0xFF;
                tr_as_bytes[3] = (tr << 24) & 0xFF;
            } else
                topright = ptr + 4 - linesize;
        } else
            topright = 0;

        pred4x4(dir, ptr, topright, linesize);
        nnz = in->non_zero_count_cache[scan8[i]];
        if ((bool)nnz) {
            int offset = i * 16;
            if (nnz == 1 && (bool)dctcoef_get(in->mb, i * 16))
                idct_dc_add(ptr, in->mb + offset, linesize);
            else
                idct_add(ptr, in->mb + offset, linesize);
        }
    }
}


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

    decode_mb_predict_luma(in, linesize, dest_y);

    int qp[2];
    qp[0] = in->chroma_qp[0];
    qp[1] = in->chroma_qp[1];
    if (in->non_zero_count_cache[scan8[CHROMA_DC_BLOCK_INDEX + 0]]) {
        chroma_dc_dequant_idct(in->mb + 16 * 16 * 1, in->dequant4_coeff[0][qp[0]]);
    }
    if (in->non_zero_count_cache[scan8[CHROMA_DC_BLOCK_INDEX + 0]]) {
        chroma_dc_dequant_idct(in->mb + 16 * 16 * 2, in->dequant4_coeff[1][qp[1]]);
    }

    idct_add8(dest_cb, dest_cr, in->block_offset, in->mb, uvlinesize, in->non_zero_count_cache);
}

void compute(struct In *input, struct Out *output) {
    decode_mb(input);
}