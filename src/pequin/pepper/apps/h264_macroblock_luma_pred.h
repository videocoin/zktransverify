//
// Created by taras on 04.04.19.
//

#ifndef H264_MACROBLOCK_LUMA_PRED_H
#define H264_MACROBLOCK_LUMA_PRED_H

#define VERT_PRED              0
#define HOR_PRED               1
#define DC_PRED                2
#define DIAG_DOWN_LEFT_PRED    3
#define DIAG_DOWN_RIGHT_PRED   4
#define VERT_RIGHT_PRED        5
#define HOR_DOWN_PRED          6
#define VERT_LEFT_PRED         7
#define HOR_UP_PRED            8

#define LUMA_DC_BLOCK_INDEX   48

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

void hl_decode_mb_predict_luma(struct In *in,
                               int mb_type,
                               int pixel_shift,
                               const int *block_offset,
                               int linesize,
                               uint8_t *dest_y)
{
    void (*idct_add)(uint8_t *dst, int16_t *block, int stride);
    void (*idct_dc_add)(uint8_t *dst, int16_t *block, int stride);
    int i;
    int qscale = in->qscale;

    if (IS_INTRA4x4(mb_type)) {
        if (IS_8x8DCT(mb_type)) {

            idct_dc_add = h->h264dsp.h264_idct8_dc_add;
            idct_add    = h->h264dsp.h264_idct8_add;

            for (i = 0; i < 16; i += 4) {
                uint8_t *ptr = dest_y + block_offset[i];
                const int dir = in->intra4x4_pred_mode_cache[scan8[i]];

                const int nnz = in->non_zero_count_cache[scan8[i]];
                pred8x8l(dir, ptr, (in->topleft_samples_available << i) & 0x8000,
                        (in->topright_samples_available << i) & 0x4000, linesize);

                if (nnz) {
                    if (nnz == 1 && dctcoef_get(in->mb, pixel_shift, i * 16))
                        idct_dc_add(ptr, in->mb + (i * 16 << pixel_shift), linesize);
                    else
                        idct_add(ptr, in->mb + (i * 16 << pixel_shift), linesize);
                }

            }
        } else {

            idct_dc_add = h->h264dsp.h264_idct_dc_add;
            idct_add    = h->h264dsp.h264_idct_add;

            for (i = 0; i < 16; i++) {
                uint8_t *ptr = dest_y + block_offset[i];
                const int dir      = in->intra4x4_pred_mode_cache[scan8[i]];

                uint8_t *topright;
                int nnz, tr;
                uint64_t tr_high;
                if (dir == DIAG_DOWN_LEFT_PRED || dir == VERT_LEFT_PRED) {
                    const int topright_avail = (in->topright_samples_available << i) & 0x8000;

                    if (!topright_avail) {
                        if (pixel_shift) {
                            tr_high  = ((uint16_t *)ptr)[3 - linesize / 2] * 0x0001000100010001ULL;
                            topright = (uint8_t *)&tr_high;
                        } else {
                            tr       = ptr[3 - linesize] * 0x01010101u;
                            topright = (uint8_t *)&tr;
                        }
                    } else
                        topright = ptr + (4 << pixel_shift) - linesize;
                } else
                    topright = 0;

                pred4x4(dir, ptr, topright, linesize);
                nnz = in->non_zero_count_cache[scan8[i]];
                if (nnz) {
                    if (nnz == 1 && dctcoef_get(sl->mb, pixel_shift, i * 16))
                        idct_dc_add(ptr, in->mb + (i * 16 << pixel_shift), linesize);
                    else
                        idct_add(ptr, in->mb + (i * 16 << pixel_shift), linesize);
                }
            }
        }
    } else {
        pred16x16(in->intra16x16_pred_mode, dest_y, linesize);
        if (in->non_zero_count_cache[scan8[LUMA_DC_BLOCK_INDEX]]) {

            h->h264dsp.h264_luma_dc_dequant_idct(in->mb,
                                                 in->mb_luma_dc[0],
                                                 h->dequant4_coeff[0][qscale][0]);
        }
    }
}

#endif //H264_MACROBLOCK_LUMA_PRED_H
