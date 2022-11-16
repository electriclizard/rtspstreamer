#!/bin/sh
gcc -o rtspstreamer $(pkg-config --cflags gstreamer-1.0 gstreamer-rtsp-server-1.0) \
    $(pkg-config --libs gstreamer-1.0 gstreamer-rtsp-server-1.0) main.c
