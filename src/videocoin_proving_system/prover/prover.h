//
// Created by taras on 15.04.19.
//

#ifndef PROVER_H
#define PROVER_H

#include <common/utility.h>

void initialize_prover();

double generate_ssim_proof(const char *pk_fn,
                           const unsigned char *src1, size_t src1_len,
                           const unsigned char *src2, size_t src2_len,
                           const char *output_fn, const char *proof_fn);

#endif //PROVER_H
