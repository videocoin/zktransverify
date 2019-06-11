
#ifndef EXTOOLS_DECODE_H264_MB_H_
#define EXTOOLS_DECODE_H264_MB_H_

typedef union _MB_DC {
    uint8_t u8[16 * 16 * 2];
    int16_t i16[16 * 16];
} MB_DC;

typedef union _MB_LUMA_DC {
    uint8_t u8[16 * 2];
    int16_t i16[16];
} MB_LUMA_DC;

typedef struct _MB_T
{
	int mb_type;
	int mb_x;
	int mb_y;
    int mb_xy;
    int mb_width;

    MB_DC mb;  // Macroblock DCT Coefficients
                                // 1 planes of 16x16 16bit coefficients
    MB_LUMA_DC mb_luma_dc;
    int dequant_coeff;
    uint8_t non_zero_count_cache[15 * 8];

    // This data will be used to decode macroblock
    uint8_t top_border[8 + 16 + 8];
    uint8_t luma_top[8 + 16 + 8];
    uint8_t luma_left[16];
    int mb_field_decoding_flag;
    int deblocking_filter;
    int intra16x16_pred_mode;
} MB_T;


void hexDump (unsigned char *pData, int n);
int getMbFromStream(const char *file_name, int key_frame_num, int mb_num, MB_T *pMb, unsigned char *pRawY, bool verbose);

#endif /* EXTOOLS_DECODE_H264_MB_H_ */
