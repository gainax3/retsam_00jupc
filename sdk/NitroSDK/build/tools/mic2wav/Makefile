#! make -f
#---------------------------------------------------------------------------
#  Project:  NitroSDK - tools - mic2wav
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
#  Revision 1.3  2004/06/18 13:14:55  yasu
#  LD_X86 -> CXX_X86
#
#  Revision 1.2  2004/06/18 10:22:58  yasu
#  LD_X86 ’è‹`‚ð—˜—p‚·‚é
#
#  Revision 1.1  2004/06/17 04:41:49  yosizaki
#  (none)
#
#  $NoKeywords: $
#---------------------------------------------------------------------------
include	$(NITROSDK_ROOT)/build/buildtools/commondefs

TARGETS		= mic2wav.exe

OBJECTS		= main.o

INSTALL_DIR	= $(NITRO_INSTALL_TOOLSDIR)/bin
INSTALL_TARGETS	= $(TARGETS)

LDIRT_CLEAN	= $(OBJECTS) $(TARGETS)

include	$(NITROSDK_ROOT)/build/buildtools/modulerules.x86


#----------------------------------------------------------------------------
#  build
#----------------------------------------------------------------------------
do-build:		$(TARGETS)

$(TARGETS):		$(OBJECTS)
			$(CXX_X86) $+ -o $@

main.o:		main.cpp
