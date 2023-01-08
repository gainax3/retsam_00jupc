#!/bin/bash
set -e

if [ "$1" == "-j" ]; then
	export MAKEFLAGS="-j$(nproc)"
fi

. ./env

(cd sdk/cw/ARM_EABI_Support; make clean)
