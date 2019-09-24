#ifndef EXTOOLS_DECODE_H264_MB_H_
#define EXTOOLS_DECODE_H264_MB_H_

int get_mb_from_stream(const char *file_name, int key_frame_num, int mb_num, unsigned char *pRawY, bool verbose);

#endif /* EXTOOLS_DECODE_H264_MB_H_ */
