#! make -f
#----------------------------------------------------------------------------
# Project:  フロンティア用パーティクル
# File:     Makefile
#
# Copyright 2007 GameFreak.inc  All rights reserved.
#
#----------------------------------------------------------------------------

SUBDIRS		=

ETD	= ./../../../convert/nitro_comp/etd.exe

SPAFILES:=$(wildcard *.spa)

include	$(NITROSDK_ROOT)/build/buildtools/commondefs.cctype.CW
include	$(NITROSYSTEM_ROOT)/build/buildtools/commondefs


#Makeで生成される*.binはmake cleanの削除対象にする
LDIRT_CLEAN	= pokelist_particle.narc pokelist_particle.naix

#----------------------------------------------------------------------------

include	$(NITROSYSTEM_ROOT)/build/buildtools/modulerules

.SUFFIXES:.spa.narc

#-------------------------------
#	実行順
#		リソースコピー
#		リソースアーカイブ
#		技エフェクトアーカイブ
#-------------------------------
do-build: particledata.narc
#---------- 技エフェクト用パーティクルをアーカイブする ----------
particledata.narc: $(SPAFILES)
	nnsarc -c -l -n -i pokelist_particle.narc -S pokelist_particle.lst
	$(ETD) pokelist_particle.naix
