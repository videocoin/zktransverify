/*
 * tiny_ssim.h
 *
 *  Created on: Apr 20, 2019
 *      Author: ramp
 */

#ifndef EXTOOLS_TINY_SSIM_H_
#define EXTOOLS_TINY_SSIM_H_

typedef uint8_t  pixel;

float ssim_plane(
                           pixel *pix1, intptr_t stride1,
                           pixel *pix2, intptr_t stride2,
                           int width, int height, void *buf, int *cnt );

uint64_t ssd_plane( const uint8_t *pix1, const uint8_t *pix2, int size );


#endif /* EXTOOLS_TINY_SSIM_H_ */
