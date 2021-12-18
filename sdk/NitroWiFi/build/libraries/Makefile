#! make -f
#----------------------------------------------------------------------------
# Project:  NitroWiFi - libraries
# File:     Makefile
#
# Copyright 2005,2006 Nintendo.  All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law.  They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Log: Makefile,v $
# Revision 1.15  2006/03/12 23:55:09  okubata_ryoma
# Copyright fix
#
# Revision 1.14  2006/02/28 08:49:05  okubata_ryoma
# NitroCrypto切り出しによる変更
#
# Revision 1.13  2005/12/21 00:22:27  seiki_masashi
# crypt を NITROWIFI_HIDDEN で囲うように修正
#
# Revision 1.12  2005/12/13 11:54:39  seiki_masashi
# private な crypt ディレクトリを追加
#
# Revision 1.11  2005/08/19 12:28:20  yasu
# add-ins のサポート
#
# Revision 1.10  2005/08/17 10:49:55  yasu
# add-ins/env の追加
#
# Revision 1.9  2005/07/29 05:08:16  terui
# SUBDIRS に stubs を追加
#
# Revision 1.8  2005/07/22 14:04:13  yasu
# 一時的なコメントアウト
#
# Revision 1.7  2005/07/22 13:57:49  yasu
# インデント修正
#
# Revision 1.6  2005/07/21 11:20:14  seiki_masashi
# bsafe をビルド対象に追加
#
# Revision 1.5  2005/07/15 13:33:25  yasu
# soc ライブラリの追加
#
# Revision 1.4  2005/07/15 09:27:32  yasu
# cps, ssl ライブラリ受け入れにともなうツリーの調整
#
# Revision 1.3  2005/07/07 10:46:40  terui
# SUBDIRS に wcm を追加
#
# Revision 1.2  2005/06/23 11:13:17  yasu
# NitroWiFi の commondefs, modulerules を読み込む
#
# Revision 1.1  2005/06/22 11:07:57  yasu
# ソースツリーテンプレート作成
#
# $NoKeywords: $
#----------------------------------------------------------------------------

include	$(NITROWIFI_ROOT)/build/buildtools/commondefs


#----------------------------------------------------------------------------

SUBDIRS			 = wcm soc stubs

#----------------------------------------------------------------------------

include	$(NITROWIFI_ROOT)/build/buildtools/modulerules


#===== End of Makefile =====
