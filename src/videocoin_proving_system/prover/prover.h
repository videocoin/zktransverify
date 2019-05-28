//
// Created by taras on 15.04.19.
//

#ifndef PROVER_H
#define PROVER_H

void initialize_prover();

__attribute_deprecated__
void generate_ssim_proof(const char *pk_fn,
                        int ref_ssim,
                        const unsigned char *src1, unsigned long src1_len,
                        const unsigned char *src2, unsigned long src2_len,
                        const char *proof_bin_fn,
                        const char *proof_uncompressed_fn,
                        const char *proof_json_fn);

void generate_h264_proof(const char *pk_fn,
                         int ref_ssim,
                         const unsigned char *ref_digest, unsigned long digest_len,
                         const unsigned char *src1, unsigned long src1_len,
                         const unsigned char *src2, unsigned long src2_len,
                         const char *proof_bin_fn,
                         const char *proof_uncompressed_fn,
                         const char *proof_json_fn);

#endif //PROVER_H