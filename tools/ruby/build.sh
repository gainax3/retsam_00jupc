#!/bin/sh
set -e

export MAKEFLAGS="-j$(nproc)"
export CFLAGS="-march=native -O2 -pipe -w"

cwd="$PWD"

rm -rf ruby-1.8.7-p374
tar xf ruby-1.8.7-p374.tar.bz2
cd ruby-1.8.7-p374
patch -p0 < ../patch/fix-cygwin-longjmp.patch
patch -p0 < ../patch/fix-cygwin64.patch
rm -rf ext/openssl ext/tk ext/curses
./configure --bindir="$cwd" --libdir="$cwd/lib" --with-static-linked-ext
make
./miniruby instruby.rb --extout=.ext -i bin-arch -i lib -i ext-comm
cd ..
rm -rf ruby-1.8.7-p374
