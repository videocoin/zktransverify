//
// Created by taras on 06.02.19.
//

#include <fix_t.h>

#define WIDTH 16
#define HEIGHT 16
#define STRIDE 16
#define C1 (fix_t)1745501552
#define C2 (fix_t)15709513973

#define pixel unsigned char
#define abs(v) ((v) >= 0 ? (v) : -(v))

struct In {
    pixel A[WIDTH * HEIGHT];
    pixel B[WIDTH * HEIGHT];
};

struct Out {
    fix_t ssim;
};

fix_t average(pixel *image) {
    pixel *ptr;
    int x, y, b=0, g=0, r=0, col=0;

    for (x=0; x< WIDTH; x++) {
        for (y=0; y< HEIGHT; y++) {
            ptr = image[y * STRIDE];
            b = ptr[3*x];
            g = ptr[3*x+1];
            r = ptr[3*x+2];
            col += b + g + r;
        }
    }
    return fix_div(int_to_fix(col), int_to_fix(3));
}

fix_t h(int v, fix_t mu) {
    fix_t fv = int_to_fix(v);
    return fix_mul(abs(fv - mu), abs(fv - mu));
}

fix_t variance(pixel *image, fix_t mu) {
    pixel *ptr;
    int x, y, b=0, g=0, r=0;
    fix_t col = 0;

    for (x=0; x < WIDTH; x++) {
        for (y=0; y < HEIGHT; y++) {
            ptr = image[y * STRIDE];

            b = ptr[3*x];
            col += h(b, mu);
            g = ptr[3*x+1];
            col += h(g, mu);
            r = ptr[3*x+2];
            col += h(r, mu);
        }
    }
    return col;
}

fix_t covariance(pixel* image1, pixel* image2, fix_t mu1, fix_t mu2) {
    pixel* ptr;
    int x, y;
    fix_t col = 0, b1=0, g1=0, r1=0, b2=0, g2=0, r2=0;

    for (x=0; x< WIDTH; x++) {
        for (y=0; y < HEIGHT; y++) {
            ptr = image1[y * STRIDE];
            b1 = int_to_fix(ptr[3*x]);
            g1 = int_to_fix(ptr[3*x+1]);
            r1 = int_to_fix(ptr[3*x+2]);

            ptr = image2[y * STRIDE];
            b2 = int_to_fix(ptr[3*x]);
            g2 = int_to_fix(ptr[3*x+1]);
            r2 = int_to_fix(ptr[3*x+2]);

            col += fix_mul((b1-mu1), (b2-mu2))+fix_mul((g1-mu1), (g2-mu2))+fix_mul((r1-mu2), (r2-mu2));
        }}

    return col;
}

void compute(struct In *input, struct Out *output) {
    fix_t mu_A = average(input->A);
    fix_t mu_B = average(input->B);

    fix_t v_A = variance(input->A, mu_A);
    fix_t v_B = variance(input->B, mu_B);

    fix_t cv = covariance(input->A, input->B, mu_A, mu_B);

    fix_t two = int_to_fix(2);
    fix_t n = fix_mul((fix_mul(fix_mul(two, mu_A), mu_B) + C1), (fix_mul(two, cv) + C2));
    fix_t d = fix_mul((fix_mul(mu_A, mu_A) + fix_mul(mu_B, mu_B) + C1), (fix_mul(v_A, v_A) + fix_mul(v_B, v_B) + C2));

    output->ssim = fix_div(n, d);
}