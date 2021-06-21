#! make -f
#----------------------------------------------------------------------------
# Project:  NitroDWC - base - libraries
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
# Revision 1.15  2006/06/29 09:52:55  takayama
# dwc_common.cをNITRODWC_NOGSオプション時から外した。
#
# Revision 1.14  2006/04/27 11:17:33  takayama
# NITRODWC_NOGSオプションをブラウザ向けから通常のものに修正。
#
# Revision 1.13  2006/04/25 12:03:51  nakata
# DWC_Ndラッパを追加
#
# Revision 1.12  2006/04/21 01:33:43  takayama
# NITORDWC_NOGSでのビルド環境の調整。
#
# Revision 1.11  2006/04/20 13:21:11  takayama
# NITRODWC_NOGSオプションに対応。
#
# Revision 1.10  2006/04/10 10:16:57  hayashi
# added the ranking module
#
# Revision 1.9  2006/03/07 01:18:16  nakata
# NASに関連する機能追加をdwc_nasfunc.c/dwc_nasfunc.hに集約する際に必要な変更を行った。
#
# Revision 1.8  2005/12/26 04:23:17  nakata
# Added NAS return code 40 related functions to Auth.
# Added DWC_GetIngamesnCheckResult() to dwc_common.c/dwc_common.h.
# Fixed the bug that incorrect ingamesn is sent when auth is called in netcheck.
#
# Revision 1.7  2005/09/05 05:16:25  onozawa
# dwc_ghttp.cの追加
#
# Revision 1.6  2005/08/31 11:25:35  sasakit
# dwc_reportをaccountモジュールに移動
#
# Revision 1.5  2005/08/31 02:29:44  sasakit
# 初期化用のコードがoverlayで分けられるように、モジュールを分けた。
#
# Revision 1.4  2005/08/19 12:12:10  sasakit
# リリースに向けて修正
#
# Revision 1.3  2005/08/19 06:10:15  sasakit
# ファイルの追加
#
# Revision 1.2  2005/08/17 13:15:01  arakit
# ・他のマッチング形式と同じく、友達指定マッチングもアルゴリズムを改良した。
# ・ゲーム定義のQR2キーを設定できるようにした。
# ・SBサーバアップデートのフィルタをゲームから指定できるようにした。
# ・SBサーバアップデートで見つけたサーバを評価し、マッチングの優先順位をゲーム側で
# 決めるシステムを作成した。
# ・マッチングのバグを修正した。
# ・いつでもマッチングをキャンセルできるようにした（サーバクライアント型マッチングは未完成）。
#
# Revision 1.1.1.1  2005/08/02 08:02:37  sasakit
# First import of NitroDWC source tree.
#
# $NoKeywords: $
#----------------------------------------------------------------------------

SUBDIRS		=

#----------------------------------------------------------------------------

SRCS  = \
	dwc_error.c \
	dwc_memfunc.c \
	dwc_connectinet.c \
	dwc_nasfunc.c \
	dwc_nd.c

ifndef NITRODWC_NOGS
SRCS += \
	dwc_common.c \
	dwc_main.c \
	dwc_login.c \
	dwc_friend.c \
	dwc_match.c \
	dwc_transport.c \
	dwc_ghttp.c \
	dwc_ranking.c
endif

NITRODWC_MODULE_NAME :=   base
TARGET_LIB	=	lib$(NITRODWC_LIB_PREFIX)$(NITRODWC_MODULE_NAME)$(NITRO_LIBSUFFIX).a

CCFLAGS_MSGSTYLE = -msgstyle gcc

#----------------------------------------------------------------------------

include	$(NITRODWC_ROOT)/build/buildtools/commondefs

INSTALL_TARGETS	=	$(TARGETS)
INSTALL_DIR		=	$(NITRODWC_INSTALL_LIBDIR)

#----------------------------------------------------------------------------

do-build:		$(TARGETS)

include	$(NITRODWC_ROOT)/build/buildtools/modulerules

#----------------------------------------------------------------------------

#===== End of Makefile =====
