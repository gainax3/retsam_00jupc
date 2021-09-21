#! make -f
#----------------------------------------------------------------------------
# Project:  NitroWiFi - tools
# File:     Makefile
#
# Copyright 2003-2006 Nintendo.  All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law.  They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Log: Makefile,v $
# Revision 1.5  2006/03/16 00:33:59  kitase_hirotake
# wifiindent は PRIVATE のみに修正
#
# Revision 1.4  2006/03/15 09:52:49  kitase_hirotake
# add wifiindent
#
# Revision 1.3  2006/02/28 08:49:40  okubata_ryoma
# NitroCrypto切り出しによる変更
#
# Revision 1.2  2006/01/30 02:36:53  seiki_masashi
# certconv ツールを追加
#
# Revision 1.1  2006/01/23 06:44:35  seiki_masashi
# rc4conv ツールの追加
#
# $NoKeywords: $
#----------------------------------------------------------------------------

include	$(NITROWIFI_ROOT)/build/buildtools/commondefs


#----------------------------------------------------------------------------

SUBDIRS 	=  certconv

ifdef	NITROWIFI_NOSUPPORT
SUBDIRS 	+= wifiindent
endif


#----------------------------------------------------------------------------

include	$(NITROWIFI_ROOT)/build/buildtools/modulerules


#===== End of Makefile =====
