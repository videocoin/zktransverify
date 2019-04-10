#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "decode-h264-mb.h"
#include "sha256-util.h"

bool ARG_HELP;
const char* ARG_VIDEO_PATH1;
const char* ARG_VIDEO_PATH2;

void parse_options(int argc, const char* argv[]);

void parse_options(int argc, const char* argv[])
{
	int i = 1;
	while (i < argc) {
		if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
			ARG_HELP = true;
		} else {
			if(i < argc) {
				ARG_VIDEO_PATH1 = argv[i];
				i++;
			}
			if(i < argc) {
				ARG_VIDEO_PATH2 = argv[i];
			}
		}
		i++;
	}
	if (ARG_HELP || ARG_VIDEO_PATH1 == NULL || ARG_VIDEO_PATH2 == NULL) {
		fprintf(stderr,
				"Usage: genproof videoPath1 videoPath2\n  --help and -h will output this help message.\n");
		exit(1);
	}
}


int main(int argc, char **argv)
{
	MB_T mbSrc = {0};
	MB_T mbTrans = {0};

	int frame_offset = 0;
	int mb_offset = 0;

	unsigned char hashSrc[65];
	unsigned char hashTrans[65];
	parse_options(argc, argv);

	// Generate hash of hashes from input stream and transcoded stream
	//      and pick random keyframe and macroblock number
	getRandomOffsets(ARG_VIDEO_PATH1, ARG_VIDEO_PATH2, &frame_offset, 10, &mb_offset, 1);
	printf("frame_offset=%d macroblock_offset=%d\n",frame_offset, mb_offset);

	getMbFromStream(ARG_VIDEO_PATH1, 1, 10, &mbSrc);
	getMbFromStream(ARG_VIDEO_PATH2, 1, 10, &mbTrans);
	if(mbSrc.mb_data) free(mbSrc.mb_data);
	if(mbTrans.mb_data) free(mbTrans.mb_data);

	// TODO Get other public parameters

	// TODO Calculate expected SSIM

	// TODO Call proof generation and obtain the proof

	// TODO Call ethereum smart-contract/verifier or stand-alone verifier and submit the proof
}
