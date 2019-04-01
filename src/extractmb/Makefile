CFLAGS = -g -D__STDC_CONSTANT_MACROS
LDFLAGS = -lswscale -lavformat -lavcodec -lswresample -lavutil -lpthread -lbz2 -lz -lc -lrt
INSTALLED_DEPS = -Idependencies/include -Ldependencies/lib

extractmb: extractmb.o h264_slice_data.o h264_nal.o h264_sei.o h264_stream.o
	gcc  -o extractmb extractmb.o h264_slice_data.o h264_nal.o h264_sei.o h264_stream.o $(CFLAGS) $(LDFLAGS) $(INSTALLED_DEPS)

clean:
	rm extractmb  *.o
