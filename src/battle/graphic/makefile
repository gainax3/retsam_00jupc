#! make -f
#----------------------------------------------------------------------------
# Project:  技エフェクト
# File:     Makefile
#
# Copyright 2005 GameFreak.inc  All rights reserved.
#
#----------------------------------------------------------------------------

SUBDIRS		=

CP_BG_RES_DIR = ..\\..\\..\\..\\pl_data\\graphic\\batt_bg\\
CP_OBJ_RES_DIR = ..\\..\\..\\..\\pl_data\\graphic\\batt_obj\\
CP_TOOL_RES_DIR = ..\\..\\..\\..\\pl_conv\\pmgracnv\\
RES_BG_DIR	= .\\batt_bg\\
RES_OBJ_DIR	= .\\batt_obj\\
RES_TOOL_DIR	= c:\\tools\\

NITRO_COMP	= ./../../../convert/nitro_comp/nitro_comp.exe

include	$(NITROSDK_ROOT)/build/buildtools/commondefs.cctype.CW
include	$(NITROSYSTEM_ROOT)/build/buildtools/commondefs


#Makeで生成される*.binはmake cleanの削除対象にする
LDIRT_CLEAN	= pl_batt_bg.narc pl_batt_obj.narc

#----------------------------------------------------------------------------

include	$(NITROSYSTEM_ROOT)/build/buildtools/modulerules

#-------------------------------
#	実行順
#		リソースコピー
#		リソースアーカイブ
#		技エフェクトアーカイブ
#-------------------------------
do-build: res_cp pl_batt_bg.narc pl_batt_obj.narc

#----------- サーバーからコピーする -----------
res_cp:
	xcopy /d /y $(CP_BG_RES_DIR)*.NCGR $(RES_BG_DIR)
	xcopy /d /y $(CP_BG_RES_DIR)*.NCLR $(RES_BG_DIR)
	xcopy /d /y $(CP_BG_RES_DIR)*.NSCR $(RES_BG_DIR)
	xcopy /d /y $(CP_OBJ_RES_DIR)*.NCGR $(RES_OBJ_DIR)
	xcopy /d /y $(CP_OBJ_RES_DIR)*.NCER $(RES_OBJ_DIR)
	xcopy /d /y $(CP_OBJ_RES_DIR)*.NCLR $(RES_OBJ_DIR)
	xcopy /d /y $(CP_OBJ_RES_DIR)*.NANR $(RES_OBJ_DIR)
	xcopy /d /y $(CP_TOOL_RES_DIR)battbgcnv.exe $(RES_TOOL_DIR)

#----------- アーカイブする -----------
pl_batt_bg.narc: $(RES_BG_DIR:\\=/)*.N*R batt_bg.lst
#	nnsarc -c -l -n -i pl_batt_bg.narc -S batt_bg.lst
	battbgcnv $(RES_BG_DIR)battle_bg00.NCGR
	battbgcnv $(RES_BG_DIR)battle_bg01.NCGR
	battbgcnv $(RES_BG_DIR)battle_bg02.NCGR
	battbgcnv $(RES_BG_DIR)battle_bg03.NCGR
	battbgcnv $(RES_BG_DIR)battle_bg04.NCGR
	battbgcnv $(RES_BG_DIR)battle_bg05.NCGR
	battbgcnv $(RES_BG_DIR)battle_bg06.NCGR
	battbgcnv $(RES_BG_DIR)battle_bg07.NCGR
	battbgcnv $(RES_BG_DIR)battle_bg08.NCGR
	battbgcnv $(RES_BG_DIR)battle_bg09.NCGR
	battbgcnv $(RES_BG_DIR)battle_bg10.NCGR
	battbgcnv $(RES_BG_DIR)battle_bg11.NCGR
	battbgcnv $(RES_BG_DIR)battle_bg12.NCGR
	battbgcnv $(RES_BG_DIR)battle_bg13.NCGR
	battbgcnv $(RES_BG_DIR)battle_bg14.NCGR
	battbgcnv $(RES_BG_DIR)battle_bg15.NCGR
	battbgcnv $(RES_BG_DIR)battle_bg16.NCGR
	battbgcnv $(RES_BG_DIR)battle_bg17.NCGR
	battbgcnv $(RES_BG_DIR)battle_bg18.NCGR
	battbgcnv $(RES_BG_DIR)battle_bg19.NCGR
	battbgcnv $(RES_BG_DIR)battle_bg20.NCGR
	battbgcnv $(RES_BG_DIR)battle_bg21.NCGR
	battbgcnv $(RES_BG_DIR)battle_bg22.NCGR
	$(NITRO_COMP) $(NITROSDK_ROOT) batt_bg.lst pl_batt_bg.narc pl_batt_bg.naix

pl_batt_obj.narc: $(RES_OBJ_DIR:\\=/)*.N*R batt_obj.lst
#	nnsarc -c -l -n -i pl_batt_obj.narc -S batt_obj.lst
	$(NITRO_COMP) $(NITROSDK_ROOT) batt_obj.lst pl_batt_obj.narc pl_batt_obj.naix

