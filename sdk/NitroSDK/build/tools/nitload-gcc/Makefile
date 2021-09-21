#! make -f
#---------------------------------------------------------------------------
#  Project:  NitroSDK - tools - nitload-gcc
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
#  Revision 1.1  2005/06/21 06:22:33  yasu
#  èâî≈çÏê¨
#
#  $NoKeywords: $
#---------------------------------------------------------------------------
include	$(NITROSDK_ROOT)/build/buildtools/commondefs
include	$(NITROSDK_ROOT)/build/buildtools/commondefs.emtype.ISD

SRCPATH		= $(ISD_ROOT)/Samples/download/X86

LINCLUDES	+= $(ISD_ROOT)/X86/include

TARGETS		= _nitload.exe

SOURCES_CC	= download.cpp

OBJECTS		= $(SOURCES_CC:.cpp=.o)

HEADERS		= nitrodll.h stdafx_orig.h stdafx.h

INSTALL_DIR     = $(NITRO_INSTALL_TOOLSDIR)/bin
INSTALL_TARGETS = $(TARGETS)

LDIRT_CLEAN	= $(OBJECTS) $(TARGETS) nitrodll.h stdafx_orig.h download.cpp

include	$(NITROSDK_ROOT)/build/buildtools/modulerules.x86

LINCS		= $(subst \ -I,\ ,$(addprefix -I,$(LINCLUDES)))
WARNING		= -Wall -Wno-unknown-pragmas -Wno-unused-variable

%.o:		%.cpp
		$(CXX_X86) $(MACROS) -DSDK_WIN32 $(WARNING) -c -I. $(LINCS) $< -o $@

download.cpp:	$(SRCPATH)/download.cpp $(HEADERS)
		@$(SED)  's/dll.GetDebugPrint(/dll_GetDebugPrint(\&dll,/g;s/ ======/=/' "$<" > $@

nitrodll.h:	$(SRCPATH)/nitrodll.h
		@cp "$<" $@

stdafx_orig.h:	$(SRCPATH)/stdafx.h
		@cp "$<" $@

#----------------------------------------------------------------------------
#  build
#----------------------------------------------------------------------------
do-build:		$(TARGETS)

$(TARGETS):		$(OBJECTS)
			$(CC_X86) $+ -o $@ -lstdc++
