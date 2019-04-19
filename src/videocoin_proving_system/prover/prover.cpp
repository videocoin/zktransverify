//
// Created by taras on 12.04.19.
//

#include <string>
#include <iostream>
#include <fstream>

#include <libsnark/relations/constraint_satisfaction_problems/r1cs/r1cs.hpp>
#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_ppzksnark/r1cs_ppzksnark.hpp>

#include <common/utility.h>
#include <common/defs.h>

#include "computation_prover.h"
#include "prover.h"

static std::string ssim_mode_to_str(ssim_mode c);

static void generate_proof_internal(const comp_params &p,
                                    const char *pws_fn,
                                    libsnark::r1cs_ppzksnark_proving_key<libsnark::default_r1cs_ppzksnark_pp> &pk,
                                    const std::vector<double> &input,
                                    std::vector<double> &output,
                                    libsnark::r1cs_ppzksnark_proof<libsnark::default_r1cs_ppzksnark_pp> &proof);

void initialize_prover() {
    initialize_env();
    libsnark::default_r1cs_ppzksnark_pp::init_public_params();
}

void generate_ssim_proof(ssim_mode mode, const char *pk_fn,
                         const unsigned char *src_luma1,
                         const unsigned char *src_luma2,
                         const char *output_fn, const char *proof_fn, double &ssim) {
    std::string app_path = application_dir + ssim_mode_to_str(mode) + "/";
    std::string params = app_path + "params";
    std::string pws = app_path + "pws";
    comp_params p = parse_params(params.c_str());

    std::ifstream pkey(pk_fn);
    if (!pkey.is_open()) {
        std::cerr << "ERROR: " << pk_fn << " not found." << std::endl;
        exit(-1);
    }

    libsnark::r1cs_ppzksnark_proving_key<libsnark::default_r1cs_ppzksnark_pp> pk;
    std::cout << "reading proving key from file..." << std::endl;
    pkey >> pk;

    std::vector<double> input;
    std::vector<double> output;

    for (int i = 0; i < p.n_inputs / 2; ++i) {
        input.emplace_back(src_luma1[i]);
    }
    for (int i = 0; i < p.n_inputs / 2; ++i) {
        input.emplace_back(src_luma2[i]);
    }

    libsnark::r1cs_ppzksnark_proof<libsnark::default_r1cs_ppzksnark_pp> proof;

    generate_proof_internal(p, pws.c_str(), pk, input, output, proof);

    std::ofstream proof_file(proof_fn);
    proof_file << proof;
    proof_file.close();

    std::ofstream output_file(output_fn);
    for (int i = 0; i < p.n_outputs; i++) {
        output_file << (unsigned) output[i] << std::endl;
    }
    output_file.close();

    ssim = output[1] / 0x10000;
    ssim /= output[2];
}

void generate_proof_internal(const comp_params &p,
                             const char *pws_fn,
                             libsnark::r1cs_ppzksnark_proving_key<libsnark::default_r1cs_ppzksnark_pp> &pk,
                             const std::vector<double> &input,
                             std::vector<double> &output,
                             libsnark::r1cs_ppzksnark_proof<libsnark::default_r1cs_ppzksnark_pp> &proof) {
    mpz_t prime;
    mpz_init_set_str(prime, prime_str, 10);

    ComputationProver prover(p.n_vars, p.n_constraints, p.n_inputs, p.n_outputs, prime, input);
    prover.compute_from_pws(pws_fn);

    libsnark::r1cs_ppzksnark_primary_input<libsnark::default_r1cs_ppzksnark_pp> primary_input;
    libsnark::r1cs_ppzksnark_auxiliary_input<libsnark::default_r1cs_ppzksnark_pp> aux_input;

    for (int i = 0; i < p.n_inputs; i++) {
        FieldT currentVar(prover.input[i]);
        primary_input.push_back(currentVar);
    }

    for (int i = 0; i < p.n_outputs; i++) {
        FieldT currentVar(prover.output[i]);
        primary_input.push_back(currentVar);
    }

    for (int i = 0; i < p.n_vars; i++) {
        FieldT currentVar(prover.F1[i]);
        aux_input.push_back(currentVar);
    }

    libff::start_profiling();
    proof = libsnark::r1cs_ppzksnark_prover<libsnark::default_r1cs_ppzksnark_pp>(
            pk, primary_input, aux_input);


    for (int i = 0; i < p.n_outputs; i++) {
        output.emplace_back(mpq_get_d(prover.input_output_q[p.n_inputs + i]));
    }
}

std::string ssim_mode_to_str(ssim_mode m) {
    switch (m) {
        case x16   :
            return "ssim16x16";
        case x32   :
            return "ssim32x32";
        case x64   :
            return "ssim64x64";
    }
}