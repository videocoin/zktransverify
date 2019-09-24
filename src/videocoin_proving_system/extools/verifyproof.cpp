#include <iostream>
#include <exception>

#include <boost/program_options.hpp>

#include <libsnark/relations/constraint_satisfaction_problems/r1cs/r1cs.hpp>
#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_ppzksnark/r1cs_ppzksnark.hpp>

#include <common/utility.h>
#include <common/defs.h>

#include "decode-h264-mb.h"

namespace po = boost::program_options;
using namespace std;

void verify(const string &verification_key_fn, const string &proof_fn,
            const string &video_fn, int ref_ssim, int num_inputs, mpz_t prime) {
    unsigned char srcRawY[256];

    libsnark::default_r1cs_ppzksnark_pp::init_public_params();

    libsnark::r1cs_variable_assignment<FieldT> input_vec;
    libsnark::r1cs_ppzksnark_proof<libsnark::default_r1cs_ppzksnark_pp> proof;

    cout << "loading proof from file: " << proof_fn << endl;
    ifstream proof_file(proof_fn);
    if (!proof_file.good()) {
        cerr << "ERROR: " << proof_fn << " not found. " << endl;
        exit(1);
    }
    proof_file >> proof;
    proof_file.close();

    cout << "loading inputs from file: " << video_fn << endl;
    stringstream inputs;

    memset(srcRawY, 0x00, 256);
    get_mb_from_stream(video_fn.c_str(), 1, 0, srcRawY, sizeof(srcRawY));
    inputs << ref_ssim << endl;
    for (unsigned char i : srcRawY) {
        inputs << (int)i << endl;
    }
    // constant values for SSIM circuit
    inputs << 1 << endl << 0 << endl;

    mpq_t tmp;
    mpq_init(tmp);
    mpz_t tmp_z;
    mpz_init(tmp_z);

    for (int i = 0; i < num_inputs; i++) {
        inputs >> tmp;

        convert_to_z(tmp_z, tmp, prime);
        FieldT currentVar(tmp_z);
        input_vec.push_back(currentVar);
    }

    mpq_clear(tmp);
    mpz_clear(tmp_z);

    cout << "loading vk from file: " << verification_key_fn << endl;
    ifstream vkey(verification_key_fn);
    libsnark::r1cs_ppzksnark_processed_verification_key<libsnark::default_r1cs_ppzksnark_pp> pvk;
    vkey >> pvk;
    vkey.close();

    cout << "verifying..." << endl;
    libff::start_profiling();
    bool result = libsnark::r1cs_ppzksnark_online_verifier_strong_IC<libsnark::default_r1cs_ppzksnark_pp>(pvk, input_vec,
                                                                                                          proof);
    cout << "VERIFICATION " << (result ? "SUCCESSFUL" : "FAILED") << endl;
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
                ("file,f", po::value<string>(), "path to video file")
                ("vkey,v", po::value<string>(), "path to verification key")
                ("proof,p", po::value<string>(), "path to proof")
                ("ssim-level,s", po::value<int>()->default_value(80), "threshold SSIM level [0-100]");

        all.add(general).add(verifier);

        po::store(po::parse_command_line(argc, argv, all), vm);
        po::notify(vm);

        if (vm.count("help")) {
            cout << all << endl;
            exit(0);
        }

        // check mandatory options
        for (auto &e: {"vkey", "proof", "file"}) {
            if (!vm.count(e)) {
                cerr << "error: the option '--" << e << "' is required but missing\n" << all << endl;
                exit(1);
            }
        }

        initialize_env();
        string params = application_dir + "ssim/params";
        struct comp_params p = parse_params(params.c_str());

        mpz_t prime;
        mpz_init_set_str(prime, prime_str, 10);

        string verification_key_fn = argv[2];
        string inputs_fn = argv[3];
        string proof_fn = argv[4];
        verify(vm["vkey"].as<string>(), vm["proof"].as<string>(),
               vm["file"].as<string>(), vm["ssim-level"].as<int>(),
               p.n_inputs + p.n_outputs, prime);
    }
    catch (exception &e) {
        cerr << "error: " << e.what() << "\n";
        exit(1);
    }
    catch (...) {
        cerr << "Exception of unknown type!\n";
        exit(1);
    }

    return 0;
}
