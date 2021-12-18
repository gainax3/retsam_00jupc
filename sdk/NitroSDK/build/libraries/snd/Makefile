#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - SND - libraries
# File:     Makefile
#
# Copyright 2004,2005 Nintendo.  All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law.  They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Log: Makefile,v $
# Revision 1.5  2005/04/13 00:13:15  ida
# 著作権年次表記を更新
#
# Revision 1.4  2005/04/12 06:16:12  ida
# SNDをNITRO-SDKへ移設
#
# Revision 1.4  2004/07/16 06:42:49  ida
# fix Project name
#
# Revision 1.3  2004/06/24 01:23:30  nishida_kenji
# Fix header comments.
#
# Revision 1.2  2004/05/06 23:55:49  ida
# fix typo
#
# Revision 1.1  2004/03/26 06:25:52  ida
# create snd_drv
#
# Revision 1.2  2004/02/20 03:56:59  ida
# add ARM9 snd library
#
# Revision 1.1  2004/02/12 12:32:40  yasu
# makefile for NitroSDK/build/libraries/snd
#
# $NoKeywords: $
#----------------------------------------------------------------------------

include	$(NITROSDK_ROOT)/build/buildtools/commondefs


#----------------------------------------------------------------------------

SUBDIRS =	ARM9

ifdef	NITRO_WITH_ARM7
SUBDIRS +=	ARM7
endif

#----------------------------------------------------------------------------

include	$(NITROSDK_ROOT)/build/buildtools/modulerules


#===== End of Makefile =====
