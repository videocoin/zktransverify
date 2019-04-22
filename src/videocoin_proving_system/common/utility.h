//
// Created by taras on 12.04.19.
//

#ifndef COMP_DEFS_H
#define COMP_DEFS_H

#include <gmp.h>
#include <cstdint>
#include <fstream>
#include <libsnark/relations/constraint_satisfaction_problems/r1cs/r1cs.hpp>
#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_ppzksnark/r1cs_ppzksnark.hpp>

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
    bool is_16() const { return _value == _x16; }
    bool is_32() const { return _value == _x32; }
    bool is_64() const { return _value == _x64; }
    int as_int() const { return _value; }

    static ssim_mode from_str(const char *v);
    static ssim_mode from_int(const int v);

    constexpr ssim_mode(value v) : _value(v) {}
    constexpr ssim_mode() : _value(value::_invalid) {}

private:
    value _value;
};

comp_params parse_params(const char *params_filename);

void convert_to_z(mpz_t z, const mpq_t q, const mpz_t prime);
void convert_to_z(const int size, mpz_t *z, const mpq_t *q, const mpz_t prime);

void alloc_init_scalar(mpz_t s);
void alloc_init_scalar(mpq_t s);

void alloc_init_vec(mpz_t **arr, uint32_t size);
void alloc_init_vec(mpq_t **arr, uint32_t size);

void clear_scalar(mpz_t);
void clear_scalar(mpq_t);

void clear_del_vec(mpz_t* vec, const uint32_t n);
void clear_del_vec(mpq_t* vec, const uint32_t n);

void assert_zero(int value);

template<typename ppT>
void print_proof_to_file(libsnark::r1cs_ppzksnark_proof<ppT> proof, const char *path_to_file)
{
    std::ofstream proof_data(path_to_file);

    libff::G1<ppT> A_g(proof.g_A.g);
    A_g.to_affine_coordinates();
    libff::G1<ppT> A_h(proof.g_A.h);
    A_h.to_affine_coordinates();

    libff::G2<ppT> B_g(proof.g_B.g);
    B_g.to_affine_coordinates();
    libff::G1<ppT> B_h(proof.g_B.h);
    B_h.to_affine_coordinates();

    libff::G1<ppT> C_g(proof.g_C.g);
    C_g.to_affine_coordinates();
    libff::G1<ppT> C_h(proof.g_C.h);
    C_h.to_affine_coordinates();

    libff::G1<ppT> H(proof.g_H);
    H.to_affine_coordinates();
    libff::G1<ppT> K(proof.g_K);
    K.to_affine_coordinates();

    proof_data << A_g.X << std::endl;
    proof_data << A_g.Y << std::endl;

    proof_data << A_h.X << std::endl;
    proof_data << A_h.Y << std::endl;

    proof_data << B_g.X << std::endl;
    proof_data << B_g.Y << std::endl;

    proof_data << B_h.X << std::endl;
    proof_data << B_h.Y << std::endl;

    proof_data << C_g.X << std::endl;
    proof_data << C_g.Y << std::endl;

    proof_data << C_h.X << std::endl;
    proof_data << C_h.Y << std::endl;

    proof_data << H.X << std::endl;
    proof_data << H.Y << std::endl;

    proof_data << K.X << std::endl;
    proof_data << K.Y << std::endl;

    proof_data.close();
}

#endif //COMP_DEFS_H

