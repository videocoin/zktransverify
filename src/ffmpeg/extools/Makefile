include $(SRC_PATH)/ffbuild/config.mak
#include $(SRC_PATH)/ffbuild/common.mak

FFLIBS := -lavfilter -lswscale -lpostproc -lavformat -lavcodec -lswresample -lavutil -lX11 -lm  -lvdpau -lva -lva-drm  -lvdpau -lva -lX11 -lva-x11 -lpthread -lm -ldl -lbz2 

genproof: decode-h264-mb.o
	$(CC)  -g -o genproof decode-h264-mb.o $(LDFLAGS) $(LDEXEFLAGS) $(FFLIBS) $(EXTRALIBS-avcodec) $(EXTRALIBS-avfilter) 

clean: 
	rm genproof decode-h264-mb.o
