//
// Created by taras on 15.04.19.
//

#include "prover/prover.h"
#include "common/utility.h"
#include "png.h"
#include "h264_ssim16x16_from_prover.h"
#include "h264_ssim32x32_from_prover.h"
#include "h264_ssim64x64_from_prover.h"
#include "h264_ssim_original.h"

#include <cstdlib>
#include <fstream>
#include <vector>
#include <exception>

#include <boost/program_options.hpp>

namespace po = boost::program_options;

int main(int argc, char *argv[]) {
    try {
        std::vector<std::string> files;
        po::options_description general("General options");
        po::options_description prover("Prover options");
        po::options_description all;
        po::variables_map vm;

        general.add_options()
                ("help,h", "produce help message");

        prover.add_options()
                ("pkey,p", po::value<std::string>(), "path to proving key")
                ("files,f", po::value<std::vector<std::string>>(&files)->multitoken(), "list of %file1, %file2")
                ("input-output,i", po::value<std::string>(), "path to input-output file")
                ("proof,P", po::value<std::string>(), "path to proof file")
                ("uncompressed-proof,u", po::value<std::string>(), "path to uncompressed proof file")
                ("json-proof,j", po::value<std::string>(), "path to json proof file");

        all.add(general).add(prover);

        po::store(po::parse_command_line(argc, argv, all), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << all << std::endl;
            exit(0);
        }

        // check mandatory options
        for (auto &e: {"files", "pkey", "input-output", "proof"}) {
            if (!vm.count(e)) {
                std::cerr << "error: the option '--" << e << "' is required but missing\n" << all << std::endl;
                exit(1);
            }
        }

        ssim_mode mode(ssim_mode::_x16);

        if (files.size() != 2) {
            std::cerr << "error: the option '--files' has invalid number files\n"
                      << all << std::endl;
            exit(1);
        }

        png_bytep *row_pointers1;
        png_bytep *row_pointers2;
        row_pointers1 = read_png_file(files.front().c_str());
        row_pointers2 = read_png_file(files.back().c_str());
        YV12_BUFFER_CONFIG src;
        YV12_BUFFER_CONFIG dest;

        png_to_YV12(row_pointers1, row_pointers2, src, dest);

        printf("Mode %s\n", mode.str().c_str());

        int ref_ssim = 80;
        unsigned int accepted;
        ///////////////////////////////////////////////////////////////////////////////////
        // Calculate SSIM using prover library.
        printf("\n\nArithmetic based SSIM\n");

        initialize_prover();
        generate_ssim_proof(
                vm["pkey"].as<std::string>().c_str(),
                ref_ssim,
                src.y_buffer, src.y_width * src.y_height,
                dest.y_buffer, dest.y_width * dest.y_height,
                vm["proof"].as<std::string>().c_str(),
                vm.count("uncompressed-proof") ? vm["uncompressed-proof"].as<std::string>().c_str() : nullptr,
                vm.count("json-proof") ? vm["json-proof"].as<std::string>().c_str() : nullptr);

        printf("\n\nPepper based SSIM\n");

        double ssim;
        if (mode.is_16()) {
            In in;
            Out out;
            in.ref_ssim = ref_ssim;
            memcpy(in.pix1, src.y_buffer, sizeof(in.pix1));
            memcpy(in.pix2, dest.y_buffer, sizeof(in.pix2));
            h264_ssim16x16_compute(&in, &out);
            ssim = out.ssim;
        } else if (mode.is_32()) {
            In32 in;
            Out32 out;
//            in.ref_ssim = ref_ssim;
            memcpy(in.pix1, src.y_buffer, sizeof(in.pix1));
            memcpy(in.pix2, dest.y_buffer, sizeof(in.pix2));
            h264_ssim32x32_compute(&in, &out);
            ssim = out.ssim;
        } else if (mode.is_64()) {
            In64 in;
            Out64 out;
//            in.ref_ssim = ref_ssim;
            memcpy(in.pix1, src.y_buffer, sizeof(in.pix1));
            memcpy(in.pix2, dest.y_buffer, sizeof(in.pix2));
            h264_ssim64x64_compute(&in, &out);
            ssim = out.ssim;
        }
        printf("ssim: %f\n", ssim);

        ///////////////////////////////////////////////////////////////////////////////////
        // Calculate SSIM using h264 implementation
        int counter;
        printf("\n\nh264 based SSIM\n");
        ssim = x264_pixel_ssim_wxh(src.y_buffer, mode.as_int(), dest.y_buffer, mode.as_int(), mode.as_int(), mode.as_int(),
                                   &counter);
        printf("ssim: %f\n", ssim / counter);

        clear_YV12(src);
        clear_YV12(dest);
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