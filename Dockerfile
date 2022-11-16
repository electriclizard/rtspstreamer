FROM archlinux:base-devel

WORKDIR /rtspstreamer
RUN pacman --noconfirm -Syy wget gst-plugins-bad gst-plugins-bad-libs gst-plugins-base gst-plugins-base-libs \
    gst-plugins-good gst-plugins-ugly gst-rtsp-server gstreamer
COPY ./rtspstreamer .
RUN ./build.sh

ENV RTSP_OUT_PORT=8554

CMD -O /rtspstreamer/input.mp4 $VIDEO_URL && ./rtspstreamer -i file:///rtspstreamer/input.mp4 -p $RTSP_OUT_PORT
