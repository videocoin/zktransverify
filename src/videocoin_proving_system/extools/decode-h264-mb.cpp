/*
 * Copyright (c) 2015 Ludmila Glinskih
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/**
 * H264 codec test.
 */
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

extern "C" {
#include <libavutil/adler32.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libavutil/timestamp.h>
#include <libavutil/base64.h>
}

#include "decode-h264-mb.h"


static char *itoa(int val, int base);

void hexDump(unsigned char *pData, int n, int row_len) {
    for (int i = 0; i < n; i++) {
        if (i % row_len == 0) printf("\n");
        printf("%02x ", pData[i]);
    }
    printf("\n");
}


void hexDump(unsigned char *pData, int n) {
    for (int i = 0; i < n; i++) {
        printf("%02x ", pData[i]);
    }
    printf("\n");
}

static char *itoa(int val, int base) {

    static char buf[32] = {0};
    int i = 30;

    for (; val && i; --i, val /= base)
        buf[i] = "0123456789abcdef"[val % base];

    return &buf[i + 1];
}

int getParam(AVFrame *frame, const char *key) {
    AVDictionaryEntry *ent = av_dict_get(frame->metadata, key, nullptr, 0);
    if (ent) {
        char *value = ent->value;
        if (value) {
            int intValue = atoi(value);
            printf("getParam key=%s value=%s 0x%0x\n", key, value, intValue);
            return intValue;
        }
    }
    return -1;
}

uint8_t *getParam(AVFrame *frame, const char *key, uint8_t *data, size_t data_size) {
    AVDictionaryEntry *ent = av_dict_get(frame->metadata, key, nullptr, 0);
    if (ent) {
        char *value = ent->value;
        if (value) {
            av_base64_decode(data, value, data_size);
            printf("getParam key=%s value=%s\n", key, value);
            return data;
        }
    }
    return data;
}

int getMbFromStream(const char *file_name, int key_frame_num, int mb_num, MB_T *pMb, unsigned char *pRawY, bool verbose) {
    AVFormatContext *fmt_ctx;
    int frame_offset = 0;
    int macroblock_offset = 0;
    int ffmpeg_videoStreamIndex;
    AVBitStreamFilterContext *h264bsfc = NULL;

    static long privateId = 0;
    AVDictionary *frameDict = NULL;

    AVFrame *frame = NULL;
    AVPacket *pkt;
    uint8_t inbuf[1024];
    int frame_count = 0;
    int key_frame_count = 0;

    pkt = (AVPacket *) av_malloc(sizeof(AVPacket));

    int idr_frame = -1;

    //if (video_decode_example(argv[1]) != 0)
    //    return 1;
    AVCodec *codec = avcodec_find_decoder(AV_CODEC_ID_H4MB);
    if (!codec) {
        fprintf(stderr, "Codec not found\n");
        exit(1);
    } else {
        fprintf(stderr, "Codec found\n");
    }
    if (!frame) {
        if (!(frame = av_frame_alloc())) {
            fprintf(stderr, "Could not allocate frame\n");
            exit(1);
        }
    }

    AVCodecContext *c = avcodec_alloc_context3(codec);
    if (!c) {
        fprintf(stderr, "Could not allocate audio codec context\n");
        exit(1);
    }

    c->debug = 1;

    /* open it */
    if (avcodec_open2(c, codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        exit(1);
    }

    av_register_all();

    fmt_ctx = avformat_alloc_context();
    ffmpeg_videoStreamIndex = -1;

    int err = 0;

    if ((err = avformat_open_input(&fmt_ctx, file_name, NULL, NULL)) != 0) {
        fprintf(stderr, "Couldn't open file. Possibly it doesn't exist.");
        exit(1);
    }

//	if ((err = avformat_find_stream_info(fmt_ctx, NULL)) < 0) {
//		fprintf(stderr, "Stream information not found.");
//		exit(1);
//	}

    for (int i = 0; i < fmt_ctx->nb_streams; i++) {
        AVCodecContext *codec_ctx = fmt_ctx->streams[i]->codec;
        if (AVMEDIA_TYPE_VIDEO == codec_ctx->codec_type
            && ffmpeg_videoStreamIndex < 0) {
            AVCodec *pCodec = avcodec_find_decoder(codec_ctx->codec_id);
            AVDictionary *opts = NULL;

            ffmpeg_videoStreamIndex = i;
            if (codec_ctx->codec_id == AV_CODEC_ID_H264) {
                if (codec_ctx->codec_id == AV_CODEC_ID_H264)
                    h264bsfc = av_bitstream_filter_init("h264_mp4toannexb");
            }
            break;
        }
    }

    if (ffmpeg_videoStreamIndex == -1) {
        fprintf(stderr, "Video stream not found.");
    }

    while (av_read_frame(fmt_ctx, pkt) >= 0) {
        // decode packet and other stuff
        printf("Stream=%d frame=%d idr_frame=%d size=0x%x\n", pkt->stream_index,
               frame_count, idr_frame, pkt->size);
        hexDump(pkt->data, 16);
        if (pkt && pkt->size && ffmpeg_videoStreamIndex == pkt->stream_index) {
            if (h264bsfc) {
                av_bitstream_filter_filter(h264bsfc,
                                           fmt_ctx->streams[ffmpeg_videoStreamIndex]->codec, NULL,
                                           &pkt->data, &pkt->size, pkt->data, pkt->size, 0);
                hexDump(pkt->data, 16);
                //read_debug_nal_unit(h, pkt->data + 4, pkt->size - 4);
                uint8_t *p = pkt->data;
                size_t sz = pkt->size;
                int nal_start, nal_end, nalu_type;

                av_frame_unref(frame);
                int got_frame = 0;

                // Add side_data to AVPacket which will be decoded
                av_dict_set_int(&frameDict, "req_mb", mb_num, 0);
                av_dict_set_int(&frameDict, "debug", 0, 0);
                int frameDictSize = 0;
                uint8_t *frameDictData = av_packet_pack_dictionary(frameDict,
                                                                   &frameDictSize);
                av_dict_free(&frameDict);
                av_packet_add_side_data(pkt, AV_PKT_DATA_STRINGS_METADATA,
                                        frameDictData, frameDictSize);

                int ret = avcodec_decode_video2(c, frame, &got_frame, pkt);
                if (ret < 0) {
                    fprintf(stderr, "Failed to decode\n");
                    exit(1);
                } else {
                    fprintf(stderr, "Frame decoded\n");
                }


                if (got_frame) {
                    if (frame->key_frame)
                        key_frame_count++;

                    if (key_frame_count == key_frame_num) {
                        // Free side data from packet
                        av_packet_free_side_data(pkt);

                        if (getParam(frame, "macroblock", (uint8_t *)pMb->mb_data, sizeof(pMb->mb_data)) != nullptr) {
                            if (verbose) {
                                printf("macroblock=");
                                hexDump((unsigned char *) pMb->mb_data, 16 * 16 * 2);

                                if (pRawY) {
                                    int mb_locn_x = 0;
                                    int mb_locn_y = 0;
                                    unsigned char *pDst = pRawY;
                                    for (int v = 0; v < 16; v++) {
                                        unsigned char *pSrc =
                                                frame->data[0] + (mb_locn_y + v) * frame->linesize[0] + mb_locn_x;
                                        for (int h = 0; h < 16; h++) {
                                            *pDst++ = *pSrc++;
                                        }
                                    }
                                    if (verbose)
                                        hexDump(pRawY, 256);
                                }
                            }
                        }
                        getParam(frame, "luma_decoded", pMb->luma_decoded, sizeof(pMb->luma_decoded));
                        getParam(frame, "luma_top", pMb->luma_top, sizeof(pMb->luma_top));
                        getParam(frame, "luma_left", pMb->luma_left, sizeof(pMb->luma_left));
                        getParam(frame, "top_border", pMb->top_border, sizeof(pMb->top_border));

                        pMb->mb_type = getParam(frame, "mb_type");
                        pMb->intra16x16_pred_mode = getParam(frame, "intra16x16_pred_mode");
                        pMb->mb_field_decoding_flag = getParam(frame, "mb_field_decoding_flag");
                        pMb->deblocking_filter = getParam(frame, "deblocking_filter");
                        pMb->mb_x = getParam(frame, "mb_x");
                        pMb->mb_y = getParam(frame, "mb_y");
                        pMb->mb_xy = getParam(frame, "mb_xy");
                        pMb->mb_width = getParam(frame, "mb_width");
                        break;
                    }
                }
                frame_count++;
            }


        }
        av_packet_unref(pkt);
    }
    return 0;
}
