#!/bin/sh
set -e

. ./env
export MAKEFLAGS="-j$(nproc)"

make clean
(cd sdk/NitroDWC; make clobber)
(cd sdk/NitroWiFi; make clobber)
(cd sdk/NitroSystem; make clobber)
(cd sdk/NitroSDK; make clobber)

(cd platinum_rs; make clean)

rm -rf tools/make/make tools/make/make.exe
rm -rf tools/ruby/ruby tools/ruby/ruby.exe tools/ruby/rubyw.exe tools/ruby/cygruby18.dll tools/ruby/lib/
