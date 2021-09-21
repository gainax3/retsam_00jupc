#! make -f
#----------------------------------------------------------------------------
# Project:  プラチナ用：バトルステージデータ
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
LDIRT_CLEAN	= *.s pl_stdpm.narc

#----------------------------------------------------------------------------

include	$(NITROSYSTEM_ROOT)/build/buildtools/modulerules

do-build: pl_bsdpm.narc
pl_bsdpm.narc: btf_pm_st.txt
	$(BTDMAKEDIR)btdmakepm btf_pm_st.txt $(BTDMAKEDIR)
	$(MAKE) -f bsdpmmake

