
#ifndef EXTOOLS_DECODE_H264_MB_H_
#define EXTOOLS_DECODE_H264_MB_H_

typedef struct _MB_T
{
	char *mb_data; // Macroblock DCT Coefficients
                   // 3 planes of 16x16 32bit coefficients
	int  mb_size;

	int mb_type;
	int intra16x16_pred_mode;
	// TODO : Add othe required params
} MB_T;

int getMbFromStream(char *file_name, int key_frame_num, int mb_num, MB_T *pMb);

#endif /* EXTOOLS_DECODE_H264_MB_H_ */