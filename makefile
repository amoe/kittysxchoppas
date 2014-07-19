CFLAGS = -g -O0 -Wall $$(pkg-config --cflags gstreamer-1.0 gtk+-3.0 gstreamer-video-1.0)
LDLIBS = $$(pkg-config --libs gstreamer-1.0 gtk+-3.0 gstreamer-video-1.0)

kittysxchoppas: kittysxchoppas.c
