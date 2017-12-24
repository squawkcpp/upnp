#!/bin/bash

sudo docker pull spielhuus/clang:latest
sudo docker run -itd --name build_upnp -v $(pwd):/repo -v $(pwd)/build:/build spielhuus/clang
sudo docker exec build_upnp /usr/sbin/pacman --noconfirm -S boost doxygen hiredis libev hdf5 make git

sudo docker exec build_upnp cmake -H/repo -B/build -G Ninja -DCMAKE_MAKE_PROGRAM=/usr/bin/ninja -DCDS_TAG_VERSION=master
sudo docker exec build_upnp cmake --build /build
sudo docker exec build_upnp /build/test_upnp
sudo docker exec build_upnp cmake --build /build --target package

sudo docker build -f docker/Dockerfile --build-arg CDS_TAG_VERSION=master -t upnp .

sudo docker rm -f build_upnp
sudo rm -rf build


