#! make -f
#----------------------------------------------------------------------------
# Project:  プラチナ用：バトルタワーデータ
# File:     Makefile
#
# Copyright 2005 GameFreak.inc  All rights reserved.
#
#----------------------------------------------------------------------------

BTDMAKEDIR = ./../../../../pl_conv/btdcnv/

SUBDIRS		=

include	$(NITROSDK_ROOT)/build/buildtools/commondefs.cctype.CW
include	$(NITROSYSTEM_ROOT)/build/buildtools/commondefs

#Makeで生成される*.bin/*.narcはmake cleanの削除対象にする
LDIRT_CLEAN	= *.s pl_btdpm.narc pl_btdtr.narc

#----------------------------------------------------------------------------

include	$(NITROSYSTEM_ROOT)/build/buildtools/modulerules

do-build: pl_btdpm.narc
pl_btdpm.narc: btf_pla_pm.txt
	$(BTDMAKEDIR)btdmakepm btf_pla_pm.txt $(BTDMAKEDIR)
	$(MAKE) -f btdpmmake

do-build: pl_btdtr.narc
pl_btdtr.narc: btf_pla_tr.txt
	$(BTDMAKEDIR)btdmaketr btf_pla_tr.txt $(BTDMAKEDIR)
	$(MAKE) -f btdtrmake
