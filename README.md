# rtspstreamer

### Stream RTSP from videofile

### Run with
`docker run --name=rtspstreamer -d -v input.mp4:/rtspstreamer/input.mp4 -e RTSP_OUT_PORT=8554 -p 8554:8554 rtspstreamer`
