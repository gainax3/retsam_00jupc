#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - build
# File:     Makefile
#
# Copyright 2003,2004 Nintendo.  All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law.  They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Log: Makefile,v $
# Revision 1.34  2004/10/30 04:39:42  yasu
# stop build WMTest
#
# Revision 1.33  2004/10/29 03:12:56  yasu
# fix tests/wm/WMTest -> tests/wm/WMTest/Makefile
#
# Revision 1.32  2004/10/28 23:57:54  yasu
# fix test/wm/WMtest -> tests/wm/WMtest
#
# Revision 1.31  2004/10/28 12:59:33  yasu
# Build WMTest if private build
#
# Revision 1.30  2004/09/06 06:04:14  yasu
# Supoort ADDINS.TOOLS
#
# Revision 1.29  2004/09/04 00:46:17  yasu
# fix dirname
#
# Revision 1.28  2004/09/04 00:44:29  yasu
# fix dirname
#
# Revision 1.27  2004/09/04 00:35:59  yasu
# build DEMOLib whenever NITRO_WITHOUT_DEMOS defined
#
# Revision 1.26  2004/09/03 01:31:15  yasu
# Split 'build add-ins' to libs and demos
#
# Revision 1.25  2004/07/14 11:54:19  yasu
# Hide add-ins directory
#
# Revision 1.24  2004/07/13 12:36:12  yasu
# Build add-in
#
# Revision 1.23  2004/05/06 10:18:18  yasu
# fix typo
#
# Revision 1.22  2004/04/21 03:05:28  yasu
# delete buildtools/verinfo.* at 'make clobber'
#
# Revision 1.21  2004/03/18 09:36:33  yasu
# don't build tests if NITRO_WITHOUT_DEMOS defined
#
# Revision 1.20  2004/03/01 07:28:21  yasu
# Add NITRO_WITHOUT_DEMOS
#
# Revision 1.19  2004/02/16 03:42:58  yasu
# switch building components by NITRO_WITH_ARM7
#
# Revision 1.18  2004/02/14 06:35:31  yasu
# add "components"
#
# Revision 1.17  2004/02/12 11:44:38  yada
# ARM9/ARM7 振り分け作業
#
# Revision 1.16  2004/02/10 03:32:44  yasu
# bug fix
#
# Revision 1.15  2004/02/09 05:51:47  yasu
# bugfix
#
# Revision 1.14  2004/02/09 03:31:03  yasu
# make "tests" private
#
# Revision 1.13  2004/02/06 11:21:51  kitani_toshikazu
# Swap demos and tests build order. because tests can use DEMOLib.
#
# Revision 1.12  2004/02/05 07:09:01  yasu
# change SDK prefix iris -> nitro
#
# Revision 1.11  2004/02/03 12:35:07  yasu
# add libraries_common
#
# Revision 1.10  2003/12/22 14:11:01  yasu
# libraries_sp の追加
#
# Revision 1.9  2003/12/16 08:05:08  yasu
# 一時的に libraries_sp のリンクを切る
#
# Revision 1.8  2003/12/16 08:01:16  yasu
# librariesSP -> libraries_sp へ変更
#
# Revision 1.7  2003/12/16 07:41:43  yasu
# spcode -> libraries_sp の変更
#
# Revision 1.6  2003/12/11 00:09:17  yasu
# _prototype のビルドの停止
#
# Revision 1.5  2003/11/12 12:30:10  yasu
# sp -> spcode へ再変更
#
# Revision 1.4  2003/11/12 04:20:00  yasu
# ディレクトリ名変更 build/spcode -> build/sp
#
# Revision 1.3  2003/11/05 07:08:09  yasu
# _prototype の追加
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

ifdef	NITRO_WITH_ARM7
SUBMAKES.COMPONENTES	= components/Makefile
SUBMAKES.ADDINS.TOOLS	= ../add-ins/Makefile.tools
SUBMAKES.ADDINS.LIBS	= ../add-ins/Makefile.libs
SUBMAKES.ADDINS.DEMOS_	= ../add-ins/Makefile.demos
endif


SUBMAKES 		= buildsetup/Makefile		\
			  $(SUBMAKES.ADDINS.TOOLS)	\
			  tools/Makefile		\
			  $(SUBMAKES.ADDINS.LIBS)	\
			  libraries/Makefile		\
			  $(SUBMAKES.COMPONENTES)	\
			  $(SUBMAKES.ADDINS.DEMOS)	\
			  $(SUBMAKES.TESTS)

LDIRT_CLOBBER		= buildtools/verinfo.cw.cc \
			  buildtools/verinfo.cw.ld

#----------------------------------------------------------------------------

include	$(NITROSDK_ROOT)/build/buildtools/modulerules


#===== End of Makefile =====
