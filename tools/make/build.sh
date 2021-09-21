#!/bin/sh
set -e

export MAKEFLAGS="-j$(nproc)"
export CFLAGS="-march=native -O2 -pipe -w"

cwd="$PWD"

rm -rf make-3.80-1
tar xf make-3.80-1-src.tar.bz2
cd make-3.80-1
patch -p0 < ../patch/eval-conditional.patch
patch -p0 < ../patch/eval-crash.diff
patch -p0 < ../patch/fix-strnicmp.patch
patch -p0 < ../patch/fix-alloca.patch
patch -p0 < ../patch/fix-cygwin64.patch
./configure --bindir="$cwd" --disable-nls
make
make install-exec
cd ..
rm -rf make-3.80-1
