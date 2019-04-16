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
              << " <ssim | mb16x16> <verification key file> <proving key file> [<unprocessed verification key file>]"
              << std::endl;
}

void generate_keys(std::string &app_path,
                   int num_constraints, int num_inputs,
                   int num_outputs, int num_vars, mpz_t p,
                   std::string &vkey_file, std::string &pkey_file,
                   std::string &unprocessed_vkey_file) {

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


    std::ofstream vkey(vkey_file);
    std::ofstream pkey(pkey_file);

    vkey << pvk;
    pkey << keypair.pk;
    pkey.close();
    vkey.close();

    if (unprocessed_vkey_file.length() > 0) {
        std::ofstream unprocessed_vkey(unprocessed_vkey_file);
        unprocessed_vkey << keypair.vk;
        unprocessed_vkey.close();
    }
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        print_usage(argv);
        exit(1);
    }

    std::string app_path = std::string("./apps/") + std::string(argv[1]) + "/";
    std::string params = app_path + "params";
    struct comp_params p = parse_params(params.c_str());

    mpz_t prime;
    mpz_init_set_str(prime, prime_str, 10);

    if (!strcmp(argv[1], "ssim") || !strcmp(argv[1], "mb16x16")) {
        if (argc != 4 && argc != 5) {
            print_usage(argv);
            exit(1);
        }
        std::string verification_key_fn = std::string(argv[2]);
        std::string proving_key_fn = std::string(argv[3]);
        std::string unprocessed_vkey_fn;
        if (argc == 5) {
            unprocessed_vkey_fn = std::string(argv[4]);
        }
        std::cout << "Creating proving/verification keys, will write to " << verification_key_fn
                  << ", " << proving_key_fn << std::endl;
        generate_keys(app_path, p.n_constraints, p.n_inputs, p.n_outputs, p.n_vars, prime, verification_key_fn,
                      proving_key_fn, unprocessed_vkey_fn);
    } else {
        print_usage(argv);
        exit(1);
    }
}
