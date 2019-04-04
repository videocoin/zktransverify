#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>


	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
	#include <libswscale/swscale.h>
	#include <libavutil/motion_vector.h>

#include "h264_stream.h"


AVFormatContext* fmt_ctx;
AVCodecContext *codec_ctx = NULL;
AVCodec *pCodec = NULL;
int ffmpeg_videoStreamIndex;
size_t ffmpeg_frameWidth, ffmpeg_frameHeight;
AVBitStreamFilterContext* h264bsfc = NULL;

int frame_offset = 0;
int macroblock_offset = 0;
bool ARG_QUIET, ARG_HELP;
const char* ARG_VIDEO_PATH;

void parse_options(int argc, const char* argv[])
{
	int i = 1;
	while(i < argc)
	{
		if(strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
			ARG_HELP = true;
		else if(strcmp(argv[i], "-q") == 0 || strcmp(argv[i], "--quiet") == 0)
			ARG_QUIET = true;
		else if(strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--frame") == 0) {
			if(i+1 < argc)
				frame_offset = atoi(argv[i+1]);
			i++;
		} else if(strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "--macroblock") == 0){
			if(i+1 < argc)
				macroblock_offset = atoi(argv[i+1]);
			i++;
		}else {
			ARG_VIDEO_PATH = argv[i];
		}
		i++;
	}
	if(ARG_HELP || ARG_VIDEO_PATH == NULL)
	{
		fprintf(stderr, "Usage: mpegflow [--raw | [[--grid8x8] [--occupancy]]] videoPath\n  --help and -h will output this help message.\n  --raw will prevent motion vectors from being arranged in matrices.\n  --grid8x8 will force fine 8x8 grid.\n  --occupancy will append occupancy matrix after motion vector matrices.\n  --quiet will suppress debug output.\n");
		exit(1);
	}
}
void ffmpeg_init()
{
	av_register_all();

	fmt_ctx = avformat_alloc_context();
	ffmpeg_videoStreamIndex = -1;

	int err = 0;

	if ((err = avformat_open_input(&fmt_ctx, ARG_VIDEO_PATH, NULL, NULL)) != 0)
	{
		//throw std::runtime_error("Couldn't open file. Possibly it doesn't exist.");
		exit(1);
	}

	if ((err = avformat_find_stream_info(fmt_ctx, NULL)) < 0)
	{
		//throw std::runtime_error("Stream information not found.");
		exit(1);
	}

	for(int i = 0; i < fmt_ctx->nb_streams; i++)
	{
		codec_ctx = fmt_ctx->streams[i]->codec;
		if( AVMEDIA_TYPE_VIDEO == codec_ctx->codec_type && ffmpeg_videoStreamIndex < 0 )
		{
			pCodec = avcodec_find_decoder(codec_ctx->codec_id);
			AVDictionary *opts = NULL;
			av_dict_set(&opts, "flags2", "+export_mvs", 0);
			if (!pCodec || avcodec_open2(codec_ctx, pCodec, &opts) < 0) {
				//throw std::runtime_error("Codec not found or cannot open codec.");
				exit(1);
			}

			ffmpeg_videoStreamIndex = i;
			ffmpeg_frameWidth = codec_ctx->width;
			ffmpeg_frameHeight = codec_ctx->height;
			if (codec_ctx->codec_id == AV_CODEC_ID_H264 || codec_ctx->codec_id == AV_CODEC_ID_HEVC) {
			    if (codec_ctx->codec_id == AV_CODEC_ID_H264)
			        h264bsfc = av_bitstream_filter_init("h264_mp4toannexb");
			    else
			        h264bsfc = av_bitstream_filter_init("hevc_mp4toannexb");
			}
			break;
		}
	}

	if(ffmpeg_videoStreamIndex == -1){
		//throw std::runtime_error("Video stream not found.");
	}
}
void ffmpeg_deinit()
{
	if (h264bsfc) {
		av_bitstream_filter_close(h264bsfc);
	}
}

void hexDump (unsigned char *pData, int n)
{
	for (int i=0; i< n; i++) {
		printf("%02x ", pData[i]);
	}
	printf("\n");
}


int main(int argc, const char* argv[])
{
	AVPacket *avpkt;
	avpkt = (AVPacket *)av_malloc(sizeof(AVPacket));
	parse_options(argc, argv);
	ffmpeg_init();
	int frame = 0;
	int idr_frame = -1;

    h264_stream_t* h = h264_new();

	while(av_read_frame(fmt_ctx, avpkt) >= 0) {
	    // decode packet and other stuff
		printf("Stream=%d frame=%d idr_frame=%d size=0x%x\n", avpkt->stream_index, frame, idr_frame, avpkt->size);
		hexDump(avpkt->data, 16);
		if (avpkt && avpkt->size && ffmpeg_videoStreamIndex == avpkt->stream_index) {
			if (h264bsfc) {
				av_bitstream_filter_filter(h264bsfc, fmt_ctx->streams[ffmpeg_videoStreamIndex]->codec, NULL, &avpkt->data, &avpkt->size, avpkt->data, avpkt->size, 0);
				hexDump(avpkt->data, 16);
				//read_debug_nal_unit(h, avpkt->data + 4, avpkt->size - 4);
			    uint8_t* p = avpkt->data;
			    size_t sz = avpkt->size;
				int nal_start, nal_end, nalu_type;
				while (find_nal_unit(p, sz, &nal_start, &nal_end) > 0) {
					p += nal_start;
					nalu_type = read_debug_nal_unit(h, p, nal_end - nal_start);
					if(nalu_type == NAL_UNIT_TYPE_CODED_SLICE_IDR) {
						idr_frame++;
					}
					if(idr_frame == frame_offset){
						char *pMbData = malloc(1024);
						int mb_size = get_macroblock(h, p, nal_end - nal_start, macroblock_offset, pMbData, 1024);
						if(mb_size > 0){
							printf("At idr_frame=%d mb=%d\n", idr_frame, macroblock_offset);
							hexDump(pMbData, 16);
						}
						goto Exit;
					}
					p += (nal_end - nal_start);
					sz -= nal_end;
				}
			}
		    frame++;
		}
	    av_packet_unref(avpkt);
	}
Exit:
	ffmpeg_deinit();
}
