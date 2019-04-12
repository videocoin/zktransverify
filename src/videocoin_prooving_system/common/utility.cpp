//
// Created by taras on 12.04.19.
//

#include "utility.h"
#include <iostream>
#include <fstream>
#include <assert.h>

comp_params parse_params(const char *params_filename) {
    std::ifstream param_file(params_filename);
    if (!param_file.is_open()) {
        std::cerr << "ERROR: " << params_filename << " not found." << std::endl;
        exit(1);
    }
    int num_constraints, num_inputs, num_outputs, num_vars;
    std::string comment;
    param_file >> num_constraints >> comment >> num_inputs >> comment >> num_outputs >> comment >> num_vars;
    param_file.close();

    return comp_params{num_constraints, num_inputs, num_outputs, num_vars};
}

void convert_to_z(mpz_t z, const mpq_t q, const mpz_t prime) {
    assert(mpz_sgn(prime) != 0);
    if (mpz_cmp_ui(mpq_denref(q), 1) == 0) {
        mpz_set(z, mpq_numref(q));
    } else if (mpz_cmp_ui(mpq_denref(q), 0) == 0) {
        mpz_set_ui(z, 0);
    } else {
        mpz_invert(z, mpq_denref(q), prime);
        mpz_mul(z, z, mpq_numref(q));
    }
    mpz_mod(z, z, prime);
}