//
// Created by taras on 05.04.19.
//

#include "h264_macroblock_intra4x4_decoder.h"

#define CHROMA_PRED
#define PRED4x4
#include "h264_macroblock_prediction.h"
#include "h264_macroblock_idct.h"

// TODO: high_bit_depth?????
int dctcoef_get(int16_t *mb, int high_bit_depth, int index)
{
    int v = mb[index];
    if (high_bit_depth) {
        index *= 2;
        v  = mb[index];
        v |= mb[index+1];
    }
    return v;
}

void decode_mb_predict_luma(struct In *in,
                               int mb_type,
                               int pixel_shift,
                               int linesize,
                               uint8_t *dest_y)
{
    int i;
    int qscale = in->qscale;

//
//    idct_dc_add = h->h264dsp.h264_idct_dc_add;
//    idct_add    = h->h264dsp.h264_idct_add;

    for (i = 0; i < 16; i++) {
        uint8_t *ptr  = dest_y + in->block_offset[i];
        int dir = in->intra4x4_pred_mode_cache[scan8[i]];

        uint8_t *topright;
        int nnz, tr;
        uint64_t tr_high;
        uint8_t tr_as_bytes[8];
        if (dir == DIAG_DOWN_LEFT_PRED || dir == VERT_LEFT_PRED) {
            int topright_avail = (in->topright_samples_available << i) & 0x8000;

            if (!(bool)topright_avail) {
                if ((bool)pixel_shift) {
                    //tr_high  = ((uint16_t *)ptr)[3 - linesize / 2] * 0x0001000100010001ULL;
                    int index = (3 - linesize/2)*2;
                    uint16_t temp = p[index];
                    temp |= p[index+2] << 8;
                    tr_high = temp * 0x0001000100010001ULL;

                    // topright = (uint8_t *)&tr_high;
                    tr_as_bytes[0] = tr_high & 0xFF;
                    tr_as_bytes[1] = (tr_high << 8) & 0xFF;
                    tr_as_bytes[2] = (tr_high << 16) & 0xFF;
                    tr_as_bytes[3] = (tr_high << 24) & 0xFF;
                    tr_as_bytes[4] = (tr_high << 32) & 0xFF;
                    tr_as_bytes[5] = (tr_high << 40) & 0xFF;
                    tr_as_bytes[6] = (tr_high << 48) & 0xFF;
                    tr_as_bytes[7] = (tr_high << 56) & 0xFF;
                    topright = tr_as_bytes;
                } else {
                    tr = ptr[3 - linesize] * 0x01010101u;

                    // topright = (uint8_t *)&tr;
                    tr_as_bytes[0] = tr & 0xFF;
                    tr_as_bytes[1] = (tr << 8) & 0xFF;
                    tr_as_bytes[2] = (tr << 16) & 0xFF;
                    tr_as_bytes[3] = (tr << 24) & 0xFF;
                }
            } else
                topright = ptr + (4 << pixel_shift) - linesize;
        } else
            topright = 0;

        pred4x4(dir, ptr, topright, linesize);
        nnz = in->non_zero_count_cache[scan8[i]];
        if ((bool)nnz) {
            int offset = (i * 16) << pixel_shift;
            if (nnz == 1 && (bool)dctcoef_get(in->mb, pixel_shift, i * 16))
                idct_dc_add(ptr, in->mb + offset, linesize);
            else
                idct_add(ptr, in->mb + offset, linesize);
        }
    }
}


void decode_mb(struct In *in)
{
    int mb_x    = in->mb_x;
    int mb_y    = in->mb_y;
    int mb_xy   = in->mb_xy;
    int mb_type = in->mb_xy;
    uint8_t *dest_y, *dest_cb, *dest_cr;
    int linesize, uvlinesize;
    int i, j;
    // const int block_h   = 16 >> in->chroma_y_shift;
    // Chroma format 4:2:0
    int block_h = 8;

    dest_y  = in->dest_y;
    dest_cb = in->dest_cb;
    dest_cr = in->dest_cr;

    linesize   = in->linesize;
    uvlinesize = in->uvlinesize;

    pred8x8_chroma(in->chroma_pred_mode, dest_cb, uvlinesize);
    pred8x8_chroma(in->chroma_pred_mode, dest_cr, uvlinesize);

    decode_mb_predict_luma(in, mb_type, PIXEL_SHIFT, linesize, dest_y);

//    hl_decode_mb_idct_luma(h, sl, mb_type, SIMPLE, transform_bypass,
//                           PIXEL_SHIFT, block_offset, linesize, dest_y, 0);
//
//    uint8_t *dest[2] = { dest_cb, dest_cr };
//    int qp[2];
//    {
//        qp[0] = sl->chroma_qp[0];
//        qp[1] = sl->chroma_qp[1];
//    }
//    if (sl->non_zero_count_cache[scan8[CHROMA_DC_BLOCK_INDEX + 0]])
//        h->h264dsp.h264_chroma_dc_dequant_idct(sl->mb + (16 * 16 * 1 << PIXEL_SHIFT),
//                                               h->dequant4_coeff[IS_INTRA(mb_type) ? 1 : 4][qp[0]][0]);
//    if (sl->non_zero_count_cache[scan8[CHROMA_DC_BLOCK_INDEX + 1]])
//        h->h264dsp.h264_chroma_dc_dequant_idct(sl->mb + (16 * 16 * 2 << PIXEL_SHIFT),
//                                               h->dequant4_coeff[IS_INTRA(mb_type) ? 2 : 5][qp[1]][0]);
//    h->h264dsp.h264_idct_add8(dest, block_offset,
//                              sl->mb, uvlinesize,
//                              sl->non_zero_count_cache);
}

void compute(struct In *input, struct Out *output) {
    decode_mb(input);
}