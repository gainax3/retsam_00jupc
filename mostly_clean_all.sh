#!/bin/sh
set -e

. ./env
export PATH="$OLD_PATH"
(cd sdk/cw/ARM_EABI_Support; make clean)
export PATH="$PATH_WITH_TOOLS"

export MAKEFLAGS="-j$(nproc)"

make clean
(cd sdk/NitroDWC; make clobber)
(cd sdk/NitroWiFi; make clobber)
(cd sdk/NitroSystem; make clobber)
(cd sdk/NitroSDK; make clobber)

(cd platinum_rs; make clean)
