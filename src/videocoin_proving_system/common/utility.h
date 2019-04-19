//
// Created by taras on 12.04.19.
//

#ifndef COMP_DEFS_H
#define COMP_DEFS_H

#include <gmp.h>
#include <cstdint>

#define INIT_MPZ_BITS 128

struct comp_params {
    int n_constraints;
    int n_inputs;
    int n_outputs;
    int n_vars;
};

class ssim_mode {
public:
    enum value {
        _invalid,
        _x16 = 16,
        _x32 = 32,
        _x64 = 64
    };
    std::string str() const;
    bool operator==(ssim_mode a) const { return _value == a._value; }
    bool operator!=(ssim_mode a) const { return _value != a._value; }
    bool is_valid() const { return _value != _invalid; }

    static ssim_mode from_str(const char *v);
    static ssim_mode x16() { return ssim_mode(value::_x16); }
    static ssim_mode x32() { return ssim_mode(value::_x32); }
    static ssim_mode x64() { return ssim_mode(value::_x64); }

    constexpr ssim_mode(value v) : _value(v) {}
    constexpr ssim_mode() : _value(value::_invalid) {}

private:
    value _value;
};

comp_params parse_params(const char *params_filename);
std::string ssim_mode_to_str(ssim_mode c);

void convert_to_z(mpz_t z, const mpq_t q, const mpz_t prime);
void convert_to_z(const int size, mpz_t *z, const mpq_t *q, const mpz_t prime);

void alloc_init_scalar(mpz_t s);
void alloc_init_scalar(mpq_t s);

void alloc_init_vec(mpz_t **arr, uint32_t size);
void alloc_init_vec(mpq_t **arr, uint32_t size);

void clear_del_vec(mpz_t* vec, const uint32_t n);
void clear_del_vec(mpq_t* vec, const uint32_t n);

void assert_zero(int value);

#endif //COMP_DEFS_H

