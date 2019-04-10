include $(SRC_PATH)/ffbuild/config.mak
#include $(SRC_PATH)/ffbuild/common.mak

FFLIBS := -lavfilter -lswscale -lpostproc -lavformat -lavcodec -lswresample -lavutil -lX11 -lm  -lvdpau -lva -lva-drm  -lvdpau -lva -lX11 -lva-x11 -lpthread -lm -ldl -lbz2 -lcrypto 

genproof: decode-h264-mb.o genproof.o
	$(CC)  -g -o genproof genproof.o decode-h264-mb.o sha256-util.o  $(LDFLAGS) $(LDEXEFLAGS) $(FFLIBS) $(EXTRALIBS-avcodec) $(EXTRALIBS-avfilter) 

genwitness: sha256-util.o decode-h264-mb.o genwitness.o
	$(CC)  -g -o genwitness  genwitness.o decode-h264-mb.o sha256-util.o $(LDFLAGS) $(LDEXEFLAGS) $(FFLIBS) $(EXTRALIBS-avcodec) $(EXTRALIBS-avfilter)

all: genproof genwitness

clean: 
	rm genproof genproof.o decode-h264-mb.o sha256-util.o
	rm genwitness genwitness.o 
