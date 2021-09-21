#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - libraries - init
# File:     Makefile
#
# Copyright 2003-2005 Nintendo.  All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law.  They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Log: Makefile,v $
# Revision 1.15  2005/03/30 00:02:30  yosizaki
# fix copyright header.
#
# Revision 1.14  2005/03/24 00:55:21  yasu
# SDK_NOCOMPRESS サポートの中止
#
# Revision 1.13  2004/09/14 01:58:14  yasu
# Fix backslashs at end of line.
#
# Revision 1.12  2004/09/09 11:45:41  yasu
# Support compstatic in default
#
# Revision 1.11  2004/09/01 07:29:09  yasu
# Support NITRO_COMPRESS
#
# Revision 1.10  2004/08/27 09:22:05  yasu
# Fix around NITRO_CODEGEN_ALL
#
# Revision 1.9  2004/07/14 14:12:07  yasu
# Change NITRO_CODEGEN=ALL -> NITRO_CODEGEN_ALL=True
#
# Revision 1.8  2004/07/14 11:52:57  yasu
# Build ARM & THUMB code always
#
# Revision 1.7  2004/05/06 10:18:18  yasu
# fix typo
#
# Revision 1.6  2004/02/05 07:09:02  yasu
# change SDK prefix iris -> nitro
#
# Revision 1.5  2003/12/11 03:52:51  yasu
# 空行調整
#
# Revision 1.4  2003/12/09 08:28:55  yasu
# crt0.s -> crt0.c へ変更
#
# Revision 1.3  2003/12/08 12:12:45  yada
# _prototype をインクルードしないようにした
#
# Revision 1.2  2003/11/30 09:35:19  yasu
# コンパイル用 ROOT とインストール用 ROOT を独立させた
#
# Revision 1.1  2003/11/12 04:18:04  yasu
# base.a を非ライブラリ化し crt0.o の形で直接リンクするようにした．
# そのためのディレクトリを移動 base -> init
#
# Revision 1.1  2003/11/06 10:37:48  yasu
# crt0.s をライブラリ化
# このディレクトリは古い SDK でも build 可能．
#
# $NoKeywords: $
#----------------------------------------------------------------------------

SUBDIRS			=


#----------------------------------------------------------------------------

# build ARM & THUMB libraries
NITRO_CODEGEN_ALL 	?= True

SRCS			= crt0.c
TARGET_OBJ		= crt0.o

include	$(NITROSDK_ROOT)/build/buildtools/commondefs

INSTALL_TARGETS		= $(TARGETS)
INSTALL_DIR		= $(NITRO_INSTALL_LIBDIR)


#----------------------------------------------------------------------------

do-build:		$(TARGETS)


include	$(NITROSDK_ROOT)/build/buildtools/modulerules


#===== End of Makefile =====
