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
const char* ARG_VIDEO_PATH1 = NULL;
const char* ARG_VIDEO_PATH2 = NULL;


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
	MB_T mb = {0};
	unsigned char outputBuffer[65];
	parse_options(argc, argv);
	int frame_offset = 0;
	int mb_offset = 0;

	// Generate hash of hashes from input stream and transcoded stream
	//      and pick random keyframe and macroblock number

	getRandomOffsets(ARG_VIDEO_PATH1, ARG_VIDEO_PATH2, &frame_offset, 10, &mb_offset, 1);

	printf("frame_offset=%d macroblock_offset=%d\n",frame_offset, mb_offset);
	// Get the macroblock
	getMbFromStream(ARG_VIDEO_PATH1, frame_offset, mb_offset, &mb, NULL);

	// Calculate hash of macroblock
	sha256_string(mb.mb_data, mb.mb_size, outputBuffer);
	printf("Hash of macroblock=%s\n", outputBuffer);
	if(mb.mb_data) free(mb.mb_data);
}
