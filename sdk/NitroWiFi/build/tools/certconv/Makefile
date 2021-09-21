#! make -f
#---------------------------------------------------------------------------
#  Project:  NitroWiFi - tools - certconv
#  File:     Makefile
#
#  Copyright 2006 Nintendo.  All rights reserved.
#
#  These coded instructions, statements, and computer programs contain
#  proprietary information of Nintendo of America Inc. and/or Nintendo
#  Company Ltd., and are protected by Federal copyright law.  They may
#  not be disclosed to third parties or copied or duplicated in any form,
#  in whole or in part, without the prior written consent of Nintendo.
#
#  $Log: Makefile,v $
#  Revision 1.1  2006/01/30 02:37:12  seiki_masashi
#  makecert を certconv に名前変更
#
#  Revision 1.1  2006/01/23 07:59:29  seiki_masashi
#  makecert ツールを追加
#
#  $NoKeywords: $
#---------------------------------------------------------------------------

include	$(NITROWIFI_ROOT)/build/buildtools/commondefs

TARGETS		= certconv.exe

OBJECTS		= make_cacert.o

INSTALL_DIR     = $(NWIFI_INSTALL_TOOLSDIR)/bin
INSTALL_TARGETS	= $(TARGETS)

LDIRT_CLEAN	= $(OBJECTS) $(TARGETS)

include	$(NITROWIFI_ROOT)/build/buildtools/modulerules.x86
WARNING =

#----------------------------------------------------------------------------
#  build
#----------------------------------------------------------------------------
do-build:		$(TARGETS)

$(TARGETS):		$(OBJECTS)
			$(CC_X86) $+ -o $@

make_cacert.o:		make_cacert.c
