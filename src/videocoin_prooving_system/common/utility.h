//
// Created by taras on 12.04.19.
//

#ifndef COMP_DEFS_H
#define COMP_DEFS_H

#include <gmp.h>

struct comp_params {
    int n_constraints;
    int n_inputs;
    int n_outputs;
    int n_vars;
};

comp_params parse_params(const char *params_filename);
void convert_to_z(mpz_t z, const mpq_t q, const mpz_t prime);

#endif //COMP_DEFS_H

