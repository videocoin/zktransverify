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

void generate_proof(const char *application, const char *pk_fn, const char *input_fn, const char *output_fn, const char *proof_fn) {
    std::string app_path = std::string("./apps/") + std::string(application) + "/";
    std::string params = app_path + "params";
    struct comp_params p = parse_params(params.c_str());

    mpz_t prime;
    mpz_init_set_str(prime, prime_str, 10);

    ComputationProver prover(p.n_vars, p.n_constraints, p.n_inputs, p.n_outputs, prime, "default_shared_db", input_fn, false);
    std::string prover_worker_sheet = app_path + "pws";
    prover.compute_from_pws(prover_worker_sheet.c_str());

    libsnark::default_r1cs_ppzksnark_pp::init_public_params();

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

    std::ifstream pkey(pk_fn);
    if (!pkey.is_open()) {
        std::cerr << "ERROR: " << pk_fn << " not found." << std::endl;
    }

    libsnark::r1cs_ppzksnark_keypair<libsnark::default_r1cs_ppzksnark_pp> keypair;
    std::cout << "reading proving key from file..." << std::endl;
    pkey >> keypair.pk;

    libff::start_profiling();
    libsnark::r1cs_ppzksnark_proof<libsnark::default_r1cs_ppzksnark_pp> proof = libsnark::r1cs_ppzksnark_prover<libsnark::default_r1cs_ppzksnark_pp>(keypair.pk, primary_input, aux_input);

    std::ofstream proof_file(proof_fn);
    proof_file << proof;
    proof_file.close();

    std::ofstream output_file(output_fn);
    for (int i = 0; i < p.n_outputs; i++) {
        output_file << prover.input_output_q[p.n_inputs + i] << std::endl;
    }
    output_file.close();
}