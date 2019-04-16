//
// Created by taras on 16.04.19.
//

#pragma once

#include <cstdint>

#ifndef pixel
#define pixel unsigned char
#endif

float x264_pixel_ssim_wxh(
                           pixel *pix1, intptr_t stride1,
                           pixel *pix2, intptr_t stride2,
                           int width, int height, int *cnt);