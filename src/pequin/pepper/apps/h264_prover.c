//
// Created by taras on 06.02.19.
//

#include <fix_t.h>

/****************************** SSIM MACROS ******************************/
#define WIDTH 16
#define HEIGHT 16
#define STRIDE 16

// const int ssim_c1 = (int)(.01*.01*255*255*64 + .5); = 416.66
#define SSIM_C1 416
// const int ssim_c2 = (int)(.03*.03*255*255*64*63 + .5); 235963.22
#define SSIM_C2 235963


#define MIN(a,b) ( (a)<(b) ? (a) : (b) )
#define MAX(a,b) ( (a)>(b) ? (a) : (b) )
#define ABS(v) ( (v)>(0) ? (v) : (-v) )

/****************************** SHA256 MACROS *****************************/
#define LUMA 16
#define MB_SIZE (LUMA * LUMA)
#define SHA256_LEN 32

#define ROTLEFT(a,b) (((a) << (b)) | ((a) >> (32-(b))))
#define ROTRIGHT(a,b) (((a) >> (b)) | ((a) << (32-(b))))

#define CH(x,y,z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x,y,z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define EP0(x) (ROTRIGHT(x,2) ^ ROTRIGHT(x,13) ^ ROTRIGHT(x,22))
#define EP1(x) (ROTRIGHT(x,6) ^ ROTRIGHT(x,11) ^ ROTRIGHT(x,25))
#define SIG0(x) (ROTRIGHT(x,7) ^ ROTRIGHT(x,18) ^ ((x) >> 3))
#define SIG1(x) (ROTRIGHT(x,17) ^ ROTRIGHT(x,19) ^ ((x) >> 10))

/****************************** SSIM *****************************/

int buf[1024];

void ssim_4x4x2_core( const uint8_t *pix1, const uint8_t *pix2, int *sums )
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

uint32_t compute_ssim(uint32_t ref_ssim, uint8_t *pix1, uint8_t *pix2) {
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
                ssim_4x4x2_core(pix1 + (4 * (x + z * STRIDE)), pix2 + (4 * (x + z * STRIDE)), sum0 + x * 4);
            }
        }

        for (x = 0; x < width-1; x += 4 )
        {
            ssim += ssim_end4(sum0 + x * 4, sum1 + x * 4, MIN(4, width - x - 1));
        }
    }

    unsigned int counter = (height - 1) * (width - 1);
    ssim = fix_div(ssim, int_to_fix(counter));
    ssim = fix_to_int(ssim * 100);
    printf("SSIM %Zd", ssim);
    return ssim > ref_ssim && ssim < 100;
}

/****************************** SHA256 *****************************/

struct sha256_input_elm {
    uint8_t values[SHA256_LEN];
};

uint32_t k[64] = {
        0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
        0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
        0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
        0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
        0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
        0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
        0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
        0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};

uint32_t state[8];

void sha256_init()
{
    state[0] = 0x6a09e667;
    state[1] = 0xbb67ae85;
    state[2] = 0x3c6ef372;
    state[3] = 0xa54ff53a;
    state[4] = 0x510e527f;
    state[5] = 0x9b05688c;
    state[6] = 0x1f83d9ab;
    state[7] = 0x5be0cd19;
}

void sha256_transform(uint8_t *data)
{
    uint32_t a, b, c, d, e, f, g, h, i, j, t1, t2, m[64];

    for (i = 0, j = 0; i < 16; ++i, j += 4)
        m[i] = (data[j] << 24) | (data[j + 1] << 16) | (data[j + 2] << 8) | (data[j + 3]);
    for ( ; i < 64; ++i)
        m[i] = SIG1(m[i - 2]) + m[i - 7] + SIG0(m[i - 15]) + m[i - 16];

    a = state[0];
    b = state[1];
    c = state[2];
    d = state[3];
    e = state[4];
    f = state[5];
    g = state[6];
    h = state[7];

    for (i = 0; i < 64; ++i) {
        t1 = h + EP1(e) + CH(e,f,g) + k[i] + m[i];
        t2 = EP0(a) + MAJ(a,b,c);
        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;
    state[5] += f;
    state[6] += g;
    state[7] += h;
}

void sha256_final(uint8_t *hash)
{
    uint32_t i;
    uint8_t data[64];

    i = 0;

    // Pad whatever data is left in the buffer.
    data[i++] = 0x80;
    while (i < 56)
        data[i++] = 0x00;

    unsigned long long bitlen;
    bitlen = 2048;

    // Append to the padding the total message's length in bits and transform.
    data[63] = bitlen;
    data[62] = bitlen >> 8;
    data[61] = bitlen >> 16;
    data[60] = bitlen >> 24;
    data[59] = bitlen >> 32;
    data[58] = bitlen >> 40;
    data[57] = bitlen >> 48;
    data[56] = bitlen >> 56;
    sha256_transform(data);

    // Since this implementation uses little endian byte ordering and SHA uses big endian,
    // reverse all the bytes when copying the final state to the output hash.
    for (i = 0; i < 4; ++i) {
        hash[i]      = (state[0] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 4]  = (state[1] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 8]  = (state[2] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 12] = (state[3] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 16] = (state[4] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 20] = (state[5] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 24] = (state[6] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 28] = (state[7] >> (24 - i * 8)) & 0x000000ff;
    }
}

uint32_t sha265_equal(uint8_t *lhs, uint8_t *rhs)
{
    int i, counter;
    i = 0;
    counter = 0;
    for (; i < SHA256_LEN; ++i) {
        printf("Bytes match test %Zd == %Zd", lhs[i], rhs[i]);
        if (lhs[i] == rhs[i]) ++counter;
    }
    return counter == SHA256_LEN;
}

uint32_t compute_sha256(uint8_t *ref_hash, uint8_t *mb)
{
    uint8_t hash[SHA256_LEN];
    sha256_init();

    // 0 - 63
    sha256_transform(mb);
    // 64 - 127
    sha256_transform(mb + 64);
    // 128 - 191
    sha256_transform(mb + 128);
    // 192 - 255
    sha256_transform(mb + 192);

    sha256_final(hash);

    return sha265_equal(ref_hash, hash);
}

/****************************** Input/Output *****************************/
struct In {
    int32_t ref_ssim;
};

struct Out {
    // compiler requires at least one variable in order to compile the source
    uint32_t dummy;
};

struct exo_input_elm {
    uint8_t values[2 * WIDTH * HEIGHT + SHA256_LEN];
};


/****************************** Main Entry *****************************/
uint32_t compute(struct In *input, struct Out *output) {
    uint32_t dummy[1];
    uint32_t *exo0_inputs[1] = { dummy };
    uint32_t lens[1] = {0};

    struct exo_input_elm inputs[1];
    exo_compute(exo0_inputs, lens, inputs, 3);
    uint8_t *pix1 = inputs[0].values;
    uint8_t *pix2 = inputs[0].values + WIDTH * HEIGHT;
    uint8_t *ref_hash = inputs[0].values + 2 * WIDTH * HEIGHT;

    uint32_t ssim_accepted = compute_ssim(input->ref_ssim, pix1, pix2);
    uint32_t hash_accepted = compute_sha256(ref_hash, pix1);
    return ssim_accepted == 1 && hash_accepted == 1;
}