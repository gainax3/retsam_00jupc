#! make -f
#----------------------------------------------------------------------------
# Project:  NitroWiFi - add-ins
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
# Revision 1.3  2005/09/17 12:46:12  terui
# NitroDWCのmakeを行わないように変更。
#
# Revision 1.2  2005/09/06 00:19:42  yasu
# env の NitroSDK への移動に伴いコンパイルを停止
#
# Revision 1.1  2005/08/19 12:28:58  yasu
# add-ins のサポート
#
# $NoKeywords: $
#----------------------------------------------------------------------------
include	$(NITROSDK_ROOT)/build/buildtools/commondefs

# <注意!>
#
# このMakefileは
# NITRO_ONLY_SETUP
# NITRO_ONLY_LIBS
# NITRO_ONLY_DEMOS
# NITRO_ONLY_CLEANUP
# の順にシンボルが export されて 4回 make されます。(Nightly Build時のみ)
# SUBDIRS以下の各Makefileはそれを考慮してifdefによる場合分けを行う必要があります。
#

SUBDIRS 	+= 

#ifdef	NITRO_NOSUPPORT
#SUBDIRS 	+= NitroDWC
#endif

include	$(NITROSDK_ROOT)/build/buildtools/modulerules

#===== End of Makefile =====
