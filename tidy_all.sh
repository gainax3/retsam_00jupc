#!/bin/sh
set -e

. ./env
export MAKEFLAGS="-j$(nproc)"

make tidy
(cd sdk/NitroDWC; make clobber)
(cd sdk/NitroWiFi; make clobber)
(cd sdk/NitroSystem; make clobber)
(cd sdk/NitroSDK; make clobber)

(cd platinum_rs; make clean)
