#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <prover/prover.h>

#include "decode-h264-mb.h"
#include "sha256-util.h"

bool ARG_HELP;
const char *ARG_VIDEO_PATH1;
const char *ARG_VIDEO_PATH2;
const char *ARG_PROVING_KEY_PATH;

void parse_options(int argc, const char *argv[]);

void save_witness(const char *filename, int refssim, int dbgssim, unsigned char *p1, unsigned char *p2);

void parse_options(int argc, const char *argv[]) {
    int i = 1;
    while (i < argc) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            ARG_HELP = true;
        } else {
            if (i < argc) {
                ARG_VIDEO_PATH1 = argv[i++];
            }
            if (i < argc) {
                ARG_VIDEO_PATH2 = argv[i++];
            }
            if (i < argc) {
                ARG_PROVING_KEY_PATH = argv[i];
            }
        }
        i++;
    }
    if (ARG_HELP || ARG_VIDEO_PATH1 == NULL || ARG_VIDEO_PATH2 == NULL) {
        fprintf(stderr,
                "Usage: genproof videoPath1 videoPath2 provingKey\n  --help and -h will output this help message.\n");
        exit(1);
    }
}

void save_witness(const char *filename, int refssim, int dbgssim, unsigned char *p1, unsigned char *p2) {
    FILE *f = fopen(filename, "w+");
    if (f != NULL) {
        int i;
        fprintf(f, "%d %d ", refssim, dbgssim);
        for (i = 0; i < 256; i++) {
            fprintf(f, "%d ", p1[i]);
        }
        for (i = 0; i < 256; i++) {
            fprintf(f, "%d ", p2[i]);
        }

        fclose(f);
    }

}

int main(int argc, const char *argv[]) {
    MB_T mbSrc = {0};
    MB_T mbTrans = {0};
    unsigned char srcRawY[256];
    unsigned char transRawY[256];
    int frame_offset = 0;
    int mb_offset = 0;

    parse_options(argc, argv);

    // Generate hash of hashes from input stream and transcoded stream
    //      and pick random keyframe and macroblock number
    getRandomOffsets(ARG_VIDEO_PATH1, ARG_VIDEO_PATH2, &frame_offset, 10, &mb_offset, 1);
    printf("frame_offset=%d macroblock_offset=%d\n", frame_offset, mb_offset);

    memset(srcRawY, 0x00, 256);
    memset(transRawY, 0x00, 256);
    getMbFromStream(ARG_VIDEO_PATH1, 1, 10, &mbSrc, srcRawY);
    getMbFromStream(ARG_VIDEO_PATH2, 1, 10, &mbTrans, transRawY);
    if (mbSrc.mb_data) free(mbSrc.mb_data);
    if (mbTrans.mb_data) free(mbTrans.mb_data);

    initialize_prover();
    double ssim = generate_ssim_proof(ARG_PROVING_KEY_PATH, srcRawY, sizeof(srcRawY), transRawY, sizeof(transRawY),
                                      "output.txt", "ssim.proof", "proof.json");
    printf("{\"witness\":[\"%d\"]}\n", (int) (ssim * 100));
    save_witness("mywitness", 80, (int) (ssim * 100), srcRawY, transRawY);

    // TODO Call ethereum smart-contract/verifier or stand-alone verifier and submit the proof
}
