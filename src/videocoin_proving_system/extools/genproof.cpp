#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <exception>
#include <iostream>

#include <boost/program_options.hpp>

#include <prover/prover.h>

#include "decode-h264-mb.h"
#include "sha256-util.h"
#include "sha256-circuit.h"

namespace po = boost::program_options;

std::vector<std::string> files;
std::string proving_key_path;
std::string proof;
std::string uncompressed_proof;
std::string json_proof;
std::string witness;
bool verbose = false;
int ref_ssim;


void parse_options(int argc, const char *argv[]);

void save_witness(const char *filename, int refssim);

void parse_options(int argc, const char *argv[]) {
    try {
        po::options_description general("General options");
        po::options_description prover("Prover options");
        po::options_description all;
        po::variables_map vm;

        general.add_options()
                ("help,h", "produce help message")
                ("verbose,v", "print additional information");

        prover.add_options()
                ("pkey,p", po::value<std::string>(&proving_key_path), "path to proving key")
                ("files,f", po::value<std::vector<std::string>>(&files)->multitoken(), "list of %file1, %file2")
                ("proof,P", po::value<std::string>(&proof), "path to proof file")
                ("uncompressed-proof,u", po::value<std::string>(&uncompressed_proof), "path to uncompressed proof file")
                ("json-proof,j", po::value<std::string>(&json_proof), "path to json proof file")
                ("witness,w", po::value<std::string>(&witness), "path to witness file")
                ("ssim-level,s", po::value<int>(&ref_ssim), "threshold ssim level [0-100]");

        all.add(general).add(prover);

        po::store(po::parse_command_line(argc, argv, all), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << all << std::endl;
            exit(0);
        }

        if (vm.count("verbose")) {
            verbose = true;
        }

        // check mandatory options
        for (auto &e: {"files", "pkey", "ssim-level", "witness"}) {
            if (!vm.count(e)) {
                std::cerr << "error: the option '--" << e << "' is required but missing\n" << all << std::endl;
                exit(1);
            }
        }

        if (files.size() != 2) {
            std::cerr << "error: the option '--files' has invalid number files\n"
                      << all << std::endl;
            exit(1);
        }
    }
    catch (std::exception &e) {
        std::cerr << "error: " << e.what() << "\n";
        exit(1);
    }
    catch (...) {
        std::cerr << "Exception of unknown type!\n";
        exit(1);
    }
}

void save_witness(const char *filename, int refssim) {
    FILE *f = fopen(filename, "w+");
    if (f != nullptr) {
        fprintf(f, "%d\n%d\n%d\n", refssim, 1, 0);
        fclose(f);
    }
}

void test_sha256_circuit(unsigned char *data, unsigned int size) {
    unsigned char sha[32];
    struct In input;
    struct Out output;

    sha256_bytes(data, size, sha);

    memcpy(input.mb, data, size);
    memcpy(input.hash, sha, sizeof(sha));
    compute(&input, &output);

    printf("Identical: %s\n", output.accepted ? "true" : "false");
}

int main(int argc, const char *argv[]) {
    MB_T mbSrc = {0};
    MB_T mbTrans = {0};
    unsigned char srcRawY[256];
    unsigned char transRawY[256];
    int frame_offset = 0;
    int mb_offset = 0;

    parse_options(argc, argv);

    // Generate hash of hashes from input stream and transcoded stream
    //      and pick random keyframe and macroblock number
    getRandomOffsets(files.front().c_str(), files.back().c_str(), &frame_offset, 10, &mb_offset, 1);
    printf("frame_offset=%d macroblock_offset=%d\n", frame_offset, mb_offset);

    memset(srcRawY, 0x00, 256);
    memset(transRawY, 0x00, 256);
    getMbFromStream(files.front().c_str(), 1, 10, &mbSrc, srcRawY, verbose);
    test_sha256_circuit(srcRawY, 256);
    getMbFromStream(files.back().c_str(), 1, 10, &mbTrans, transRawY, verbose);
    if (mbSrc.mb_data) free(mbSrc.mb_data);
    if (mbTrans.mb_data) free(mbTrans.mb_data);

    initialize_prover();
    generate_ssim_proof(
            proving_key_path.c_str(),
            ref_ssim,
            srcRawY, sizeof(srcRawY),
            transRawY, sizeof(transRawY),
            proof.empty() ? nullptr : proof.c_str(),
            uncompressed_proof.empty() ? nullptr : uncompressed_proof.c_str(),
            json_proof.empty() ? nullptr : json_proof.c_str());

    save_witness(witness.c_str(), ref_ssim);
}
