#! make -f
#----------------------------------------------------------------------------
# Project:  トレーナーグラフィック
# File:     Makefile
#
# Copyright 2005 GameFreak.inc  All rights reserved.
#
#----------------------------------------------------------------------------

SUBDIRS		=

NCGFILES:=$(wildcard *.ncg)
NCEFILES:=$(wildcard *.nce)
NCLFILES:=$(wildcard *.ncl)

#MAKELOG	= >> log

CP_RES_DIR = ..\\..\\..\\..\\pl_data\\graphic\\trgra\\
RES_DIR	= .

include	$(NITROSDK_ROOT)/build/buildtools/commondefs.cctype.CW
include	$(NITROSYSTEM_ROOT)/build/buildtools/commondefs

#Makeで生成される*.bin/*.narcはmake cleanの削除対象にする
LDIRT_CLEAN	= *.NCGR *.NCER *.NCLR *.NCBR trfgra.narc trbgra.narc

#----------------------------------------------------------------------------

include	$(NITROSYSTEM_ROOT)/build/buildtools/modulerules

.SUFFIXES:.ncg.nce.ncl.NCGR.NCER.NCLR

do-build:res_cp
res_cp:
	xcopy /d /y $(CP_RES_DIR)*.ncg $(RES_DIR)
	xcopy /d /y $(CP_RES_DIR)*.nce $(RES_DIR)
	xcopy /d /y $(CP_RES_DIR)*.ncl $(RES_DIR)
	xcopy /d /y $(CP_RES_DIR)*.scr $(RES_DIR)

%.nce : %.ncg
	echo タイムスタンプが違ってます $*
	cmd /c pause

do-build: $(NCEFILES:.nce=.NCER)
%.NCER: %.ncg %.nce %.ncl
	../../../convert/clact_conv/clact_conv_cui $*.nce $*.ncg $*.ncl -vta -oua $(MAKELOG)

do-build: $(NCGFILES:.ncg=.NCBR)
%.NCBR: %.ncg
	ncgcnv $< 0 0

do-build: trfgra.narc
trfgra.narc: $(NCGFILES:.ncg=.NCGR) $(NCEFILES:.nce=.NCER) $(NCLFILES:.ncl=.NCLR) arctrf.scr
	nnsarc -c -l -n -i trfgra.narc -S arctrf.scr

do-build: trbgra.narc
trbgra.narc: $(NCGFILES:.ncg=.NCGR) $(NCEFILES:.nce=.NCER) $(NCLFILES:.ncl=.NCLR) arctrb.scr
	nnsarc -c -l -n -i trbgra.narc -S arctrb.scr

