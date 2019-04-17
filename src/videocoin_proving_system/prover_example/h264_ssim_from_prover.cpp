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
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long int uint64_t;
typedef signed char int_least8_t;
typedef short int int_least16_t;
typedef int int_least32_t;
typedef long int int_least64_t;
typedef unsigned char uint_least8_t;
typedef unsigned short int uint_least16_t;
typedef unsigned int uint_least32_t;
typedef unsigned long int uint_least64_t;
typedef signed char int_fast8_t;
typedef long int int_fast16_t;
typedef long int int_fast32_t;
typedef long int int_fast64_t;
typedef unsigned char uint_fast8_t;
typedef unsigned long int uint_fast16_t;
typedef unsigned long int uint_fast32_t;
typedef unsigned long int uint_fast64_t;
typedef long int intptr_t;
typedef unsigned long int uintptr_t;
typedef long int intmax_t;
typedef unsigned long int uintmax_t;
typedef int32_t fix_t;
typedef uint32_t ufix_t;
fix_t fix_add(fix_t a, fix_t b){
    return a + b;
}
fix_t fix_mul(fix_t a, fix_t b){
    int64_t prod = ((int64_t)a) * b;
    return (fix_t)(prod >> 16);
}
fix_t fix_div(fix_t a, fix_t b){
    int64_t quotient = ((int64_t)a) << 16;
    quotient /= b;
    return (fix_t)(quotient);
}
uint64_t uint64sqrt(uint64_t a, int bits){
    uint64_t lo = 0;
    int i;
    for(i = bits - 1; i >= 0; i--){
        uint64_t mid = lo + (1UL << i);
        if (mid * mid > a){
        } else {
            lo = mid;
        }
    }
    return lo;
}
fix_t fix_sqrt(fix_t a){
    uint64_t tosqrt = ((uint64_t) a) << 16;
    return (fix_t)(uint64sqrt(tosqrt,(32 + 16)/2));
}
fix_t int_to_fix(int32_t val){
    return (fix_t)(((uint64_t)val) << 16);
}
int32_t fix_to_int(fix_t val){
    return (int32_t)(val >> 16);
}
fix_t fix_ceil(fix_t val){
    val = fix_add(val, 0x10000 - 1);
    return int_to_fix(fix_to_int(val));
}
ufix_t ufix_add(ufix_t a, ufix_t b){
    return a + b;
}
ufix_t ufix_mul(ufix_t a, ufix_t b){
    uint64_t prod = ((uint64_t)a) * b;
    return (ufix_t)(prod >> 16);
}
ufix_t ufix_div(ufix_t a, ufix_t b){
    uint64_t quotient = ((uint64_t)a) << 16;
    quotient /= b;
    return (ufix_t)(quotient);
}
ufix_t ufix_sqrt(ufix_t a){
    uint64_t tosqrt = ((uint64_t) a) << 16;
    return (ufix_t)(uint64sqrt(tosqrt,(32 + 16)/2));
}
ufix_t uint_to_ufix(uint32_t val){
    return (ufix_t)(((uint64_t)val) << 16);
}
uint32_t ufix_to_int(ufix_t val){
    return (uint32_t)(val >> 16);
}
ufix_t ufix_ceil(ufix_t val){
    val = ufix_add(val, 0x10000 - 1);
    return uint_to_ufix(ufix_to_int(val));
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

ufix_t ssim_end1( int s1, int s2, int ss, int s12 )
{
    o << s1 << " " << s2 << " " << ss << " " << s12 << "\n";
    int vars = ss*64 - s1*s1 - s2*s2;
    int covar = s12*64 - s1*s2;
    o << vars << " " << covar << "\n";

    uint64_t a = (2 * s1 * s2 + 416);
    uint64_t b = (2 * covar + 235963);
    uint64_t c = (s1*s1 + s2*s2 + 416);
    uint64_t d = (vars + 235963);
    uint64_t e = (a * b);
    uint64_t f = (c * d);
    e >>= 32;
    f >>= 32;

    o  << a << " " << b << " " << c << " " << d << " " << e << " " << f << "\n";
    ufix_t res = ufix_div(e, f);
    o << ((float)res / 65536.0) << "\n";

    return res;
}

ufix_t ssim_end4( int *sum0, int *sum1, int width )
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
    ufix_t ssim = 0;
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
        o << "ssim: " << ((float)ssim / 65536.0) << "\n";
    }
    o << "ssim2: " << ((float)ssim / 65536.0) << "\n";

    output->ssim = ssim;
    output->counter = (height - 1) * (width - 1);
}
