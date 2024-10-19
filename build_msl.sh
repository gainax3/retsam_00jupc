#!/bin/bash
set -e

if [ "$1" == "-j" ]; then
	export MAKEFLAGS="-j$(nproc)"
fi

. ./env

export PATH="$OLD_PATH"
(cd sdk/cw/ARM_EABI_Support; make)
export PATH="$PATH_WITH_TOOLS"
