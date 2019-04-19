//
// Created by taras on 06.02.19.
//

#include "h264_ssim_from_prover.h"
#include <cstdio>
#include <fstream>
#include <iomanip>

typedef signed char int8_t;
typedef short int int16_t;
typedef int int32_t;
typedef long int int64_t;
typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef unsigned long int uint64_t;
typedef long int intptr_t;
typedef int32_t fix_t;

fix_t fix_div(fix_t a, fix_t b){
    int64_t quotient = ((int64_t)a) << 16;
    quotient /= b;
    return (fix_t)(quotient);
}

int buf[1024];
static std::ofstream o("temp/ported");

void ssim_4x4x2_core( const unsigned char *pix1, const unsigned char *pix2, int *sums )
{
    int z, y, x;
    for (z = 0; z < 2; z++ )
    {
        unsigned int s1 = 0, s2 = 0, ss = 0, s12 = 0;
        for (y = 0; y < 4; y++ )
        {
            for (x = 0; x < 4; x++) {
                int a = pix1[x + y * 16];
                int b = pix2[x + y * 16];
                s1 += a;
                s2 += b;
                ss += a * a;
                ss += b * b;
                s12 += a * b;
            }
        }
        int k = z * 4;
        sums[k + 0] = s1;
        sums[k + 1] = s2;
        sums[k + 2] = ss;
        sums[k + 3] = s12;
        pix1 += 4;
        pix2 += 4;
    }
}

#define ABS(v) ((v > 0) ? (v) : (-v))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

fix_t ssim_end1( int s1, int s2, int ss, int s12 )
{
    o << s1 << " " << s2 << " " << ss << " " << s12 << "\n";
    int vars = ss*64 - s1*s1 - s2*s2;
    int covar = s12*64 - s1*s2;
    o << vars << " " << covar << "\n";

    int64_t a = (2 * s1 * s2 + 416);
    int64_t b = (2 * covar + 235963);
    int64_t c = (s1*s1 + s2*s2 + 416);
    int64_t d = (vars + 235963);
    int64_t e = (a * b);
    int64_t f = (c * d);
    int64_t temp = MAX(ABS(e), ABS(f));
    if (temp > 0x100000000) {
        e /= 0x100000000;
        f /= 0x100000000;
    }

    o  << a << " " << b << " " << c << " " << d << " " << e << " " << f << "\n";
    fix_t res = fix_div(e, f);
    o << ((float)res / 65536.0) << "\n";

    return res;
}

fix_t ssim_end4( int *sum0, int *sum1, int width )
{
    fix_t ssim = 0;
    int i = 0;
    for (i = 0; i < width; i++)
    {
        ssim += ssim_end1(sum0[i * 4 + 0] + sum0[(i + 1) * 4 + 0] + sum1[i * 4 + 0] + sum1[(i + 1) * 4 + 0],
                          sum0[i * 4 + 1] + sum0[(i + 1) * 4 + 1] + sum1[i * 4 + 1] + sum1[(i + 1) * 4 + 1],
                          sum0[i * 4 + 2] + sum0[(i + 1) * 4 + 2] + sum1[i * 4 + 2] + sum1[(i + 1) * 4 + 2],
                          sum0[i * 4 + 3] + sum0[(i + 1) * 4 + 3] + sum1[i * 4 + 3] + sum1[(i + 1) * 4 + 3]);
    }
    return ssim;
}
void h264_ssim_compute(struct In *input, struct Out *output) {
    int z = 0;
    fix_t ssim = 0;
    int *sum0 = buf;
    int *sum1 = sum0 + ((16 >> 2) + 3) * 4;
    unsigned int width = 16 >> 2;
    unsigned int height = 16 >> 2;
    int y, x;

    for (y = 1; y < height; y++)
    {
        for( ; z <= y; z++ )
        {
            int *temp = sum0;
            sum0 = sum1;
            sum1 = temp;
            for (x = 0; x < width; x+=2 )
            {
                ssim_4x4x2_core(input->pix1 + (4 * (x + z * 16)), input->pix2 + (4 * (x + z * 16)), sum0 + x * 4);
            }
        }
        for (x = 0; x < width-1; x += 4 )
        {
            ssim += ssim_end4(sum0 + x, sum1 + x, ( (4)<(width - x - 1) ? (4) : (width - x - 1) ));
        }
        o << "ssim16x16: " << ((float)ssim / 65536.0) << "\n";
    }
    o << "ssim2: " << ((float)ssim / 65536.0) << "\n";

    output->ssim = ssim;
    output->counter = (height - 1) * (width - 1);
}
