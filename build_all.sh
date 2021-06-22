#!/bin/sh
set -e

if [ ! -x tools/make/make -a ! -x tools/make/make.exe ]; then
    (cd tools/make; ./build.sh)
fi

if [ ! -x tools/ruby/ruby -a ! -x tools/ruby/ruby.exe ]; then
    (cd tools/ruby; ./build.sh)
fi

. ./env
export MAKEFLAGS="-j$(nproc)"

(cd sdk/NitroSDK; make)
(cd sdk/NitroSystem; make)
(cd sdk/NitroWiFi; make)
(cd sdk/NitroDWC; make)
make
