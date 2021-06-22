#!/bin/sh

LENGTH=$((0x10000))
echo $LENGTH

hexdump -C -n $LENGTH pokeplatinum.jp.nds > baserom_platinum_j.txt # can create an empty baserom.txt if no baserom.gba
hexdump -C -n $LENGTH ../bin/ARM9-TS/Rom/main.srl > main.txt

diff -u baserom_platinum_j.txt main.txt > compare_dump.dump
#split -l 10000 compare_dump.dump compare_dump_out.dump
