//
// Created by taras on 15.04.19.
//

#ifndef PROVER_H
#define PROVER_H

#include <common/utility.h>

void initialize_prover();

void generate_ssim_proof(ssim_mode mode, const char *pk_fn,
                    const unsigned char *src_luma1, const unsigned char *src_luma2,
                    const char *output_fn, const char *proof_fn, double &ssim);

#endif //PROVER_H
