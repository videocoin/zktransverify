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

void print_usage() {
    printf("Usage: program_name <ssim16x16 | ssim32x32 | ssim64x64> <file1> <file2> <proving key> <output> <proof>\n");
}

int main(int argc, char *argv[]) {
    if (argc != 7) {
        print_usage();
        exit(-1);
    }

    auto mode = ssim_mode::from_str(argv[1]);
    if (!mode.is_valid()) {
        printf("Invalid mode %s\n", argv[1]);
        print_usage();
        exit(-1);
    }

    png_bytep *row_pointers1;
    png_bytep *row_pointers2;
    row_pointers1 = read_png_file(argv[2]);
    row_pointers2 = read_png_file(argv[3]);
    YV12_BUFFER_CONFIG src;
    YV12_BUFFER_CONFIG dest;

    png_to_YV12(row_pointers1, row_pointers2, src, dest);

    printf("Mode %s\n", mode.str().c_str());

    ///////////////////////////////////////////////////////////////////////////////////
    // Calculate SSIM using prover library.

    initialize_prover();
    double ssim = generate_ssim_proof(argv[4], src.y_buffer, src.y_width * src.y_height, dest.y_buffer,
                                      dest.y_width * dest.y_height, argv[5], argv[6], "temp/proof.json");
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