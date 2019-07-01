//
// Created by taras on 12.04.19.
//

#include <string>
#include <iostream>
#include <fstream>
#include <sys/stat.h>

#include <libsnark/relations/constraint_satisfaction_problems/r1cs/r1cs.hpp>
#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_ppzksnark/r1cs_ppzksnark.hpp>

#include <common/defs.h>
#include <common/utility.h>

#include "computation_prover.h"
#include "prover.h"

template<typename ppT>
void generate_proof_internal(const comp_params &p,
                             const char *pws_fn,
                             libsnark::r1cs_ppzksnark_proving_key<ppT> &pk,
                             int ref_ssim,
                             libsnark::r1cs_ppzksnark_proof<ppT> &proof);

template<typename ppT>
void generate_proof_internal2(const comp_params &p,
                              const char *pws_fn,
                              libsnark::r1cs_ppzksnark_proving_key<ppT> &pk,
                              int ref_ssim,
                              const unsigned char *ref_digest, unsigned long digest_len,
                              libsnark::r1cs_ppzksnark_proof<ppT> &proof);

static void write_to_auxiliary_input(const char *exo_name, const unsigned char *src1, size_t len1,
                                     const unsigned char *src2, size_t len2,
                                     const unsigned char *digest, size_t digest_len);

static void write_mb_to_auxiliary_input(const char *exo_name, const unsigned char *src1, size_t len1);
static void write_mb_decoder_data_to_auxiliary_input(const char *exo_name, const mb_context_t *mb_context);

static void clear_auxiliary_input(const char *exo_name);

void initialize_prover() {
    initialize_env();
    libsnark::default_r1cs_ppzksnark_pp::init_public_params();
}

void generate_h264_proof2(const char *pk_fn,
                           int ref_ssim,
                           const unsigned char *ref_digest, unsigned long digest_len,
                           const unsigned char *ref_src, unsigned long src_len,
                           const mb_context_t *mb_context,
                           const char *proof_bin_fn,
                           const char *proof_uncompressed_fn,
                           const char *proof_json_fn) {
    if (ref_src == nullptr || src_len != 256) {
        std::cerr << "ERROR: invalid source (should be 256 bytes long)" << std::endl;
        exit(-1);
    }
    if (ref_digest == nullptr || digest_len != 32) {
        std::cerr << "ERROR: invalid reference digest." << std::endl;
        exit(-1);
    }
    if (mb_context == nullptr) {
        std::cerr << "ERROR: invalid macroblock decoder context." << std::endl;
        exit(-1);
    }

    auto mode = prover_mode::from_str("h264prover");

    std::string app_path = application_dir + mode.str() + "/";
    std::string params = app_path + "params";
    std::string pws = app_path + "pws";
    comp_params p = parse_params(params.c_str());

    std::ifstream pkey(pk_fn);
    if (!pkey.is_open()) {
        std::cerr << "ERROR: " << pk_fn << " not found." << std::endl;
        exit(-1);
    }

    libff::start_profiling();
    libsnark::r1cs_ppzksnark_proving_key<libsnark::default_r1cs_ppzksnark_pp> pk;
    libff::enter_block("Reading proving key from file");
    pkey >> pk;
    libff::leave_block("Reading proving key from file");

    // write reference macroblock as auxiliary input
    libff::enter_block("Call to write_mb_to_auxiliary_input");
    write_mb_to_auxiliary_input("exo0", ref_src, src_len);
    libff::leave_block("Call to write_mb_to_auxiliary_input");

    // write transcoded macroblock data as auxiliary input
    libff::enter_block("Call to write_mb_decoder_data_to_auxiliary_input");
    write_mb_decoder_data_to_auxiliary_input("exo1", mb_context);
    libff::leave_block("Call to write_mb_decoder_data_to_auxiliary_input");

    libsnark::r1cs_ppzksnark_proof<libsnark::default_r1cs_ppzksnark_pp> proof;
    generate_proof_internal2(p, pws.c_str(), pk, ref_ssim, ref_digest, digest_len, proof);

    clear_auxiliary_input("exo0");
    clear_auxiliary_input("exo1");

    if (proof_bin_fn != nullptr) {
        std::ofstream proof_file(proof_bin_fn);
        if (proof_file.is_open()) {
            proof_file << proof;
            proof_file.close();
        } else {
            std::cerr << "WARNING: unable to open file at path: " << proof_bin_fn << std::endl;
        }
    }

    if (proof_uncompressed_fn != nullptr) {
        print_proof_to_file(proof, proof_uncompressed_fn);
    }

    if (proof_json_fn != nullptr) {
        print_proof_to_json(proof, proof_json_fn);
    }
}

void generate_h264_proof(const char *pk_fn,
                         int ref_ssim,
                         const unsigned char *ref_digest, unsigned long digest_len,
                         const unsigned char *src1, unsigned long src1_len,
                         const unsigned char *src2, unsigned long src2_len,
                         const char *proof_bin_fn,
                         const char *proof_uncompressed_fn,
                         const char *proof_json_fn) {
    if (src1 == nullptr || src2 == nullptr) {
        std::cerr << "ERROR: invalid source." << std::endl;
        exit(-1);
    }

    if (src1_len != src2_len && src1_len != 256) {
        std::cerr << "ERROR: src1 and src2 should be same length and 256 bytes long." << std::endl;
        exit(-1);
    }

    if (ref_digest == nullptr || digest_len != 32) {
        std::cerr << "ERROR: invalid reference digest." << std::endl;
        exit(-1);
    }

    if (src1_len != src2_len) {
        std::cerr << "ERROR: src1 and src2 should be same length." << std::endl;
        exit(-1);
    }

    auto mode = prover_mode::from_str("h264");

    std::string app_path = application_dir + mode.str() + "/";
    std::string params = app_path + "params";
    std::string pws = app_path + "pws";
    comp_params p = parse_params(params.c_str());

    std::ifstream pkey(pk_fn);
    if (!pkey.is_open()) {
        std::cerr << "ERROR: " << pk_fn << " not found." << std::endl;
        exit(-1);
    }

    libff::start_profiling();
    libsnark::r1cs_ppzksnark_proving_key<libsnark::default_r1cs_ppzksnark_pp> pk;
    libff::enter_block("Reading proving key from file");
    pkey >> pk;
    libff::leave_block("Reading proving key from file");

    libff::enter_block("Call to write_to_auxiliary_input");
    write_to_auxiliary_input("exo3", src1, src1_len, src2, src2_len, ref_digest, digest_len);
    libff::leave_block("Call to write_to_auxiliary_input");

    libsnark::r1cs_ppzksnark_proof<libsnark::default_r1cs_ppzksnark_pp> proof;
    generate_proof_internal(p, pws.c_str(), pk, ref_ssim, proof);

    clear_auxiliary_input("exo3");

    if (proof_bin_fn != nullptr) {
        std::ofstream proof_file(proof_bin_fn);
        if (proof_file.is_open()) {
            proof_file << proof;
            proof_file.close();
        } else {
            std::cerr << "WARNING: unable to open file at path: " << proof_bin_fn << std::endl;
        }
    }

    if (proof_uncompressed_fn != nullptr) {
        print_proof_to_file(proof, proof_uncompressed_fn);
    }

    if (proof_json_fn != nullptr) {
        print_proof_to_json(proof, proof_json_fn);
    }
}

void generate_ssim_proof(const char *pk_fn,
                         int ref_ssim,
                         const unsigned char *src1, unsigned long src1_len,
                         const unsigned char *src2, unsigned long src2_len,
                         const char *proof_bin_fn,
                         const char *proof_uncompressed_fn,
                         const char *proof_json_fn) {
    if (src1 == nullptr || src2 == nullptr) {
        std::cerr << "ERROR: invalid source." << std::endl;
        exit(-1);
    }

    if (src1_len != src2_len) {
        std::cerr << "ERROR: src1 and src2 should be same length." << std::endl;
        exit(-1);
    }

    auto mode = prover_mode::from_str("ssim");

    std::string app_path = application_dir + mode.str() + "/";
    std::string params = app_path + "params";
    std::string pws = app_path + "pws";
    comp_params p = parse_params(params.c_str());

    std::ifstream pkey(pk_fn);
    if (!pkey.is_open()) {
        std::cerr << "ERROR: " << pk_fn << " not found." << std::endl;
        exit(-1);
    }

    libff::start_profiling();
    libsnark::r1cs_ppzksnark_proving_key<libsnark::default_r1cs_ppzksnark_pp> pk;
    libff::enter_block("Reading proving key from file");
    pkey >> pk;
    libff::leave_block("Reading proving key from file");

    libff::enter_block("Call to write_to_auxiliary_input");
    write_to_auxiliary_input("exo3", src1, src1_len, src2, src2_len, nullptr, 0);
    libff::leave_block("Call to write_to_auxiliary_input");

    libsnark::r1cs_ppzksnark_proof<libsnark::default_r1cs_ppzksnark_pp> proof;
    generate_proof_internal(p, pws.c_str(), pk, ref_ssim, proof);

    clear_auxiliary_input("exo3");

    if (proof_bin_fn != nullptr) {
        std::ofstream proof_file(proof_bin_fn);
        if (proof_file.is_open()) {
            proof_file << proof;
            proof_file.close();
        } else {
            std::cerr << "WARNING: unable to open file at path: " << proof_bin_fn << std::endl;
        }
    }

    if (proof_uncompressed_fn != nullptr) {
        print_proof_to_file(proof, proof_uncompressed_fn);
    }

    if (proof_json_fn != nullptr) {
        print_proof_to_json(proof, proof_json_fn);
    }
}

template<typename ppT>
void generate_proof_internal2(const comp_params &p,
                             const char *pws_fn,
                             libsnark::r1cs_ppzksnark_proving_key<ppT> &pk,
                             int ref_ssim,
                             const unsigned char *ref_digest, unsigned long digest_len,
                             libsnark::r1cs_ppzksnark_proof<ppT> &proof) {
    mpz_t prime;
    mpz_init_set_str(prime, prime_str, 10);

    std::vector<double> input;
    input.emplace_back(ref_ssim);
    for (int i = 0; i < digest_len; ++i) {
        input.emplace_back(ref_digest[i]);
    }

    libff::start_profiling();
    libff::enter_block("Compute algorithm");
    ComputationProver prover(p.n_vars, p.n_constraints, p.n_inputs, p.n_outputs, prime, input);
    prover.compute_from_pws(pws_fn);
    libff::leave_block("Compute algorithm");

    libsnark::r1cs_ppzksnark_primary_input<ppT> primary_input;
    libsnark::r1cs_ppzksnark_auxiliary_input<ppT> aux_input;

    for (int i = 0; i < p.n_inputs; i++) {
        FieldT currentVar(prover.input[i]);
        primary_input.push_back(currentVar);
    }
/*
    for (int i = 0; i < p.n_outputs - 1; i++) {
        FieldT currentVar(prover.output[i]);
        primary_input.push_back(currentVar);
    }
*/
    mpz_t accepted;
    alloc_init_scalar(accepted);
    mpz_set_ui(accepted, 1);
    primary_input.push_back(FieldT(accepted));

    mpz_t dummy;
    alloc_init_scalar(dummy);
    primary_input.push_back(FieldT(dummy));

    for (int i = 0; i < p.n_vars; i++) {
        FieldT currentVar(prover.F1[i]);
        aux_input.push_back(currentVar);
    }

    libff::start_profiling();
    proof = libsnark::r1cs_ppzksnark_prover<ppT>(pk, primary_input, aux_input);
}

template<typename ppT>
void generate_proof_internal(const comp_params &p,
                             const char *pws_fn,
                             libsnark::r1cs_ppzksnark_proving_key<ppT> &pk,
                             int ref_ssim,
                             libsnark::r1cs_ppzksnark_proof<ppT> &proof) {
    mpz_t prime;
    mpz_init_set_str(prime, prime_str, 10);

    std::vector<double> input;
    input.emplace_back(ref_ssim);

    libff::start_profiling();
    libff::enter_block("Compute algorithm");
    ComputationProver prover(p.n_vars, p.n_constraints, p.n_inputs, p.n_outputs, prime, input);
    prover.compute_from_pws(pws_fn);
    libff::leave_block("Compute algorithm");

    libsnark::r1cs_ppzksnark_primary_input<ppT> primary_input;
    libsnark::r1cs_ppzksnark_auxiliary_input<ppT> aux_input;

    for (int i = 0; i < p.n_inputs; i++) {
        FieldT currentVar(prover.input[i]);
        primary_input.push_back(currentVar);
    }
/*
    for (int i = 0; i < p.n_outputs - 1; i++) {
        FieldT currentVar(prover.output[i]);
        primary_input.push_back(currentVar);
    }
*/
    mpz_t accepted;
    alloc_init_scalar(accepted);
    mpz_set_ui(accepted, 1);
    primary_input.push_back(FieldT(accepted));

    mpz_t dummy;
    alloc_init_scalar(dummy);
    primary_input.push_back(FieldT(dummy));

    for (int i = 0; i < p.n_vars; i++) {
        FieldT currentVar(prover.F1[i]);
        aux_input.push_back(currentVar);
    }

    libff::start_profiling();
    proof = libsnark::r1cs_ppzksnark_prover<ppT>(pk, primary_input, aux_input);
}

void write_to_auxiliary_input(const char *exo_name, const unsigned char *src1, size_t len1,
                              const unsigned char *src2, size_t len2,
                              const unsigned char *digest, size_t digest_len) {
    std::string file = exo_dir + exo_name;
    std::ofstream aux(file);
    if (!aux.is_open()) {
        std::cerr << "ERROR: can't create file " << file << "." << std::endl;
        exit(1);
    }

    aux << "#!/bin/sh" << std::endl << std::endl << std::endl;
    for (size_t i = 0; i < len1; ++i) {
        aux << "echo " << (int) src1[i] << std::endl;
    }
    for (size_t i = 0; i < len2; ++i) {
        aux << "echo " << (int) src2[i] << std::endl;
    }
    if (digest != nullptr) {
        for (size_t i = 0; i < digest_len; ++i) {
            aux << "echo " << (int) digest[i] << std::endl;
        }
    }

    aux.close();
    chmod(file.c_str(), ALLPERMS);
}

void write_mb_to_auxiliary_input(const char *exo_name, const unsigned char *src1, size_t len1) {
    std::string file = exo_dir + exo_name;
    std::ofstream aux(file);
    if (!aux.is_open()) {
        std::cerr << "ERROR: can't create file " << file << "." << std::endl;
        exit(1);
    }

    aux << "#!/bin/sh" << std::endl << std::endl << std::endl;
    for (size_t i = 0; i < len1; ++i) {
        aux << "echo " << (int) src1[i] << std::endl;
    }

    aux.close();
    chmod(file.c_str(), ALLPERMS);
}

void write_mb_decoder_data_to_auxiliary_input(const char *exo_name, const mb_context_t *mb_context) {
    std::string file = exo_dir + exo_name;
    std::ofstream aux(file);
    if (!aux.is_open()) {
        std::cerr << "ERROR: can't create file " << file << "." << std::endl;
        exit(1);
    }

    aux << "#!/bin/sh" << std::endl << std::endl << std::endl;
    aux << "echo " << (int)mb_context->mb_x << std::endl;
    aux << "echo " << (int)mb_context->mb_y << std::endl;
    aux << "echo " << (int)mb_context->mb_width << std::endl;
    aux << "echo " << (int)mb_context->dequant_coeff << std::endl;
    aux << "echo " << (int)mb_context->mb_field_decoding_flag << std::endl;
    aux << "echo " << (int)mb_context->deblocking_filter << std::endl;
    aux << "echo " << (int)mb_context->intra16x16_pred_mode << std::endl;

    for (int j = 0; j < 16*16; ++j) {
        aux << "echo " << (int)mb_context->mb[j] << std::endl;
    }

    for (int j = 0; j < 16; ++j) {
        aux << "echo " << (int)mb_context->mb_luma_dc[j] << std::endl;
    }

    for (int j = 0; j < 15*8; ++j) {
        aux << "echo " << (int)mb_context->non_zero_count_cache[j] << std::endl;
    }

    for (int j = 0; j < 8+16+8; ++j) {
        aux << "echo " << (int)mb_context->top_border[j] << std::endl;
    }

    for (int j = 0; j < 8+16+8; ++j) {
        aux << "echo " << (int)mb_context->luma_top[j] << std::endl;
    }

    for (int j = 0; j < 16; ++j) {
        aux << "echo " << (int)mb_context->luma_left[j] << std::endl;
    }

    aux.close();
    chmod(file.c_str(), ALLPERMS);
}

void clear_auxiliary_input(const char *exo_name) {
    remove((exo_dir + exo_name).c_str());
}