#!/bin/sh

. ./env
export MAKEFLAGS="-j$(nproc)"

make clean
(cd sdk/NitroDWC; make clobber)
(cd sdk/NitroWiFi; make clobber)
(cd sdk/NitroSystem; make clobber)
(cd sdk/NitroSDK; make clobber)

rm -rf tools/make/make tools/make/make.exe
rm -rf tools/ruby/ruby tools/ruby/ruby.exe tools/ruby/lib/
