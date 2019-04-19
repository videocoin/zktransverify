//
// Created by taras on 15.04.19.
//

#include "prover/prover.h"
#include "png.h"
#include "h264_ssim_from_prover.h"
#include "h264_ssim_original.h"

#include <cstdlib>
#include <fstream>

int main(int argc, char *argv[]) {
    if (argc != 6) {
        printf("Usage: program_name <file1> <file2> <proving key> <output> <proof>\n");
        exit(-1);
    }

    png_bytep *row_pointers1;
    png_bytep *row_pointers2;
    row_pointers1 = read_png_file(argv[1]);
    row_pointers2 = read_png_file(argv[2]);
    YV12_BUFFER_CONFIG src;
    YV12_BUFFER_CONFIG dest;

    png_to_YV12(row_pointers1, row_pointers2, src, dest);

    double ssim;

    ///////////////////////////////////////////////////////////////////////////////////
    // Calculate SSIM using prover library.

    std::ofstream inputs("temp/inputs.txt");
    YV12_BUFFER_CONFIG *p = &src;
    for (int i = 0; i < p->y_height * p->y_width; ++i) {
        inputs << (int)p->y_buffer[i] << "\n";
    }
    p = &dest;
    for (int i = 0; i < p->y_height * p->y_width; ++i) {
        inputs << (int)p->y_buffer[i] << "\n";
    }
    inputs.close();

    initialize_prover();
    generate_ssim_proof(ssim_mode::x16, argv[3], src.y_buffer, dest.y_buffer, argv[4], argv[5], ssim);
    printf("\n\nArithmetic based SSIM\n");
    printf("ssim16x16: %f\n", ssim);

    ///////////////////////////////////////////////////////////////////////////////////
    // Calculate SSIM using h264 implementation

    printf("\n\nh264 based SSIM\n");
    int counter;
    ssim = x264_pixel_ssim_wxh(src.y_buffer, 16, dest.y_buffer, 16, 16, 16, &counter);
    printf("ssim16x16: %f\n", ssim/counter);

    printf("\n\nPepper based SSIM\n");
    In in; Out out;
    memcpy(in.pix1, src.y_buffer, sizeof(in.pix1));
    memcpy(in.pix2, dest.y_buffer, sizeof(in.pix2));

    h264_ssim_compute(&in, &out);
    ssim = (float)out.ssim / 65536.0;
    ssim /= out.counter;
    printf("ssim16x16: %f\n", ssim);

    clear_YV12(src);
    clear_YV12(dest);
}