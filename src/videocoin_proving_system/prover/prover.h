//
// Created by taras on 15.04.19.
//

#ifndef PROVER_H
#define PROVER_H

typedef struct _mb_context
{
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
} mb_context_t;

void initialize_prover();

__attribute_deprecated__
void generate_ssim_proof(const char *pk_fn,
                        int ref_ssim,
                        const unsigned char *src1, unsigned long src1_len,
                        const unsigned char *src2, unsigned long src2_len,
                        const char *proof_bin_fn,
                        const char *proof_uncompressed_fn,
                        const char *proof_json_fn);

__attribute_deprecated__
void generate_h264_proof(const char *pk_fn,
                         int ref_ssim,
                         const unsigned char *ref_digest, unsigned long digest_len,
                         const unsigned char *src1, unsigned long src1_len,
                         const unsigned char *src2, unsigned long src2_len,
                         const char *proof_bin_fn,
                         const char *proof_uncompressed_fn,
                         const char *proof_json_fn);

void generate_h264_proof2(const char *pk_fn,
                           int ref_ssim,
                           const unsigned char *ref_digest, unsigned long digest_len,
                           const unsigned char *ref_src, unsigned long src_len,
                           const mb_context_t *mb_context,
                           const char *proof_bin_fn,
                           const char *proof_uncompressed_fn,
                           const char *proof_json_fn);

#endif //PROVER_H