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
                ("mode,m", po::value<std::string>(),
                 "set algorithm type <ssim16x16 | ssim32x32 | ssim64x64>")
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
        for (auto &e: {"mode", "files", "pkey", "input-output", "proof"}) {
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

        ///////////////////////////////////////////////////////////////////////////////////
        // Calculate SSIM using prover library.

        initialize_prover();
        double ssim = generate_ssim_proof(
                vm["pkey"].as<std::string>().c_str(),
                src.y_buffer, src.y_width * src.y_height,
                dest.y_buffer, dest.y_width * dest.y_height,
                vm["input-output"].as<std::string>().c_str(),
                vm["proof"].as<std::string>().c_str(),
                vm.count("uncompressed-proof") ? vm["uncompressed-proof"].as<std::string>().c_str() : nullptr,
                vm.count("json-proof") ? vm["json-proof"].as<std::string>().c_str() : nullptr);
        printf("\n\nArithmetic based SSIM\n");
        printf("ssim: %f\n", ssim);

        ///////////////////////////////////////////////////////////////////////////////////
        // Calculate SSIM using h264 implementation

        printf("\n\nh264 based SSIM\n");
        int counter;
        ssim = x264_pixel_ssim_wxh(src.y_buffer, mode.as_int(), dest.y_buffer, mode.as_int(), mode.as_int(), mode.as_int(),
                                   &counter);
        printf("ssim: %f\n", ssim / counter);

        printf("\n\nPepper based SSIM\n");

        if (mode.is_16()) {
            In in;
            Out out;
            memcpy(in.pix1, src.y_buffer, sizeof(in.pix1));
            memcpy(in.pix2, dest.y_buffer, sizeof(in.pix2));
            h264_ssim16x16_compute(&in, &out);
            ssim = out.ssim;
            counter = out.counter;
        } else if (mode.is_32()) {
            In32 in;
            Out32 out;
            memcpy(in.pix1, src.y_buffer, sizeof(in.pix1));
            memcpy(in.pix2, dest.y_buffer, sizeof(in.pix2));
            h264_ssim32x32_compute(&in, &out);
            ssim = out.ssim;
            counter = out.counter;
        } else if (mode.is_64()) {
            In64 in;
            Out64 out;
            memcpy(in.pix1, src.y_buffer, sizeof(in.pix1));
            memcpy(in.pix2, dest.y_buffer, sizeof(in.pix2));
            h264_ssim64x64_compute(&in, &out);
            ssim = out.ssim;
            counter = out.counter;
        }
        ssim /= 65536.0;
        ssim /= counter;
        printf("ssim: %f\n", ssim);

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