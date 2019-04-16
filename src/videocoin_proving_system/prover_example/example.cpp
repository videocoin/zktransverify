//
// Created by taras on 15.04.19.
//

#include "prover/prover.h"

int main(int argc, char *argv[]) {
    initialize_prover();
    generate_ssim_proof("temp/p.key", "temp/inputs.txt", "temp/outputs.txt", "temp/ssim.proof");
}