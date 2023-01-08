#!/bin/sh
set -e

. ./env
(cd sdk/cw/ARM_EABI_Support; make clean)

. ./env_path.sh

export MAKEFLAGS="-j$(nproc)"

make clean
(cd sdk/NitroDWC; make clobber)
(cd sdk/NitroWiFi; make clobber)
(cd sdk/NitroSystem; make clobber)
(cd sdk/NitroSDK; make clobber)

(cd platinum_rs; make clean)
