# Dockerfile to install upnp server (UPNP)
FROM base/archlinux:latest

ARG UPNP_TAG_VERSION=master

#RUN pacman -Syu --noconfirm && \
#    pacman --noconfirm -S boost doxygen hiredis libev hdf5 make

ADD build/upnp_$UPNP_TAG_VERSION.tar.gz /

ADD docker/upnp.sh /upnp.sh
RUN chmod +x /*.sh

EXPOSE 9003

CMD ["/upnp.sh"]
