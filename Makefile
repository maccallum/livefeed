CC=clang
CFLAGS=-O3

all: CFLAGS=-O3
all: cc_capture cc_captured

cc_capture: INCLUDES=-I/usr/include/gstreamer-1.0 -I/usr/include/glib-2.0 -I/usr/lib/aarch64-linux-gnu/glib-2.0/include
cc_capture: LIBS=-lgstreamer-1.0 -lglib-2.0 -lgobject-2.0
cc_capture: cc_capture.c cc_captured.c
	$(CC) \
	$(CFLAGS) \
	$(INCLUDES) \
	$(LIBS) \
	-o $@ $<

cc_captured: INCLUDES=
cc_captured: LIBS=-lpthread
cc_captured: cc_captured.c cc_capture.c
	$(CC) \
	$(CFLAGS) \
	$(INCLUDES) \
	$(LIBS) \
	-o $@ $<

gst_test: gst_test.c
	$(CC) $(CFLAGS) -o gst_test $(INCLUDES) -lgstreamer-1.0 -lglib-2.0 -lpthread gst_test.c

.PHONY: clean
clean:
	rm -rf cc_capture cc_captured
