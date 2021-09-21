#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - MATH - libraries
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
# Revision 1.3  2005/01/11 07:39:35  takano_makoto
# fix copyright header.
#
# Revision 1.2  2005/01/06 06:24:32  seiki_masashi
# ARM7用ライブラリも生成するように変更
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
