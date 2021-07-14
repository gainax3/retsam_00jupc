#!/bin/bash
set -e

. ./env
export NITRO_FINALROM=yes
export PM_VERSION
export PM_DEBUG
export PM_LANG

if [ "$1" == "-j" ]; then
	export MAKEFLAGS="-j$(nproc)"
fi

export BUILDING_SDK=0
make
