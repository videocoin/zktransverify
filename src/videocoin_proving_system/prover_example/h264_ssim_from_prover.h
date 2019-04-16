//
// Created by taras on 06.02.19.
//
#pragma once

struct In {
    unsigned char pix1[16 * 16];
    unsigned char pix2[16 * 16];
};
struct Out {
    unsigned int ssim;
    unsigned int counter;
};

void h264_ssim_compute(struct In *input, struct Out *output);
