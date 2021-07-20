#!/bin/bash
set -e

if [ ! -x tools/make/make -a ! -x tools/make/make.exe ]; then
    (cd tools/make; ./build.sh)
fi

if [ ! -x tools/ruby/ruby -a ! -x tools/ruby/ruby.exe ]; then
    (cd tools/ruby; ./build.sh)
fi

. ./env
export NITRO_FINALROM=yes
export PM_VERSION
export PM_DEBUG
export PM_LANG

if [ "$1" == "-j" ]; then
	export MAKEFLAGS="-j$(nproc)"
fi

export BUILDING_SDK=1
(cd sdk/NitroSDK; make)
(cd sdk/NitroSystem; make)
(cd sdk/NitroWiFi; make)
(cd sdk/NitroDWC; make)

(cd platinum_rs; make)

export BUILDING_SDK=0
make
