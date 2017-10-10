#! /bin/bash -e

PATH=/bin:/usr/bin:/sbin:/usr/sbin

/usr/local/bin/upnp $UPNP_OPTS

tail -f /var/log/dmesg
