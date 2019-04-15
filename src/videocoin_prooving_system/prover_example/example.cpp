//
// Created by taras on 15.04.19.
//

#include "prover/prover.h"

int main(int argc, char *argv[]) {
    generate_proof("ssim", "temp/p.key", "temp/inputs.txt", "outputs.txt", "ssim.proof");
}