#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - libraries
# File:     Makefile
#
# Copyright 2003-2005,2007 Nintendo.  All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law.  They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Log: Makefile,v $
# Revision 1.44  2007/04/16 00:13:54  yosizaki
# add wfs
#
# Revision 1.43  2005/09/05 09:15:09  yada
# add env
#
# Revision 1.42  2005/08/22 00:14:53  yada
# add std library
#
# Revision 1.41  2005/06/24 02:19:39  yasu
# utest 追加
#
# Revision 1.40  2005/04/12 06:16:59  ida
# sndの追加
#
# Revision 1.39  2005/01/13 10:08:12  yada
# fix copyright header
#
# Revision 1.38  2005/01/13 01:45:57  terui
# SUBDIRS に wvr を改めて追加。
#
# Revision 1.37  2005/01/13 00:25:20  terui
# SUBDIRS から wvr を一旦削除
#
# Revision 1.36  2004/12/14 10:50:06  seiki_masashi
# MATH ジャンルを追加
#
# Revision 1.35  2004/12/06 10:18:02  terui
# Add wvr library
#
# Revision 1.34  2004/09/13 10:47:39  yada
# add ctrdg directory
#
# Revision 1.33  2004/09/13 05:37:28  yasu
# merged with NitroSDK_2_00pr6_branch
#
# Revision 1.32.4.1  2004/09/13 04:25:26  yasu
# Add wbt in build chain
#
# Revision 1.32  2004/08/29 23:56:48  yasu
# bring wbt into private package
#
# Revision 1.31  2004/08/23 09:44:27  yosizaki
# add WBT.
#
# Revision 1.30  2004/08/07 07:13:39  yosizaki
# add libmb
#
# Revision 1.29  2004/06/28 01:54:22  yosizaki
# add libcard.
#
# Revision 1.28  2004/06/23 11:59:49  seiki_masashi
# add 'PRC*' to SDK
#
# Revision 1.27  2004/06/14 04:23:20  terui
# Add wm in SUBDIRS
#
# Revision 1.26  2004/05/31 03:24:12  yasu
# merge e3s gpio package
#
# Revision 1.25  2004/05/12 05:31:30  terui
# Add spi , rtc in SUBDIRS.
#
# Revision 1.24  2004/05/06 10:18:18  yasu
# fix typo
#
# Revision 1.23  2004/05/06 02:54:17  takano_makoto
# Add cp.
#
# Revision 1.22  2004/04/22 06:56:33  yasu
# workaround for CW blxeq/blxne problem
#
# Revision 1.21  2004/04/14 12:37:45  yasu
# add spi
#
# Revision 1.20  2004/04/01 11:03:16  yosizaki
# add libraries -libfs*
#
# Revision 1.19  2004/03/30 07:09:56  yasu
# refering to NITROSYSTEM_BUILDLIBS
#
# Revision 1.18  2004/03/30 07:04:15  yasu
# add NITROSYSTEM_SOUNDBUILD
#
# Revision 1.17  2004/03/26 04:49:41  yasu
# stop building sound library
#
# Revision 1.16  2004/02/28 09:48:45  yasu
# add EXI functions for ARM7
#
# Revision 1.15  2004/02/23 06:41:51  ida
# hide snd library
#
# Revision 1.14  2004/02/18 04:16:30  yasu
# append pad
#
# Revision 1.13  2004/02/13 02:24:16  yasu
# add new pxi
#
# Revision 1.12  2004/02/12 12:33:19  yasu
# add snd
#
# Revision 1.11  2004/02/05 07:09:01  yasu
# change SDK prefix iris -> nitro
#
# Revision 1.10  2004/02/04 12:38:17  kitani_toshikazu
# Added ext lib
#
# Revision 1.9  2003/12/24 07:53:15  yada
# miディレクトリ追加
#
# Revision 1.8  2003/12/10 12:19:03  yasu
# ISD ライブラリの無い環境のために空の関数をダミーで用意し、stubs 下に置いた．
#
# Revision 1.7  2003/11/17 00:33:15  nishida_kenji
# fx追加
#
# Revision 1.6  2003/11/12 04:18:35  yasu
# ディレクトリを変更 base -> init
#
# Revision 1.5  2003/11/06 13:43:34  yasu
# os の追加
#
# Revision 1.4  2003/11/06 10:36:48  yasu
# base の追加
#
# Revision 1.3  2003/11/05 09:09:58  yasu
# gx の追加
#
# Revision 1.2  2003/10/30 05:51:52  yasu
# build: ターゲットの削除
#
# Revision 1.1  2003/10/29 13:27:27  yasu
# make の伝播のフレームワーク完成
# all/install/clean/clobber が使える
#
# $NoKeywords: $
#----------------------------------------------------------------------------

include	$(NITROSDK_ROOT)/build/buildtools/commondefs


#----------------------------------------------------------------------------

SUBDIRS		= fx		\
		  gx		\
		  os		\
		  mi		\
		  pxi		\
		  init		\
		  stubs		\
		  ext		\
		  pad		\
		  exi		\
		  fs		\
		  cp		\
		  spi		\
		  rtc		\
		  wm		\
		  wbt		\
		  wfs		\
		  prc		\
		  card		\
		  mb		\
		  ctrdg		\
		  math		\
		  wvr		\
		  snd		\
		  utest		\
		  std		\
		  env

ifdef   NITROSYSTEM_BUILDLIBS
SUBDIRS		+= $(NITROSYSTEM_BUILDLIBS)
endif

ifdef	NITRO_PRIVATE
SUBDIRS		+=
endif

ifdef   NITRO_WITH_ARM7
SUBDIRS		+= e3s
endif

#----------------------------------------------------------------------------
export	NITRO_BLXCHECKED = yes

#----------------------------------------------------------------------------

include	$(NITROSDK_ROOT)/build/buildtools/modulerules


#===== End of Makefile =====
