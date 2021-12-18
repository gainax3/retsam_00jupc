#! make -f
#---------------------------------------------------------------------------
#  Project:  NitroSDK - tools - envconv
#  File:     Makefile
#
#  Copyright 2005 Nintendo.  All rights reserved.
#
#  These coded instructions, statements, and computer programs contain
#  proprietary information of Nintendo of America Inc. and/or Nintendo
#  Company Ltd., and are protected by Federal copyright law.  They may
#  not be disclosed to third parties or copied or duplicated in any form,
#  in whole or in part, without the prior written consent of Nintendo.
#
#  $Log: Makefile,v $
#  Revision 1.1  2005/10/20 02:14:22  kitase_hirotake
#  Initial Upload
#
#  $NoKeywords: $
#---------------------------------------------------------------------------
include	$(NITROSDK_ROOT)/build/buildtools/commondefs

TARGETS		= init2env.exe

SOURCES		= init.l init.y

OBJECTS		= init.tab.o init.yy.o

INSTALL_DIR     = $(NITRO_INSTALL_TOOLSDIR)/bin
INSTALL_TARGETS	= $(TARGETS)

LDIRT_CLEAN	= $(OBJECTS) $(TARGETS) init.yy.c init.tab.c init.tab.h

include	$(NITROSDK_ROOT)/build/buildtools/modulerules.x86

#----------------------------------------------------------------------------
#  build
#----------------------------------------------------------------------------
do-build:		$(TARGETS)

$(TARGETS):		$(OBJECTS)
			$(CC_X86) $+ -o $@

lex.yy.o:		init.yy.c init.tab.c
lex.yy.c:		init.l
y.tab.c:		init.y
