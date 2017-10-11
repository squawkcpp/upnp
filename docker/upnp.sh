#! /bin/bash -e

PATH=/bin:/usr/bin:/sbin:/usr/sbin

OPTIONS=""

if [ -n "$REDIS" ]; then
    OPTIONS="$OPTIONS --redis $REDIS"
fi

if [ -n "$REDIS_PORT" ]; then
    OPTIONS="$OPTIONS --redis-port $REDIS_PORT"
fi

if [ -n "$MULTICAST_ADDRESS" ]; then
    OPTIONS="$OPTIONS --multicast $MULTICAST_ADDRESS"
fi

if [ -n "$MULTICAST_PORT" ]; then
    OPTIONS="$OPTIONS --multicast-port $MULTICAST_PORT"
fi

if [ -n "$NAME" ]; then
    OPTIONS="$OPTIONS --name$NAME"
fi

if [ -n "$CDS_URI" ]; then
    OPTIONS="$OPTIONS --cds $CDS_URI"
fi

if [ -n "$LISTEN" ]; then
    OPTIONS="$OPTIONS --listen $LISTEN"
fi

if [ -n "$HTTP_PORT" ]; then
    OPTIONS="$OPTIONS --http-port $HTTP_PORT"
fi

echo "start /usr/local/bin/upnp $OPTIONS"
/usr/local/bin/upnp $OPTIONS
