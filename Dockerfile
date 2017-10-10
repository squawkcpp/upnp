# Dockerfile to install upnp server (UPNP)
FROM ubuntu:xenial

RUN apt-get -y update

ADD build/upnp_0.1.1.deb /upnp_0.1.1.deb

RUN apt-get install -y libpcrecpp0v5 libimlib2 libavcodec-ffmpeg56 libavformat-ffmpeg56 libavutil-ffmpeg54 libpoppler-cpp0 libboost-filesystem1.58.0 \
        libcurl3 libhiredis0.13 libev4 libmagic1 libopencv-highgui2.4v5 \
&& dpkg -i /upnp_0.1.1.deb

ADD script/upnp.sh /upnp.sh
RUN chmod +x /*.sh

EXPOSE 9003

CMD ["/upnp.sh"]
