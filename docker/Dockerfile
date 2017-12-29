# Dockerfile to install upnp server (UPNP)
FROM busybox:musl

ARG UPNP_TAG_VERSION=master

ADD build/upnp_$UPNP_TAG_VERSION.tar.gz /

ADD docker/upnp.sh /upnp.sh
RUN chmod +x /*.sh

EXPOSE 9003

CMD ["/upnp.sh"]
