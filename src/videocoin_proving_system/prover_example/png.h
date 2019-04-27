//
// Created by taras on 16.04.19.
//

#ifndef VIDEOCOIN_PROOVING_SYSTEM_PNG_H
#define VIDEOCOIN_PROOVING_SYSTEM_PNG_H

#include <png.h>

typedef struct
{
    int y_width;
    int y_height;
    int y_stride;

    int uv_width;
    int uv_height;
    int uv_stride;

    unsigned char *y_buffer;
    unsigned char *u_buffer;
    unsigned char *v_buffer;
} YV12_BUFFER_CONFIG;

png_bytep *read_png_file(const char *file_name);
void png_to_YV12(png_bytep *row_pointers, png_bytep *row_pointers2, YV12_BUFFER_CONFIG &src, YV12_BUFFER_CONFIG &dest);
void clear_YV12(YV12_BUFFER_CONFIG &buffer);

#endif //VIDEOCOIN_PROOVING_SYSTEM_PNG_H
