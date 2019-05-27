//
// Created by taras on 27.05.19.
//

#ifndef VIDEOCOIN_PROOVING_SYSTEM_SHA256_H
#define VIDEOCOIN_PROOVING_SYSTEM_SHA256_H

#define LUMA 16
#define MB_SIZE (LUMA * LUMA)

#include <stdint.h>

struct In {
    uint8_t mb[MB_SIZE];
}; //Message is preprocessed

struct Out {
    uint8_t hash[32];
}; //sha256 hash (256 bits).

void compute(struct In *input, struct Out *output);

#endif //VIDEOCOIN_PROOVING_SYSTEM_SHA256_H
