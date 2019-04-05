//
// Created by taras on 04.04.19.
//

#include "h264_macroblock_decoder.h"
#include "h264_macroblock_prediction.h"
#include "h264_macroblock_luma_pred.h"

void hl_decode_mb_simple(struct In *in)
{
    const int mb_x    = in->mb_x;
    const int mb_y    = in->mb_y;
    const int mb_xy   = in->mb_xy;
    const int mb_type = in->mb_xy;
    uint8_t *dest_y, *dest_cb, *dest_cr;
    int linesize, uvlinesize /*dct_offset*/;
    int i, j;
    const int *block_offset = &in->block_offset[0];
    const int block_h   = 16 >> in->chroma_y_shift;

    dest_y  = in->dest_y;
    dest_cb = in->dest_cb;
    dest_cr = in->dest_cr;

    linesize   = in->linesize;
    uvlinesize = in->uvlinesize;

    if (IS_INTRA(mb_type)) {

        pred8x8_chroma(in->chroma_pred_mode, dest_cb, uvlinesize);
        pred8x8_chroma(in->chroma_pred_mode, dest_cr, uvlinesize);

        hl_decode_mb_predict_luma(in, mb_type, PIXEL_SHIFT,
                                  block_offset, linesize, dest_y);

    } else {
        // TODO: Lets disable this part for now.
        // B, P macroblocks
//        FUNC(hl_motion_420)(h, sl, dest_y, dest_cb, dest_cr,
//                            h->qpel_put, h->h264chroma.put_h264_chroma_pixels_tab,
//                            h->qpel_avg, h->h264chroma.avg_h264_chroma_pixels_tab,
//                            h->h264dsp.weight_h264_pixels_tab,
//                            h->h264dsp.biweight_h264_pixels_tab);
    }

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
    bool is_complex = IS_INTRA_PCM(input->mb_type) || (input->qscale == 0);

    if (is_complex) {

    } else {
        hl_decode_mb_simple(input);
    }
}