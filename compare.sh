#!/bin/sh

#LENGTH=$((0x405e00))
LENGTH=$((0x210000))
START_OFFSET=$((0x4000))
echo $LENGTH

#hexdump -C -n $LENGTH -s $START_OFFSET diff/pokeplatinum.jp.nds > diff/baserom_platinum_j.txt # can create an empty baserom.txt if no baserom.gba
#hexdump -C -n $LENGTH -s $START_OFFSET bin/ARM9-TS/Rom/main.srl > diff/main.txt
hexdump -C diff/pokeplatinum.jp.nds > diff/baserom_platinum_j.txt # can create an empty baserom.txt if no baserom.gba
hexdump -C bin/ARM9-TS/Rom/main.srl > diff/main.txt

diff -u diff/baserom_platinum_j.txt diff/main.txt > diff/compare_dump.dump
#split -l 10000 compare_dump.dump compare_dump_out.dump
