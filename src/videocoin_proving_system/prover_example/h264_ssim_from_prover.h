//
// Created by taras on 06.02.19.
//
#pragma once

#include "fix_t.h"

#define WIDTH 16
#define HEIGHT 16
#define STRIDE 16

// const int ssim_c1 = (int)(.01*.01*255*255*64 + .5); = 416.66
#define SSIM_C1 416
// const int ssim_c2 = (int)(.03*.03*255*255*64*63 + .5); 235963.22
#define SSIM_C2 235963


#ifndef pixel
#define pixel unsigned char
#endif

#define X264_MIN(a,b) ( (a)<(b) ? (a) : (b) )

struct In {
    pixel pix1[WIDTH * HEIGHT];
    pixel pix2[WIDTH * HEIGHT];
};

struct Out {
    ufix_t ssim;
    unsigned int counter;
};

void h264_ssim_compute(struct In *input, struct Out *output);
