#! make -f
#----------------------------------------------------------------------------
# Project:  技エフェクト
# File:     Makefile
#
# Copyright 2005 GameFreak.inc  All rights reserved.
#
#----------------------------------------------------------------------------

SUBDIRS		=

RES_BG_DIR	= .\\bg\\
RES_OBJ_DIR	= .\\obj\\

NITRO_COMP	= ./../../../convert/nitro_comp/nitro_comp.exe

include	$(NITROSDK_ROOT)/build/buildtools/commondefs.cctype.CW
include	$(NITROSYSTEM_ROOT)/build/buildtools/commondefs


#Makeで生成される*.binはmake cleanの削除対象にする
LDIRT_CLEAN	= frontier_bg.narc frontier_obj.narc

#----------------------------------------------------------------------------

include	$(NITROSYSTEM_ROOT)/build/buildtools/modulerules

#-------------------------------
#	実行順
#		リソースコピー
#		リソースアーカイブ
#		技エフェクトアーカイブ
#-------------------------------
do-build: frontier_bg.narc frontier_obj.narc

#----------- アーカイブする -----------
frontier_bg.narc: $(RES_BG_DIR:\\=/)*.N*R frontier_bg.lst
#	nnsarc -c -l -n -i frontier_bg.narc -S frontier_bg.lst
	$(NITRO_COMP) $(NITROSDK_ROOT) frontier_bg.lst frontier_bg.narc frontier_bg.naix

frontier_obj.narc: $(RES_OBJ_DIR:\\=/)*.N*R frontier_obj.lst
#	nnsarc -c -l -n -i frontier_obj.narc -S frontier_obj.lst
	$(NITRO_COMP) $(NITROSDK_ROOT) frontier_obj.lst frontier_obj.narc frontier_obj.naix

