#! make -f
#----------------------------------------------------------------------------
# Project:  技シーケンス
# File:     Makefile
#
# Copyright 2005 GameFreak.inc  All rights reserved.
#
#----------------------------------------------------------------------------

SUBDIRS		=

POKEGRA:=$(wildcard *_m.ncg)
POKECOL:=$(wildcard *_n.ncl)

CP_RES_DIR = ..\\..\\..\\..\\pl_data\\graphic\\pokegra\\
CP_RES_DIR2 = ..\\..\\..\\..\\pl_conv\\pmgracnv\\
RES_DIR	= .
RES_DIR2= c:\\tools\\

include	$(NITROSDK_ROOT)/build/buildtools/commondefs.cctype.CW
include	$(NITROSYSTEM_ROOT)/build/buildtools/commondefs

#Makeで生成される*.bin/*.narcはmake cleanの削除対象にする
LDIRT_CLEAN	= $(POKEGRA:.ncg=.NCBR) $(POKECOL:.ncl=.NCLR) $(OTHERPOKE:.ncg=.NCBR) pl_pokegra.narc pl_otherpoke.narc

#----------------------------------------------------------------------------

include	$(NITROSYSTEM_ROOT)/build/buildtools/modulerules

.suffixes:.ncg.ncl.NCBR.NCLR

do-build:res_cp
res_cp:
	xcopy /d /y $(CP_RES_DIR)*.ncg $(RES_DIR)
	xcopy /d /y $(CP_RES_DIR)*.ncl $(RES_DIR)
	xcopy /d /y $(CP_RES_DIR)*.scr $(RES_DIR)
	xcopy /d /y $(CP_RES_DIR)*.dat $(RES_DIR)
	xcopy /d /y $(CP_RES_DIR2)pmncgcnv.exe $(RES_DIR2)
	xcopy /d /y $(CP_RES_DIR2)pmnclcnv.exe $(RES_DIR2)

do-build: $(POKEGRA:.ncg=.NCBR)
%.NCBR: %.ncg
	pmncgcnv $< mfratio.scr 0

do-build: $(POKECOL:.ncl=.NCLR)
%.NCLR: %.ncl
	pmnclcnv $<

do-build: pl_pokegra.narc
pl_pokegra.narc: $(POKEGRA:.ncg=.NCBR) $(POKECOL:.ncl=.NCLR) pokegra.scr
	nnsarc -c -l -n -i pl_pokegra.narc -S pokegra.scr
	pl_pokegra.bat

do-build: pl_otherpoke.narc
pl_otherpoke.narc: $(POKEGRA:.ncg=.NCBR) otherpoke.scr
	nnsarc -c -l -n -i pl_otherpoke.narc -S otherpoke.scr
	pl_otherpoke.bat

do-build: height.narc
height.narc: pl_pokegra.narc
	nnsarc -c -l -n height.narc -S height.scr

do-build: height_o.narc
height_o.narc: pl_otherpoke.narc
	nnsarc -c -l -n -i height_o.narc -S height_o.scr
