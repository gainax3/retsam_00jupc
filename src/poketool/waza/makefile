#! make -f
#----------------------------------------------------------------------------
# Project:  技データ
# File:     Makefile
#
# Copyright 2005 GameFreak.inc  All rights reserved.
#
#----------------------------------------------------------------------------

SUBDIRS		=

CP_WAZA_RES_DIR = ..\\..\\..\\..\\pl_data\\data\\wazatbl\\
RES_WAZA_DIR	= .\\

include	$(NITROSDK_ROOT)/build/buildtools/commondefs.cctype.CW
include	$(NITROSYSTEM_ROOT)/build/buildtools/commondefs

#Makeで生成される*.bin/*.narcはmake cleanの削除対象にする
LDIRT_CLEAN	= pl_waza_tbl.narc

#----------------------------------------------------------------------------

include	$(NITROSYSTEM_ROOT)/build/buildtools/modulerules


do-build: res_cp pl_waza_tbl.narc

#----------- サーバーからコピーする -----------
res_cp:
	xcopy /d /y $(CP_WAZA_RES_DIR)*.bin $(RES_WAZA_DIR)

#----------- アーカイブする -----------
pl_waza_tbl.narc: *.bin
	cmd /c mkbin.bat
