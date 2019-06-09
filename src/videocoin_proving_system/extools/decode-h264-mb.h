
#ifndef EXTOOLS_DECODE_H264_MB_H_
#define EXTOOLS_DECODE_H264_MB_H_

typedef struct _MB_T
{
	char mb_data[16 * 16 * 2]; // Macroblock DCT Coefficients
                               // 1 planes of 16x16 16bit coefficients

	int mb_type;
	int intra16x16_pred_mode;
	int mb_x;
	int mb_y;
    int mb_xy;
    int mb_width;
    int mb_field_decoding_flag;
    int deblocking_filter;

    uint8_t         luma_decoded[16 * 16]; // will be used for debug. Circuit should generate identical data to this

    // This data will be used to decode macroblock
    uint8_t         top_border[8 + 16 + 8];
    uint8_t         luma_top[8 + 16 + 8];
    uint8_t         luma_left[16];
} MB_T;


void hexDump (unsigned char *pData, int n);
void hexDump(unsigned char *pData, int n, int row_len);
int getMbFromStream(const char *file_name, int key_frame_num, int mb_num, MB_T *pMb, unsigned char *pRawY, bool verbose);

#endif /* EXTOOLS_DECODE_H264_MB_H_ */
