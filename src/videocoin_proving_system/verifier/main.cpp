#include <iostream>
#include <fstream>
#include <exception>

#include <boost/program_options.hpp>

#include <libsnark/relations/constraint_satisfaction_problems/r1cs/r1cs.hpp>
#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_ppzksnark/r1cs_ppzksnark.hpp>

#include <common/utility.h>
#include <common/defs.h>

namespace po = boost::program_options;

void verify(const std::string &verification_key_fn, const std::string &inputs_fn,
            const std::string &proof_fn, int num_inputs, mpz_t prime) {

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

    mpq_t tmp;
    mpq_init(tmp);
    mpz_t tmp_z;
    mpz_init(tmp_z);

    for (int i = 0; i < num_inputs; i++) {
        inputs_file >> tmp;
        convert_to_z(tmp_z, tmp, prime);
        FieldT currentVar(tmp_z);
        inputvec.push_back(currentVar);
    }

    mpq_clear(tmp);
    mpz_clear(tmp_z);

    inputs_file.close();

    std::cout << "loading vk from file: " << verification_key_fn << std::endl;
    std::ifstream vkey(verification_key_fn);
    libsnark::r1cs_ppzksnark_processed_verification_key<libsnark::default_r1cs_ppzksnark_pp> pvk;
    vkey >> pvk;
    vkey.close();

    std::cout << "verifying..." << std::endl;
    libff::start_profiling();
    bool result = libsnark::r1cs_ppzksnark_online_verifier_strong_IC<libsnark::default_r1cs_ppzksnark_pp>(pvk, inputvec,
                                                                                                          proof);
    std::cout << "VERIFICATION " << (result ? "SUCCESSFUL" : "FAILED") << std::endl;
}

int main(int argc, char *argv[]) {
    try {
        po::options_description general("General options");
        po::options_description verifier("Verifier options");
        po::options_description all;
        po::variables_map vm;

        general.add_options()
                ("help,h", "produce help message");

        verifier.add_options()
                ("vkey,v", po::value<std::string>(), "path to verification key")
                ("proof,p", po::value<std::string>(), "path to proof")
                ("witness,w", po::value<std::string>(), "path to witness file");

        all.add(general).add(verifier);

        po::store(po::parse_command_line(argc, argv, all), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << all << std::endl;
            exit(0);
        }

        // check mandatory options
        for (auto &e: {"vkey", "proof", "witness"}) {
            if (!vm.count(e)) {
                std::cerr << "error: the option '--" << e << "' is required but missing\n" << all << std::endl;
                exit(1);
            }
        }

        initialize_env();
        std::string params = application_dir + "ssim/params";
        struct comp_params p = parse_params(params.c_str());

        mpz_t prime;
        mpz_init_set_str(prime, prime_str, 10);

        std::string verification_key_fn = argv[2];
        std::string inputs_fn = argv[3];
        std::string proof_fn = argv[4];
        verify(vm["vkey"].as<std::string>(), vm["witness"].as<std::string>(), vm["proof"].as<std::string>(),
               p.n_inputs + p.n_outputs, prime);
    }
    catch (std::exception &e) {
        std::cerr << "error: " << e.what() << "\n";
        exit(1);
    }
    catch (...) {
        std::cerr << "Exception of unknown type!\n";
        exit(1);
    }

    return 0;
}
