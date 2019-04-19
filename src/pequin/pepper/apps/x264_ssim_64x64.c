//
// Created by taras on 06.02.19.
//

#include <fix_t.h>

#define WIDTH 64
#define HEIGHT 64
#define STRIDE 64

// const int ssim_c1 = (int)(.01*.01*255*255*64 + .5); = 416.66
#define SSIM_C1 416
// const int ssim_c2 = (int)(.03*.03*255*255*64*63 + .5); 235963.22
#define SSIM_C2 235963


#define pixel unsigned char
#define MIN(a,b) ( (a)<(b) ? (a) : (b) )
#define MAX(a,b) ( (a)>(b) ? (a) : (b) )
#define ABS(v) ( (v)>(0) ? (v) : (-v) )

struct In {
    pixel pix1[WIDTH * HEIGHT];
    pixel pix2[WIDTH * HEIGHT];
};

struct Out {
    fix_t ssim;
    unsigned int counter;
};

int buf[1024];

void ssim_4x4x2_core( const pixel *pix1, const pixel *pix2, int *sums )
{
    int z, y, x;
    for (z = 0; z < 2; z++ )
    {
        unsigned int s1 = 0, s2 = 0, ss = 0, s12 = 0;
        for (y = 0; y < 4; y++ )
        {
            for (x = 0; x < 4; x++) {
                int a = pix1[x + y * STRIDE];
                int b = pix2[x + y * STRIDE];
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

// Initializes a value with an expression.
// Otherwise compiler wont work.
int64_t expr(int64_t value)
{
    return value;
}

fix_t ssim_end1( int s1, int s2, int ss, int s12 )
{
    int vars = ss*64 - s1*s1 - s2*s2;
    int covar = s12*64 - s1*s2;
    int64_t a = expr(2 * s1 * s2 + SSIM_C1);
    int64_t b = expr(2 * covar + SSIM_C2);
    int64_t c = expr(s1*s1 + s2*s2 + SSIM_C1);
    int64_t d = expr(vars + SSIM_C2);
    int64_t e = expr(a * b);
    int64_t f = expr(c * d);

    // WARNING: Till this line of code calculations are precise
    // next operation should be fixed point dividing to preserve fraction.
    // Dividing of two int64 values requires int128 scalar type which
    // is not supported by compiler.
    //
    // `ssim_end1` function produces ratio between sums.
    // So we can reduce both values in order to fit into int32 value.

    int64_t temp = expr(MAX(ABS(e), ABS(f)));
    if (temp > 0x100000000) {
        e /= 0x100000000;
        f /= 0x100000000;
    }

    // Calculate the ratio.
    return fix_div(e, f);
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

void compute(struct In *input, struct Out *output) {
    int z = 0;
    fix_t ssim = 0;

    int *sum0 = buf;
    int *sum1 = sum0 + ((WIDTH >> 2) + 3) * 4;

    unsigned int width = WIDTH >> 2;
    unsigned int height = HEIGHT >> 2;

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
                ssim_4x4x2_core(input->pix1 + (4 * (x + z * STRIDE)), input->pix2 + (4 * (x + z * STRIDE)), sum0 + x * 4);
            }
        }

        for (x = 0; x < width-1; x += 4 )
        {
            ssim += ssim_end4(sum0 + x * 4, sum1 + x * 4, MIN(4, width - x - 1));
        }
    }

    output->ssim = ssim;
    output->counter = (height - 1) * (width - 1);
}