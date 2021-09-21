#! make -f
#---------------------------------------------------------------------------
#  Project:  NitroSDK - gx - DEMOlib - tools
#  File:     Makefile
#
#  Copyright 2003,2004 Nintendo.  All rights reserved.
#
#  These coded instructions, statements, and computer programs contain
#  proprietary information of Nintendo of America Inc. and/or Nintendo
#  Company Ltd., and are protected by Federal copyright law.  They may
#  not be disclosed to third parties or copied or duplicated in any form,
#  in whole or in part, without the prior written consent of Nintendo.
#
#  $Log: Makefile,v $
#  Revision 1.4  2004/08/09 09:06:47  yasu
#  Add ppmconvbg
#
#  Revision 1.3  2004/08/03 10:56:21  yasu
#  Fix implicit rules
#
#  Revision 1.2  2004/07/28 08:35:00  yasu
#  Add ppmconv8
#
#  Revision 1.1  2004/07/28 07:00:21  yasu
#  'ppmconv' as new tool.
#
#  $NoKeywords: $
#---------------------------------------------------------------------------
include	$(NITROSDK_ROOT)/build/buildtools/commondefs

TARGETS			= ppmconv.exe ppmconv8.exe ppmconvbg.exe
SOURCES			= $(TARGETS:.exe=.c)
OBJECTS			= $(SOURCES:.c=.o)
HEADERS			=

INSTALL_DIR     	= $(NITRO_INSTALL_TOOLSDIR)/bin
INSTALL_TARGETS		= $(TARGETS)

LDIRT_CLEAN		= $(OBJECTS) $(TARGETS)

include	$(NITROSDK_ROOT)/build/buildtools/modulerules.x86

#----------------------------------------------------------------------------
#  build
#----------------------------------------------------------------------------
do-build:		$(TARGETS)

%.exe:			%.o
			$(CC_X86) $+ -o $@

#--- ppmconv
ppmconv.o:		ppmconv.c
ppmconv8.o:		ppmconv8.c
ppmconvbg.o:		ppmconvbg.c

#
