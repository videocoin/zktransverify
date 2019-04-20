//
// Created by taras on 06.02.19.
//
#pragma once

typedef struct In64 {
    unsigned char pix1[64 * 64];
    unsigned char pix2[64 * 64];
} In64;
typedef struct Out64 {
    int ssim;
    unsigned int counter;
} Out64;

void h264_ssim64x64_compute(struct In64 *input, struct Out64 *output);
