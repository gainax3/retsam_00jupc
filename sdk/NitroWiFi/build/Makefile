#! make -f
#----------------------------------------------------------------------------
# Project:  NitroWiFi - build
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
# Revision 1.9  2006/03/10 09:21:49  kitase_hirotake
# tests 以下の make を削除
#
# Revision 1.8  2006/01/29 23:54:49  okubata_ryoma
# Copyrightの修正
#
# Revision 1.7  2006/01/27 08:13:31  seiki_masashi
# tools を build パスに追加
#
# Revision 1.6  2005/08/19 12:28:20  yasu
# add-ins のサポート
#
# Revision 1.5  2005/06/28 04:51:18  yasu
# buildsetup/versions のビルド条件の修正
#
# Revision 1.4  2005/06/28 04:46:24  yasu
# buildsetup/versions の追加
#
# Revision 1.3  2005/06/28 04:06:30  yasu
# NITROWIFI_NOSUPPORT フラグの追加
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
# STAGE: SETUP
#
ifdef	NITROWIFI_NOSUPPORT
SUBMAKES.SETUP		+= buildsetup/versions/Makefile
endif
SUBMAKES.SETUP		+= $(NWIFI_ADDINSDIR)/Makefile.setup

#----------------------------------------------------------------------------
# STAGE: TOOLS
#
SUBMAKES.TOOLS		+= tools/Makefile

#----------------------------------------------------------------------------
# STAGE: LIBS
#
SUBMAKES.LIBS		+= libraries/Makefile			\
			   $(NWIFI_ADDINSDIR)/Makefile.libs

#----------------------------------------------------------------------------
# STAGE: CLEANUP
#
SUBMAKES.CLEANUP	+= $(NWIFI_ADDINSDIR)/Makefile.cleanup

#----------------------------------------------------------------------------

SUBMAKES		= $(SUBMAKES.SETUP)			\
			  $(SUBMAKES.TOOLS)			\
			  $(SUBMAKES.LIBS)			\
			  $(SUBMAKES.CLEANUP)

#----------------------------------------------------------------------------

include	$(NITROWIFI_ROOT)/build/buildtools/modulerules


#===== End of Makefile =====
