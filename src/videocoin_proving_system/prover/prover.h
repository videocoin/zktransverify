//
// Created by taras on 15.04.19.
//

#ifndef PROVER_H
#define PROVER_H

void initialize_prover();

void generate_ssim_proof(const char *pk_fn,
                        int ref_ssim,
                        const unsigned char *src1, unsigned long src1_len,
                        const unsigned char *src2, unsigned long src2_len,
                        const char *proof_fn);

#endif //PROVER_H