#! make -f
#----------------------------------------------------------------------------
# Project:  スクリプト
# File:     Makefile
#
# Copyright 2005 GameFreak.inc  All rights reserved.
#
#----------------------------------------------------------------------------

SUBDIRS		=

#DEPENDHEADERS	=	scr_seq_def.h	usescript.h
DEPENDHEADERS	=	usescript.h
include	scrfile.lst
include msg_list

include	$(NITROSDK_ROOT)/build/buildtools/commondefs.cctype.CW
include	$(NITROSYSTEM_ROOT)/build/buildtools/commondefs

#Makeで生成される*.bin/*.narcはmake cleanの削除対象にする
LDIRT_CLEAN	= $(SCRSRCS:.ev=.bin) scr_seq.narc elf2bin.exe

IPATH	=	-I../eventdata
#----------------------------------------------------------------------------

include	$(NITROSYSTEM_ROOT)/build/buildtools/modulerules

do-build: $(SCRSRCS:.ev=.bin)

common_scr.bin:	../../../include/msgdata/msg_common_scr.h

%.bin: %.ev elf2bin.exe $(DEPENDHEADERS)
	@echo $<
	@cp $< temp_$*.s
	@$(MWAS) $(IPATH) temp_$*.s -o $*.o
	@$(MWLD) -dis -o $*.elf $*.o
	@./elf2bin.exe $*.elf
	@rm $*.o $*.elf temp_$*.s

elf2bin.exe: ../../battle/skill/elf2bin.c
	$(CC_X86) -o $@ $<

do-build: scr_seq.narc
scr_seq.narc: $(SCRSRCS:.ev=.bin)
	nnsarc -i -c -l -n scr_seq.narc $(SCRSRCS:.ev=.bin) > arc_result.txt

mklst:
	ls -1 *.ev > temp_$*
	listmk temp_$* scrfile.lst SCRSRCS
	rm temp_$*

mkmsglist:
	cat $(MSGLIST) > msglist.h

diff:
	-cp scr_seq.narc old.narc
	$(MAKE)
	-fc /b scr_seq.narc old.narc
