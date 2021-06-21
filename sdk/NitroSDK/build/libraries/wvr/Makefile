#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - WVR - libraries
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
# Revision 1.6  2005/02/16 10:35:24  terui
# SUBDIRSにARM9を追加。
#
# Revision 1.5  2005/01/14 06:18:44  terui
# ARM9をSUBDIRSに含める条件を追加。
#
# Revision 1.4  2005/01/13 01:47:01  terui
# SUBDIRS から ARM9 を一旦削除。
#
# Revision 1.3  2005/01/11 07:46:29  takano_makoto
# fix copyright header.
#
# Revision 1.2  2005/01/05 07:28:36  terui
# ARM9をSUB_DIRに追加。
#
# Revision 1.1  2004/12/06 10:18:24  terui
# Initial upload.
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
