#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - libraries - mi
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
# Revision 1.4  2004/05/06 10:18:18  yasu
# fix typo
#
# Revision 1.3  2004/02/13 02:22:44  yasu
# bug fix
#
# Revision 1.2  2004/02/12 12:33:38  yasu
# add NITRO_WITH_ARM7 switch
#
# Revision 1.1  2004/02/12 11:32:17  yasu
# makefile for NitroSDK/build/libraries/mi
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
