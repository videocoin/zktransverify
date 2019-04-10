
#ifndef EXTOOLS_SHA256_UTIL_H_
#define EXTOOLS_SHA256_UTIL_H_


void sha256_string(unsigned char *pData, int len, char outputBuffer[65]);
int sha256_file(char *path, char outputBuffer[65]);

int getRandomOffsets(
		char *ARG_VIDEO_PATH1,
		char *ARG_VIDEO_PATH2,
		int *frameOffset,
		int frame_range,
		int *mbOffset,
		int mb_range
		);
#endif /* EXTOOLS_SHA256_UTIL_H_ */
