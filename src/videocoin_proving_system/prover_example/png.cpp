//
// Created by taras on 18.04.19.
//

#include "png.h"
#include <cstdlib>

int x, y;

png_uint_32 width, height;

png_structp png_ptr;
png_infop info_ptr;

png_bytep *read_png_file(const char *file_name) {
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

#if PNG_LIBPNG_VER > 10254
    png_get_IHDR(png_ptr, info_ptr, &width, &height, NULL, NULL,
                 NULL, NULL, NULL);
#else

    width = info_ptr->width;
    height = info_ptr->height;
#endif
    png_read_update_info(png_ptr, info_ptr);


    /* read file */
    if (setjmp(png_jmpbuf(png_ptr))) {
        printf("[read_png_file] Error during read_image\n");
    }

    row_pointers = (png_bytep *) malloc(sizeof(png_bytep) * height);
    for (y = 0; y < height; y++)
#if PNG_LIBPNG_VER > 10254
        row_pointers[y] = (png_byte *)png_malloc(png_ptr, png_get_rowbytes(png_ptr, info_ptr));
#else
        row_pointers[y] = (png_byte *) malloc(info_ptr->rowbytes);
#endif

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

void clear_YV12(YV12_BUFFER_CONFIG &buf) {
    delete []buf.y_buffer;
    delete []buf.u_buffer;
    delete []buf.v_buffer;
}