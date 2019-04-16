//
// Created by taras on 15.04.19.
//

#ifndef PROVER_H
#define PROVER_H

void initialize_prover();
void generate_ssim_proof(const char *pk_fn, const unsigned char *src_luma1, const unsigned char *src_luma2,
                          const char *output_fn, const char *proof_fn, double &ssim);
void generate_ssim_proof(const char *pk_fn, const char *input_fn, const char *output_fn, const char *proof_fn);

#endif //PROVER_H
