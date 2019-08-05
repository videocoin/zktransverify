
#ifndef EXTOOLS_DECODE_H264_MB_H_
#define EXTOOLS_DECODE_H264_MB_H_

typedef struct _MB_T
{
	int mb_type;
	int mb_x;
	int mb_y;
    int mb_xy;
    int mb_width;
    int mb_height;

    int dequant_coeff;
    int mb_field_decoding_flag;
    int deblocking_filter;
    int intra16x16_pred_mode;

    int16_t mb[16 * 16];
    int16_t mb_luma_dc[16];
    uint8_t non_zero_count_cache[15 * 8];

    // This data will be used to decode macroblock
    uint8_t top_border[8 + 16 + 8];
    uint8_t luma_top[8 + 16 + 8];
    uint8_t luma_left[16];
    uint8_t luma_decoded[16*16];
} MB_T;


void hex_dump(unsigned char *pData, int n);
int get_mb_from_stream(const char *file_name, int key_frame_num, int mb_num, MB_T *pMb, bool verbose);

#endif /* EXTOOLS_DECODE_H264_MB_H_ */
