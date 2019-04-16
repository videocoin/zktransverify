//
// Created by taras on 15.04.19.
//

#include "prover/prover.h"
#include "ssim.h"
#include "h264_ssim_from_prover.h"
#include "h264_ssim_original.h"
#include <png.h>
#include <cstdlib>

using std::exit;

int x, y;

int width, height;

png_structp png_ptr;
png_infop info_ptr;

png_bytep *read_png_file(char *file_name) {
    png_bytep *row_pointers;
    png_byte header[8];    // 8 is the maximum size that can be checked

    /* open file and test for it being a png */
    FILE *fp = fopen(file_name, "rb");
    if (!fp) {
        printf("[read_png_file] File %s could not be opened for reading\n", file_name);
        abort();
    }

    fread(header, 1, 8, fp);
    if (png_sig_cmp(header, 0, 8)) {
        printf("[read_png_file] File %s is not recognized as a PNG file\n", file_name);
        exit(-1);
    }

    /* initialize stuff */
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

    if (!png_ptr) {
        printf("[read_png_file] png_create_read_struct failed");
        exit(-1);
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        printf("[read_png_file] png_create_info_struct failed");
        exit(-1);
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        printf("[read_png_file] Error during init_io\n");
        exit(-1);
    }

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);

    png_read_info(png_ptr, info_ptr);

    width = info_ptr->width;
    height = info_ptr->height;

    png_read_update_info(png_ptr, info_ptr);


    /* read file */
    if (setjmp(png_jmpbuf(png_ptr))) {
        printf("[read_png_file] Error during read_image\n");
    }

    row_pointers = (png_bytep *) malloc(sizeof(png_bytep) * height);
    for (y = 0; y < height; y++)
        row_pointers[y] = (png_byte *) malloc(info_ptr->rowbytes);

    png_read_image(png_ptr, row_pointers);

    fclose(fp);
    return row_pointers;
}

int clamp(int a) {
    if (a > 255)
        return 255;
    if (a < 0)
        return 0;
    return a;
}

int yc(int r, int g, int b) {
    return clamp(16 + 65.738 * r / 256 + 129.057 * g / 256 + 25.064 * b / 256);
}

int uc(int r, int g, int b) {
    return clamp(128 + -37.945 * r / 256 - 74.494 * g / 256 + 112.439 * b / 256);
}

int vc(int r, int g, int b) {
    return clamp(128 + 112.439 * r / 256 - 94.154 * g / 256 - 18.285 * b / 256);
}

void png_to_YV12(png_bytep *row_pointers, png_bytep *row_pointers2, YV12_BUFFER_CONFIG &src, YV12_BUFFER_CONFIG &dest) {
//    if (info_ptr->color_type != PNG_COLOR_TYPE_RGB) {
//        printf("[png_to_YV12] color_type of input file must be PNG_COLOR_TYPE_RGBA (is %d)\n", info_ptr->color_type);
//        exit(-1);
//    }

    src.y_height = height;
    src.y_width = width;
    src.y_stride = width;
    src.uv_height = height;
    src.uv_width = width;
    src.uv_stride = width;
    src.y_buffer = new png_byte[height * width];
    src.u_buffer = new png_byte[height * width];
    src.v_buffer = new png_byte[height * width];

    dest.y_buffer = new png_byte[height * width];
    dest.u_buffer = new png_byte[height * width];
    dest.v_buffer = new png_byte[height * width];
    dest.y_height = height;
    dest.y_width = width;
    dest.y_stride = width;
    dest.uv_height = height;
    dest.uv_width = width;
    dest.uv_stride = width;

    for (y = 0; y < height; y++) {
        png_byte *row1 = row_pointers[y];
        png_byte *row2 = row_pointers2[y];
        for (x = 0; x < width; x++) {
            int src_r = row1[x * 3 + 0];
            int src_g = row1[x * 3 + 1];
            int src_b = row1[x * 3 + 2];
            //int src_a = row1[x*3 + 3];

            src.y_buffer[y * width + x] = yc(src_r, src_g, src_b);
            src.u_buffer[y * width + x] = uc(src_r, src_g, src_b);
            src.v_buffer[y * width + x] = vc(src_r, src_g, src_b);

            int dst_r = row2[x * 3 + 0];
            int dst_g = row2[x * 3 + 1];
            int dst_b = row2[x * 3 + 2];
            //int dst_a = row2[x*4 + 3];

            dest.y_buffer[y * width + x] = yc(dst_r, dst_g, dst_b);
            dest.u_buffer[y * width + x] = uc(dst_r, dst_g, dst_b);
            dest.v_buffer[y * width + x] = vc(dst_r, dst_g, dst_b);


        }
    }
}


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

    initialize_prover();
    generate_ssim_proof(argv[3], src.y_buffer, dest.y_buffer, argv[4], argv[5], ssim);
    printf("\n\nArithmetic circuit\n");
    printf("ssim: %f\n", ssim);

    printf("\n\nFrom Pepper\n");
    In in;
    Out out;
    memcpy(in.pix1, src.y_buffer, sizeof(in.pix1));
    memcpy(in.pix2, dest.y_buffer, sizeof(in.pix2));

    h264_ssim_compute(&in, &out);
    ssim = out.ssim / 0x10000;
    ssim /= out.counter;
    printf("ssim: %f\n", ssim);

    printf("\n\nOriginal h264 implementation\n");
    int counter;
    ssim = x264_pixel_ssim_wxh(src.y_buffer, 16, dest.y_buffer, 16, 16, 16, &counter);
    printf("ssim: %f\n", ssim/counter);

    printf("\n\nOther implementations\n");

    double ssim_y;
    double ssim_u;
    double ssim_v;

    ssim = vp8_calc_ssimg(&src, &dest, &ssim_y, &ssim_u, &ssim_v);
    printf("ssimg: %f\n\ty: %f\n\tu: %f\n\tv: %f\n", 1 / (1 - ssim), 1 / (1 - ssim_y), 1 / (1 - ssim_u), 1 / (1 - ssim_v));
    double weight;
    double p = vp8_calc_ssim(&src, &dest, 1, &weight);
    printf("ssim: %f\n", 1 / (1 - p));

    delete []src.y_buffer;
    delete []src.u_buffer;
    delete []src.v_buffer;

    delete []dest.y_buffer;
    delete []dest.u_buffer;
    delete []dest.v_buffer;
}