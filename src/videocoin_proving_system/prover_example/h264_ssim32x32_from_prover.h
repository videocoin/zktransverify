//
// Created by taras on 06.02.19.
//
#pragma once

typedef struct In32 {
    unsigned char pix1[32 * 32];
    unsigned char pix2[32 * 32];
} In32;
typedef struct Out32 {
    int ssim;
    unsigned int counter;
} Out32;

void h264_ssim32x32_compute(struct In32 *input, struct Out32 *output);
