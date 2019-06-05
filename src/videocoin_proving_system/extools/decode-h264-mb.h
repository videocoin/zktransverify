
#ifndef EXTOOLS_DECODE_H264_MB_H_
#define EXTOOLS_DECODE_H264_MB_H_

typedef struct _MB_T
{
	char *mb_data; // Macroblock DCT Coefficients
                   // 3 planes of 16x16 32bit coefficients
	int  mb_size;

	int mb_type;
	int intra16x16_pred_mode;

    uint8_t         luma_decoded[16 * 16]; // will be used for debug. Circuit should generate identical data to this

    // This data will be used to decode macroblock
    uint8_t         luma_neighbour_top[16];
    uint8_t         luma_neighbour_left[16];
    uint8_t         luma_neighbour_left_top; // from plane prediction mode
} MB_T;


void hexDump (unsigned char *pData, int n);
int getMbFromStream(const char *file_name, int key_frame_num, int mb_num, MB_T *pMb, unsigned char *pRawY, bool verbose);

#endif /* EXTOOLS_DECODE_H264_MB_H_ */
