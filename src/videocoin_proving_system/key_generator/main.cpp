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


void generate_keys(std::string &app_path,
                   int num_constraints, int num_inputs,
                   int num_outputs, int num_vars, mpz_t p,
                   po::variables_map &vm) {

    std::ifstream Amat(app_path + "matrix_a");
    std::ifstream Bmat(app_path + "matrix_b");
    std::ifstream Cmat(app_path + "matrix_c");

    libsnark::default_r1cs_ppzksnark_pp::init_public_params();
    libsnark::r1cs_constraint_system<FieldT> q;

    int Ai, Aj, Bi, Bj, Ci, Cj;
    mpz_t Acoef, Bcoef, Ccoef;
    mpz_init(Acoef);
    mpz_init(Bcoef);
    mpz_init(Ccoef);

    Amat >> Ai;
    Amat >> Aj;
    Amat >> Acoef;

    if (mpz_cmpabs(Acoef, p) > 0) {
        gmp_printf("WARNING: Coefficient larger than prime (%Zd > %Zd).\n", Acoef, p);
        mpz_mod(Acoef, Acoef, p);
    }
    if (mpz_sgn(Acoef) == -1) {
        mpz_add(Acoef, p, Acoef);
    }

    //    std::cout << Ai << " " << Aj << " " << Acoef << std::std::endl;

    Bmat >> Bi;
    Bmat >> Bj;
    Bmat >> Bcoef;
    if (mpz_cmpabs(Bcoef, p) > 0) {
        gmp_printf("WARNING: Coefficient larger than prime (%Zd > %Zd).\n", Bcoef, p);
        mpz_mod(Bcoef, Bcoef, p);
    }
    if (mpz_sgn(Bcoef) == -1) {
        mpz_add(Bcoef, p, Bcoef);
    }

    Cmat >> Ci;
    Cmat >> Cj;
    Cmat >> Ccoef;

    if (mpz_cmpabs(Ccoef, p) > 0) {
        gmp_printf("WARNING: Coefficient larger than prime (%Zd > %Zd).\n", Ccoef, p);
        mpz_mod(Ccoef, Ccoef, p);
    }
    if (mpz_sgn(Ccoef) == -1) {
        mpz_mul_si(Ccoef, Ccoef, -1);
    } else if (mpz_sgn(Ccoef) == 1) {
        mpz_mul_si(Ccoef, Ccoef, -1);
        mpz_add(Ccoef, p, Ccoef);
    }

    int num_intermediate_vars = num_vars;
    int num_inputs_outputs = num_inputs + num_outputs;

    q.primary_input_size = num_inputs_outputs;
    q.auxiliary_input_size = num_intermediate_vars;

    for (int currentconstraint = 1; currentconstraint <= num_constraints; currentconstraint++) {
        libsnark::linear_combination<FieldT> A, B, C;

        while (Aj == currentconstraint && Amat) {
            if (Ai <= num_intermediate_vars && Ai != 0) {
                Ai += num_inputs_outputs;
            } else if (Ai > num_intermediate_vars) {
                Ai -= num_intermediate_vars;
            }

            FieldT AcoefT(Acoef);
            A.add_term(Ai, AcoefT);
            if (!Amat) {
                break;
            }
            Amat >> Ai;
            Amat >> Aj;
            Amat >> Acoef;
            if (mpz_cmpabs(Acoef, p) > 0) {
                gmp_printf("WARNING: Coefficient larger than prime (%Zd > %Zd).\n", Acoef, p);
                mpz_mod(Acoef, Acoef, p);
            }
            if (mpz_sgn(Acoef) == -1) {
                mpz_add(Acoef, p, Acoef);
            }
        }

        while (Bj == currentconstraint && Bmat) {
            if (Bi <= num_intermediate_vars && Bi != 0) {
                Bi += num_inputs_outputs;
            } else if (Bi > num_intermediate_vars) {
                Bi -= num_intermediate_vars;
            }
            //         std::cout << Bi << " " << Bj << " " << Bcoef << std::std::endl;
            FieldT BcoefT(Bcoef);
            B.add_term(Bi, BcoefT);
            if (!Bmat) {
                break;
            }
            Bmat >> Bi;
            Bmat >> Bj;
            Bmat >> Bcoef;
            if (mpz_cmpabs(Bcoef, p) > 0) {
                gmp_printf("WARNING: Coefficient larger than prime (%Zd > %Zd).\n", Bcoef, p);
                mpz_mod(Bcoef, Bcoef, p);
            }
            if (mpz_sgn(Bcoef) == -1) {
                mpz_add(Bcoef, p, Bcoef);
            }
        }

        while (Cj == currentconstraint && Cmat) {
            if (Ci <= num_intermediate_vars && Ci != 0) {
                Ci += num_inputs_outputs;
            } else if (Ci > num_intermediate_vars) {
                Ci -= num_intermediate_vars;
            }
            //Libsnark constraints are A*B = C, vs. A*B - C = 0 for Zaatar.
            //Which is why the C coefficient is negated.

            // std::cout << Ci << " " << Cj << " " << Ccoef << std::std::endl;
            FieldT CcoefT(Ccoef);
            C.add_term(Ci, CcoefT);
            if (!Cmat) {
                break;
            }
            Cmat >> Ci;
            Cmat >> Cj;
            Cmat >> Ccoef;
            if (mpz_cmpabs(Ccoef, p) > 0) {
                gmp_printf("WARNING: Coefficient larger than prime (%Zd > %Zd).\n", Ccoef, p);
                mpz_mod(Ccoef, Ccoef, p);
            }
            if (mpz_sgn(Ccoef) == -1) {
                mpz_mul_si(Ccoef, Ccoef, -1);
            } else if (mpz_sgn(Ccoef) == 1) {
                mpz_mul_si(Ccoef, Ccoef, -1);
                mpz_add(Ccoef, p, Ccoef);
            }
        }

        q.add_constraint(libsnark::r1cs_constraint<FieldT>(A, B, C));
    }

    Amat.close();
    Bmat.close();
    Cmat.close();

    libff::start_profiling();
    libsnark::r1cs_ppzksnark_keypair<libsnark::default_r1cs_ppzksnark_pp> keypair = libsnark::r1cs_ppzksnark_generator<libsnark::default_r1cs_ppzksnark_pp>(
            q);
    libsnark::r1cs_ppzksnark_processed_verification_key<libsnark::default_r1cs_ppzksnark_pp> pvk = libsnark::r1cs_ppzksnark_verifier_process_vk<libsnark::default_r1cs_ppzksnark_pp>(
            keypair.vk);


    std::ofstream vkey(vm["vkey"].as<std::string>());
    std::ofstream pkey(vm["pkey"].as<std::string>());

    vkey << pvk;
    pkey << keypair.pk;
    pkey.close();
    vkey.close();

    if (vm.count("uncompressed-vkey")) {
        print_vk_to_file(keypair.vk, vm["uncompressed-vkey"].as<std::string>());
    }

    if (vm.count("json-vkey")) {
        print_vk_to_json(keypair.vk, vm["json-vkey"].as<std::string>());
    }
}

int main(int argc, char *argv[]) {
    try {
        po::options_description general("General options");
        po::options_description generator("Generator options");
        po::options_description all;
        po::variables_map vm;

        general.add_options()
                ("help,h", "produce help message");

        generator.add_options()
                ("mode,m", po::value<std::string>(),
                 "set algorithm type <ssim16x16 | ssim32x32 | ssim64x64>")
                ("vkey,v", po::value<std::string>(), "path to verification key")
                ("pkey,p", po::value<std::string>(), "path to proving key")
                ("uncompressed-vkey,u", po::value<std::string>(), "path to uncompressed verification key")
                ("json-vkey,j", po::value<std::string>(), "path to json verification key");

        all.add(general).add(generator);

        po::store(po::parse_command_line(argc, argv, all), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << all << std::endl;
            exit(0);
        }

        // check mandatory options
        for (auto &e: {"mode", "vkey", "pkey"}) {
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
        std::string app_path = application_dir + mode.str() + "/";
        std::string params = app_path + "params";
        comp_params p = parse_params(params.c_str());

        mpz_t prime;
        mpz_init_set_str(prime, prime_str, 10);

        std::cout << "Creating proving/verification keys\n";
        generate_keys(app_path, p.n_constraints, p.n_inputs, p.n_outputs, p.n_vars, prime, vm);
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
