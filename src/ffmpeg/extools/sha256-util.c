#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "openssl/sha.h"
#include "sha256-util.h"


void sha256_hash_string (unsigned char hash[SHA256_DIGEST_LENGTH], char outputBuffer[65]);

void sha256_string(unsigned char *pData, int len, char outputBuffer[65])
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, pData, len);
    SHA256_Final(hash, &sha256);
    int i = 0;
    for(i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
    }
    outputBuffer[64] = 0;
}

void sha256_hash_string (unsigned char hash[SHA256_DIGEST_LENGTH], char outputBuffer[65])
{
    int i = 0;

    for(i = 0; i < SHA256_DIGEST_LENGTH; i++){
        sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
    }
    outputBuffer[64] = 0;
}

int sha256_file(char *path, char outputBuffer[65])
{
    FILE *file = fopen(path, "rb");
    if(!file) return -534;

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    const int bufSize = 32768;
    unsigned char *buffer = malloc(bufSize);
    int bytesRead = 0;
    if(!buffer) return -1;
    while((bytesRead = fread(buffer, 1, bufSize, file))) {
        SHA256_Update(&sha256, buffer, bytesRead);
    }
    SHA256_Final(hash, &sha256);

    sha256_hash_string(hash, outputBuffer);
    fclose(file);
    free(buffer);
    return 0;
}

int getRandomOffsets(
		char *ARG_VIDEO_PATH1,
		char *ARG_VIDEO_PATH2,
		int *frameOffset,
		int frame_range,
		int *mbOffset,
		int mb_range
		)
{
	unsigned char hashSrc[65];
	unsigned char hashTrans[65];
	sha256_file(ARG_VIDEO_PATH1, hashSrc);
	printf("hashSrc=%s\n", hashSrc);
	sha256_file(ARG_VIDEO_PATH2, hashTrans);
	printf("hashTrans=%s\n", hashTrans);

	int frame0 = strtol(hashSrc+62, NULL, 16); // Last two bytes
	int frame1 = strtol(hashTrans+62, NULL, 16); // Last two bytes
	*frameOffset = (frame0 + frame1) % frame_range;

	int mb0 = strtol(hashSrc+60, NULL, 16); // Last 4 bytes
	int mb1 = strtol(hashTrans+60, NULL, 16); // Last 4 bytes
	*mbOffset =  (mb0 + mb1) % mb_range;

	return 0;
}

