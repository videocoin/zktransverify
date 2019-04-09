include $(SRC_PATH)/ffbuild/config.mak
#include $(SRC_PATH)/ffbuild/common.mak

FFLIBS := -lavfilter -lswscale -lpostproc -lavformat -lavcodec -lswresample -lavutil -lX11 -lm  -lvdpau -lva -lva-drm  -lvdpau -lva -lX11 -lva-x11 -lpthread -lm -ldl -lbz2 

decodemb: decode-h264-mb.o
	$(CC)  -g -o decodemb decode-h264-mb.o $(LDFLAGS) $(LDEXEFLAGS) $(FFLIBS) $(EXTRALIBS-avcodec) $(EXTRALIBS-avfilter) 

clean: 
	rm decodemb decode-h264-mb.o
