//
// Created by taras on 06.02.19.
//


#include "h264_mb_decode.c"
#include "h264_ssim.c"
#include "sha256.c"

/****************************** Input/Output *****************************/
struct In {
    int32_t ref_ssim;
    uint8_t ref_hash[SHA256_LEN];
};

struct Out {
    // compiler requires at least one variable in order to compile the source
    uint32_t dummy;
};

struct exo_input_mb {
    uint8_t values[WIDTH * HEIGHT];
};

struct exo_h264mb_context {
    int values[H264CONTEXT_EXO_NUM];
};


/****************************** Main Entry *****************************/
uint32_t compute(struct In *input, struct Out *output) {
    uint32_t dummy[1];
    uint32_t *exo0_inputs[1] = { dummy };
    uint32_t lens[1] = {0};
    uint8_t decoded_mb[256];

    struct exo_input_mb macroblock[1];
    struct exo_h264mb_context mb_context[1];

    exo_compute(exo0_inputs, lens, macroblock, 0);
    // input macroblock
    uint8_t *input_mb = macroblock[0].values;

    // Parameters to decode macroblock
    exo_compute(exo0_inputs, lens, mb_context, 1);
    init_context(mb_context[0].values);

    decode_mb(&context, decoded_mb);
    uint32_t ssim_accepted = compute_ssim(input->ref_ssim, input_mb, decoded_mb);
    uint32_t hash_accepted = compute_sha256(input->ref_hash, input_mb);
    return ssim_accepted == 1 && hash_accepted == 1;
}