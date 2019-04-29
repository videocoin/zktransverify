//
// Created by taras on 06.02.19.
//
#pragma once

typedef struct In {
    unsigned char pix1[16 * 16];
    unsigned char pix2[16 * 16];
    unsigned int ref_ssim;
} In;
typedef struct Out {
    unsigned int ssim;
    unsigned int accepted;
} Out;

void h264_ssim16x16_compute(struct In *input, struct Out *output);
