#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "decode-h264-mb.h"
#include "sha256-util.h"

bool ARG_HELP;
const char* ARG_VIDEO_PATH1 = nullptr;
const char* ARG_VIDEO_PATH2 = nullptr;


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
	if (ARG_HELP || ARG_VIDEO_PATH1 == nullptr || ARG_VIDEO_PATH2 == nullptr) {
		fprintf(stderr,
				"Usage: genproof videoPath1 videoPath2\n  --help and -h will output this help message.\n");
		exit(1);
	}
}

int main(int argc, const char **argv)
{
	unsigned char outputBuffer[65];
	parse_options(argc, argv);
	int frame_offset = 0;
	int mb_offset = 0;

	// Generate hash of hashes from input stream and transcoded stream
	//      and pick random keyframe and macroblock number

	getRandomOffsets(ARG_VIDEO_PATH1, ARG_VIDEO_PATH2, &frame_offset, 10, &mb_offset, 1);

	printf("frame_offset=%d macroblock_offset=%d\n",frame_offset, mb_offset);
	// Get the macroblock
    get_mb_from_stream(ARG_VIDEO_PATH1, frame_offset, mb_offset, nullptr, false);

	// Calculate hash of macroblock
	printf("Hash of macroblock=%s\n", outputBuffer);
}
