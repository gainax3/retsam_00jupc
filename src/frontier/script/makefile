#! make -f
#----------------------------------------------------------------------------
# Project:  フロンティア用スクリプト
# File:     Makefile
#
# Copyright 2005 GameFreak.inc  All rights reserved.
#
#----------------------------------------------------------------------------

SUBDIRS		=

SCRIPTSRCS:=$(wildcard *.s)

ELF2BIN	=	../../../convert/eventdata/elf2bin.exe

include	$(NITROSDK_ROOT)/build/buildtools/commondefs.cctype.CW
include	$(NITROSYSTEM_ROOT)/build/buildtools/commondefs

#Makeで生成される*.bin/*.narcはmake cleanの削除対象にする
LDIRT_CLEAN	= $(SCRIPTSRCS:.s=.bin) fr_script.narc fr_script.naix

#----------------------------------------------------------------------------

include	$(NITROSYSTEM_ROOT)/build/buildtools/modulerules

do-build: $(SCRIPTSRCS:.s=.bin)
%.bin: %.s ../frontier_seq_def.h ../../data/sound/pl_sound_data.sadl
	$(MWAS) $< -o $*.o
	$(MWLD) -dis -o $*.elf $*.o
	$(ELF2BIN) $*.elf
#	rm $*.o
#	rm $*.elf

do-build: fr_script.narc
fr_script.narc: $(SCRIPTSRCS:.s=.bin) ../../data/sound/pl_sound_data.sadl
	nnsarc -c -l -n -i fr_script.narc *.bin

