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

void convert_to_z(const int size, mpz_t *z, const mpq_t *q, const mpz_t prime) {
    for (int i = 0; i < size; i++)
        convert_to_z(z[i], q[i], prime);
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

void alloc_init_vec(mpz_t **arr, uint32_t size) {
    *arr = new mpz_t[size];
    for (uint32_t i=0; i<size; i++)
        alloc_init_scalar((*arr)[i]);
}

void alloc_init_vec(mpq_t **arr, uint32_t size) {
    *arr = new mpq_t[size];
    for (uint32_t i=0; i<size; i++) {
        alloc_init_scalar((*arr)[i]);
    }
}

void alloc_init_scalar(mpz_t s) {
    mpz_init2(s, INIT_MPZ_BITS);
    //mpz_init(s);
    mpz_set_ui(s, 0);
}

void alloc_init_scalar(mpq_t s) {
    mpq_init(s);
    mpq_set_ui(s, 0, 1);
}

void clear_del_vec(mpq_t* vec, const uint32_t n) {
    for (uint32_t i = 0; i < n; i++)
        mpq_clear(vec[i]);
    delete[] vec;
}

void clear_del_vec(mpz_t* vec, const uint32_t n) {
    for (uint32_t i = 0; i < n; i++)
        mpz_clear(vec[i]);
    delete[] vec;
}

void assert_zero(int value) {
    assert(value == 0);
}