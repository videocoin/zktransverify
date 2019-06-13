//
// Created by taras on 06.02.19.
//


#include "h264_mb_decode.c"
#include "h264_ssim.c"
#include "sha256.c"

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