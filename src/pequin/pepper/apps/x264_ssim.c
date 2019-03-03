//
// Created by taras on 06.02.19.
//

#include <fix_t.h>

#define WIDTH 16
#define HEIGHT 16
#define STRIDE 16

// const int ssim_c1 = (int)(.01*.01*255*255*64 + .5); = 416.66
#define SSIM_C1 416
// const int ssim_c2 = (int)(.03*.03*255*255*64*63 + .5); 235963.22
#define SSIM_C2 23596322


#define pixel unsigned char
#define X264_MIN(a,b) ( (a)<(b) ? (a) : (b) )

struct In {
    pixel pix1[WIDTH * HEIGHT];
    pixel pix2[WIDTH * HEIGHT];
};

struct Out {
    fix_t ssim;
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

fix_t ssim_end1( int s1, int s2, int ss, int s12 )
{
    int vars = ss*64 - s1*s1 - s2*s2;
    int covar = s12*64 - s1*s2;
    fix_t a = int_to_fix(2 * s1 * s2 + SSIM_C1);
    fix_t b = int_to_fix(2 * covar + SSIM_C2);
    fix_t c = int_to_fix(s1*s1 + s2*s2 + SSIM_C1);
    fix_t d = int_to_fix(vars + SSIM_C2);

    return fix_div(fix_mul(a, b), fix_mul(c, d));
}

fix_t ssim_end4( int *sum0, int *sum1, int width )
{
    fix_t ssim = 0;
    int i = 0;
    int k = 4;
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

    int width = WIDTH >> 2;
    int height = HEIGHT >> 2;

    int y, x;

    for (y = 1; y < height; y++)
    {
        for( ; z <= y; z++ )
        {
            int *temp = sum0;
            sum0 = sum1;
            sum1 = sum0;

            for (x = 0; x < width; x+=2 )
            {
                ssim_4x4x2_core(input->pix1 + (4 * (x + z * STRIDE)), input->pix2 + (4 * (x + z * STRIDE)), sum0 + x);
            }
        }

        for (x = 0; x < width-1; x += 4 )
        {
            ssim += ssim_end4(sum0 + x, sum1 + x, X264_MIN(4, width - x - 1));
        }
    }

    fix_t cnt = int_to_fix((height - 1) * (width - 1));

    output->ssim = fix_div(ssim, cnt);
}