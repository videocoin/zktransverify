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

static void generate_proof_internal(const comp_params &p,
                                    const char *pws_fn,
                                    libsnark::r1cs_ppzksnark_proving_key<libsnark::default_r1cs_ppzksnark_pp> &pk,
                                    const std::vector<double> &input,
                                    std::vector<double> &output,
                                    libsnark::r1cs_ppzksnark_proof<libsnark::default_r1cs_ppzksnark_pp> &proof);

static void write_to_auxilary_input(const unsigned char *src1, size_t len1, const unsigned char *src2, size_t len2);
static void clear_auxilary_input();

void initialize_prover() {
    initialize_env();
    libsnark::default_r1cs_ppzksnark_pp::init_public_params();
}

double generate_ssim_proof(const char *pk_fn,
                           const unsigned char *src1, unsigned long src1_len,
                           const unsigned char *src2, unsigned long src2_len,
                           const char *output_fn, const char *proof_fn,
                           const char *json_fn) {
    if (src1 == nullptr || src2 == nullptr) {
        std::cerr << "ERROR: invalid source." << std::endl;
        exit(-1);
    }

    if (src1_len != src2_len) {
        std::cerr << "ERROR: src1 and src2 should be same length." << std::endl;
        exit(-1);
    }

    auto mode = ssim_mode::from_int(sqrt(src1_len));

    std::string app_path = application_dir + mode.str() + "/";
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

    write_to_auxilary_input(src1, src1_len, src2, src2_len);
    input.emplace_back(src1[0]);

    libsnark::r1cs_ppzksnark_proof<libsnark::default_r1cs_ppzksnark_pp> proof;

    generate_proof_internal(p, pws.c_str(), pk, input, output, proof);

    print_proof_to_file(proof, proof_fn);

    std::ofstream proof_file(proof_fn);
    proof_file << proof;
    proof_file.close();

    std::ofstream output_file(output_fn);
    for (int i = 0; i < p.n_inputs; i++) {
        output_file << (unsigned) input[i] << std::endl;
    }
    for (int i = 0; i < p.n_outputs; i++) {
        output_file << (unsigned) output[i] << std::endl;
    }
    output_file.close();

    clear_auxilary_input();

    if (json_fn != nullptr) {
        pt::ptree root;
        input.insert(input.end(), output.begin(), output.end());

        root.add_child("proof", proof_to_ptree<libsnark::default_r1cs_ppzksnark_pp>(proof));
        root.add_child("inputs", input_to_ptree<unsigned>(input));

        std::ofstream proof_data(json_fn);
        pt::write_json(proof_data, root);
        proof_data.close();
    }

    double ssim;
    ssim = output[1] / 0x10000;
    ssim /= output[2];
    return ssim;
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

void write_to_auxilary_input(const unsigned char *src1, size_t len1, const unsigned char *src2, size_t len2) {
    std::string file = exo_dir + "exo3";
    std::ofstream aux(file);
    if (!aux.is_open()) {
        std::cerr << "ERROR: can't create file " << file << "." << std::endl;
        exit(1);
    }

    aux << "#!/bin/sh" << std::endl << std::endl << std::endl;
    for (size_t i = 0; i < len1; ++i) {
        aux << "echo " << (int)src1[i] << std::endl;
    }
    for (size_t i = 0; i < len2; ++i) {
        aux << "echo " << (int)src2[i] << std::endl;
    }

    aux.close();
    chmod(file.c_str(), ALLPERMS);
}

void clear_auxilary_input() {
    remove((exo_dir + "exo3").c_str());
}