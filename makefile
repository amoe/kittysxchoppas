CFLAGS = -g -O0 -Wall $$(pkg-config --cflags gstreamer-1.0 gtk+-3.0 gstreamer-video-1.0)
LDLIBS = $$(pkg-config --libs gstreamer-1.0 gtk+-3.0 gstreamer-video-1.0) -lm

prefix = /usr/local
bindir = $(prefix)/bin

kittysxchoppas: kittysxchoppas.c

install: kittysxchoppas
	cp -v kittysxchoppas $(bindir)
	chmod +x $(bindir)/kittysxchoppas
