#! make -f
#----------------------------------------------------------------------------
# Project:  NitroWiFi - SOC - libraries
# File:     Makefile
#
# Copyright 2005 Nintendo.  All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law.  They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Log: Makefile,v $
# Revision 1.17  2005/08/23 04:42:25  yasu
# NITROWIFI_INSTALL_ROOT 関連の修正
#
# Revision 1.16  2005/08/18 13:19:23  yasu
# cleanup と list の追加
#
# Revision 1.15  2005/08/08 06:16:43  seiki_masashi
# socl_ssl.c を復帰
# sed に渡すパスが cygwin 形式になるよう修正
#
# Revision 1.14  2005/08/07 13:03:20  yasu
# socl_misc.c 追加/socl_ssl.c を一時コメントアウト
#
# Revision 1.13  2005/08/06 11:55:54  seiki_masashi
# SOCL_EnableSsl を追加
#
# Revision 1.12  2005/07/30 15:31:14  yasu
# コマンドパイプ分離にともなう修正
#
# Revision 1.11  2005/07/28 11:16:25  yasu
# SOC_Poll の実装
#
# Revision 1.10  2005/07/25 10:15:01  yasu
# SOC ライブラリの追加
#
# Revision 1.9  2005/07/22 13:55:53  yasu
# listen accept の追加
#
# Revision 1.8  2005/07/22 12:45:05  yasu
# 非同期処理仮実装
#
# Revision 1.7  2005/07/19 14:06:39  yasu
# SOCL_Read/Write 追加
#
# Revision 1.6  2005/07/18 13:16:36  yasu
# 送信受信スレッドを作成する
#
# Revision 1.5  2005/07/15 13:33:11  yasu
# soc ライブラリのソース調整
#
# Revision 1.4  2005/06/28 02:35:14  yasu
# ソケットスレッド作成関数を別に
#
# Revision 1.3  2005/06/23 11:13:17  yasu
# NitroWiFi の commondefs, modulerules を読み込む
#
# Revision 1.2  2005/06/22 10:52:13  yasu
# アルファ版作成
#
# Revision 1.1  2005/06/20 08:42:29  terui
# ツリー構造を新規作成
#
# $NoKeywords: $
#----------------------------------------------------------------------------

SUBDIRS			 =

#----------------------------------------------------------------------------
# build ARM & THUMB libraries
NITRO_CODEGEN_ALL 	?= True

#SRCDIR			= src
#INCDIR			=

SRCS			= socl_startup.c	\
			  socl_command.c	\
			  socl_create.c		\
			  socl_listen_accept.c	\
			  socl_bind.c		\
			  socl_read.c		\
			  socl_write.c		\
			  socl_shutdown.c	\
			  socl_close.c		\
			  socl_cleanup.c	\
			  socl_resolve.c	\
			  socl_poll.c		\
			  socl_const.c		\
			  socl_misc.c		\
			  socl_list.c		\
			  soc.c

SRCS			+= socl_ssl.c

TARGET_LIB		= libsoc$(NITRO_LIBSUFFIX).a

include	$(NITROWIFI_ROOT)/build/buildtools/commondefs

#----------------------------------------------------------------------------

INSTALL_TARGETS		= $(TARGETS)
INSTALL_DIR		= $(NWIFI_INSTALL_LIBDIR)

#----------------------------------------------------------------------------

do-build:		$(TARGETS)

include	$(NITROWIFI_ROOT)/build/buildtools/modulerules

ERRTABLE_H	= include/errtable.h

$(OBJDIR)/socl_misc.o:	$(ERRTABLE_H)

$(ERRTABLE_H): $(NWIFI_ROOT)/include/nitroWiFi/socl.h
	mkdir -p include
	sed -e '/^\/\/ ERROR-CODE BEGINS/,/^\/\/ ERROR-CODE ENDS/!d;\
		/SOC_/!d;\
		s/^.*SOC_\([A-Z]*\).*$$/{ SOC_\1, "\1" },/g' $< > $@

LDIRT_CLEAN	= $(ERRTABLE_H)

#===== End of Makefile =====
