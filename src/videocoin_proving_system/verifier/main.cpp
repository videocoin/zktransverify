#include <iostream>
#include <fstream>

#include <libsnark/relations/constraint_satisfaction_problems/r1cs/r1cs.hpp>
#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_ppzksnark/r1cs_ppzksnark.hpp>

#include <common/utility.h>
#include <common/defs.h>

void print_usage(char *argv[]) {
    std::cout << "usage: " << std::endl
              << argv[0]
              << " <ssim16x16 | mb16x16> <verification key file> <inputs file> <outputs file> <proof file>"
              << std::endl;
}

void verify (std::string &verification_key_fn, std::string &inputs_fn, std::string &outputs_fn,
             std::string &proof_fn, int num_inputs, int num_outputs, mpz_t prime) {

    libsnark::default_r1cs_ppzksnark_pp::init_public_params();

    libsnark::r1cs_variable_assignment<FieldT> inputvec;
    libsnark::r1cs_ppzksnark_proof<libsnark::default_r1cs_ppzksnark_pp> proof;

    std::cout << "loading proof from file: " << proof_fn << std::endl;
    std::ifstream proof_file(proof_fn);
    if (!proof_file.good()) {
        std::cerr << "ERROR: " << proof_fn << " not found. " << std::endl;
        exit(1);
    }
    proof_file >> proof;
    proof_file.close();

    std::cout << "loading inputs from file: " << inputs_fn << std::endl;
    std::ifstream inputs_file(inputs_fn);

    std::cout << "loading outputs from file: " << outputs_fn << std::endl;
    std::ifstream outputs_file(outputs_fn);

    mpq_t tmp; mpq_init(tmp);
    mpz_t tmp_z; mpz_init(tmp_z);

    for (int i = 0; i < num_inputs; i++) {
        inputs_file >> tmp;
        convert_to_z(tmp_z, tmp, prime);
        FieldT currentVar(tmp_z);
        inputvec.push_back(currentVar);
    }

    for (int i = 0; i < num_outputs; i++) {
        outputs_file >> tmp;
        convert_to_z(tmp_z, tmp, prime);
        FieldT currentVar(tmp_z);
        inputvec.push_back(currentVar);
    }

    mpq_clear(tmp); mpz_clear(tmp_z);

    inputs_file.close();
    outputs_file.close();

    std::cout << "loading vk from file: " << verification_key_fn << std::endl;
    std::ifstream vkey(verification_key_fn);
    libsnark::r1cs_ppzksnark_processed_verification_key<libsnark::default_r1cs_ppzksnark_pp> pvk;
    vkey >> pvk;
    vkey.close();

    std::cout << "verifying..." << std::endl;
    libff::start_profiling();
    bool result = libsnark::r1cs_ppzksnark_online_verifier_strong_IC<libsnark::default_r1cs_ppzksnark_pp>(pvk, inputvec, proof);
    std::cout << "VERIFICATION " << (result ? "SUCCESSFUL": "FAILED") << std::endl;
}

int main(int argc, char *argv[]) {
    if (argc != 6) {
        print_usage(argv);
        exit(1);
    }

    ssim_mode mode = ssim_mode::from_str(argv[1]);

    initialize_env();

    std::string params = application_dir + mode.str() + "/params";
    struct comp_params p = parse_params(params.c_str());

    mpz_t prime;
    mpz_init_set_str(prime, prime_str, 10);

    if (mode.is_valid() || !strcmp(argv[1], "mb16x16")) {
        std::string verification_key_fn = argv[2];
        std::string inputs_fn = argv[3];
        std::string outputs_fn = argv[4];
        std::string proof_fn = argv[5];
        verify(verification_key_fn, inputs_fn, outputs_fn, proof_fn, p.n_inputs, p.n_outputs, prime);
    } else {
        print_usage(argv);
        exit(1);
    }
}
