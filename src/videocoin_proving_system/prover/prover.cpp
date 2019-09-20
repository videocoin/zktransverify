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
                            const unsigned char *src1, size_t len,
                            libsnark::r1cs_ppzksnark_proof<ppT> &proof);

static void write_to_auxiliary_input(const unsigned char *src, size_t len);

static void clear_auxiliary_input();

void initialize_prover() {
    initialize_env();
    libsnark::default_r1cs_ppzksnark_pp::init_public_params();
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

    std::string app_path = application_dir + "ssim/";
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
    write_to_auxiliary_input(src2, src2_len);
    libff::leave_block("Call to write_to_auxiliary_input");

    libsnark::r1cs_ppzksnark_proof<libsnark::default_r1cs_ppzksnark_pp> proof;
    generate_proof_internal(p, pws.c_str(), pk, ref_ssim, src1, src1_len, proof);

    clear_auxiliary_input();

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
void generate_proof_internal(const comp_params &p,
                            const char *pws_fn,
                            libsnark::r1cs_ppzksnark_proving_key<ppT> &pk,
                            int ref_ssim,
                            const unsigned char *src, size_t len,
                            libsnark::r1cs_ppzksnark_proof<ppT> &proof) {
    mpz_t prime;
    mpz_init_set_str(prime, prime_str, 10);

    std::vector<double> input;
    input.emplace_back(ref_ssim);
    for (int i = 0; i < len; ++i) {
        input.emplace_back(src[i]);
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

void write_to_auxiliary_input(const unsigned char *src, size_t len) {
    std::string file = exo_dir + "exo0";
    std::ofstream aux(file);
    if (!aux.is_open()) {
        std::cerr << "ERROR: can't create file " << file << "." << std::endl;
        exit(1);
    }

    aux << "#!/bin/sh" << std::endl << std::endl << std::endl;
    for (size_t i = 0; i < len; ++i) {
        aux << "echo " << (int) src[i] << std::endl;
    }

    aux.close();
    chmod(file.c_str(), ALLPERMS);
}

void clear_auxiliary_input() {
    remove((exo_dir + "exo0").c_str());
}