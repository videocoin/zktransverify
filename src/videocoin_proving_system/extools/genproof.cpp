#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <iostream>

#include <boost/program_options.hpp>

#include <prover/prover.h>

#include "decode-h264-mb.h"
#include "sha256-util.h"

namespace po = boost::program_options;

std::vector<std::string> files;
std::string proving_key_path;
std::string proof;
int ref_ssim;
bool verbose = false;

void parse_options(int argc, const char *argv[]);

//void save_witness(const char *filename, int ref_ssim, unsigned char *src, size_t len);

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
                ("ssim-level,s", po::value<int>(&ref_ssim)->default_value(80), "threshold ssim level [0-100]")
                ("verbose,v", "verbose mode");

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
        for (auto &e: {"files", "pkey", "ssim-level", "proof"}) {
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

/*
void save_witness(const char *filename, int refssim, unsigned char *src, size_t len) {
    if (!filename) return;
    FILE *f = fopen(filename, "w+");
    if (f != nullptr) {
        fprintf(f, "%d\n", refssim);
        for (int i = 0; i < len; ++i) {
            fprintf(f, "%d\n", src[i]);
        }
        fprintf(f, "%d\n%d\n", 1, 0);
//        fprintf(f, "%d\n%d\n%d\n", refssim, 1, 0);
        fclose(f);
    }
}
*/

int main(int argc, const char *argv[]) {
    unsigned char src_raw_y[256];
    unsigned char trans_raw_y[256];
    int frame_offset = 0;
    int mb_offset = 0;

    parse_options(argc, argv);

    // Generate hash of hashes from input stream and transcoded stream
    //      and pick random keyframe and macroblock number
    getRandomOffsets(files.front().c_str(), files.back().c_str(), &frame_offset, 10, &mb_offset, 1);
    frame_offset = 1;
    mb_offset = 0;

    printf("frame_offset=%d macroblock_offset=%d\n", frame_offset, mb_offset);

    memset(src_raw_y, 0x00, 256);
    memset(trans_raw_y, 0x00, 256);
    get_mb_from_stream(files.front().c_str(), frame_offset, mb_offset, src_raw_y, verbose);
    get_mb_from_stream(files.back().c_str(), frame_offset, mb_offset, trans_raw_y, verbose);

    initialize_prover();
    generate_ssim_proof(
            proving_key_path.c_str(),
            ref_ssim,
            src_raw_y, sizeof(src_raw_y),
            trans_raw_y, sizeof(trans_raw_y),
            proof.c_str());

//    save_witness(witness.c_str(), ref_ssim, src_raw_y, sizeof(src_raw_y));
}
