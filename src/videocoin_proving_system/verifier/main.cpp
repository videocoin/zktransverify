#include <iostream>
#include <fstream>
#include <exception>

#include <boost/program_options.hpp>

#include <libsnark/relations/constraint_satisfaction_problems/r1cs/r1cs.hpp>
#include <libsnark/common/default_types/r1cs_gg_ppzksnark_pp.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_gg_ppzksnark/r1cs_gg_ppzksnark.hpp>

#include <common/utility.h>
#include <common/defs.h>

namespace po = boost::program_options;

template <typename ppT>
void verify(const std::string &verification_key_fn, const std::string &inputs_fn,
            const std::string &proof_fn, int num_inputs, mpz_t prime) {

    ppT::init_public_params();

    libsnark::r1cs_variable_assignment<FieldT> inputvec;
    libsnark::r1cs_gg_ppzksnark_proof<ppT> proof;

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
    libsnark::r1cs_gg_ppzksnark_processed_verification_key<ppT> pvk;
    vkey >> pvk;
    vkey.close();

    std::cout << "verifying..." << std::endl;
    libff::start_profiling();
    bool result = libsnark::r1cs_gg_ppzksnark_online_verifier_strong_IC<ppT>(pvk, inputvec, proof);
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
                ("mode,m", po::value<std::string>(),
                 "set algorithm type <ssim16x16 | ssim32x32 | ssim64x64>")
                ("vkey,v", po::value<std::string>(), "path to verification key")
                ("proof,p", po::value<std::string>(), "path to proof")
                ("input-output,i", po::value<std::string>(), "path to input-output file");

        all.add(general).add(verifier);

        po::store(po::parse_command_line(argc, argv, all), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << all << std::endl;
            exit(0);
        }

        // check mandatory options
        for (auto &e: {"mode", "vkey", "proof", "input-output"}) {
            if (!vm.count(e)) {
                std::cerr << "error: the option '--" << e << "' is required but missing\n" << all << std::endl;
                exit(1);
            }
        }

        ssim_mode mode = ssim_mode::from_str(vm["mode"].as<std::string>().c_str());
        if (!mode.is_valid()) {
            std::cerr << "error: the option '--mode' has invalid value: " << vm["mode"].as<std::string>() << std::endl
                      << all << std::endl;
            exit(1);
        }

        initialize_env();
        std::string params = application_dir + mode.str() + "/params";
        struct comp_params p = parse_params(params.c_str());

        mpz_t prime;
        mpz_init_set_str(prime, prime_str, 10);

        std::string verification_key_fn = argv[2];
        std::string inputs_fn = argv[3];
        std::string proof_fn = argv[4];
        verify<libsnark::default_r1cs_gg_ppzksnark_pp>(vm["vkey"].as<std::string>(), vm["input-output"].as<std::string>(), vm["proof"].as<std::string>(),
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
