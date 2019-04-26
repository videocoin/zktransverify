//
// Created by taras on 12.04.19.
//

#ifndef DEFS_H
#define DEFS_H

typedef libff::Fr<libsnark::default_r1cs_ppzksnark_pp> FieldT;

const char *prime_str = "21888242871839275222246405745257275088548364400416034343698204186575808495617";

std::string exo_dir;
std::string application_dir;

void initialize_env() {
    if (const char *env_p = std::getenv("VIDEOCOIN_ENV")) {
        application_dir = exo_dir = env_p;
        application_dir += "/apps/";
        exo_dir += "/exo/";
        mkdir(exo_dir.c_str(), S_IRWXU);
    } else {
        std::cerr << "ERROR: Environment variable VIDEOCOIN_ENV is not set" << std::endl;
        exit(-1);
    }
}

#endif //DEFS_H
