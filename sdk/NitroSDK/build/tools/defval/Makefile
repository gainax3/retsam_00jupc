#! make -f
#---------------------------------------------------------------------------
#  Project:  NitroSDK - tools - defval
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
#  Revision 1.1  2005/06/20 07:07:15  yasu
#  add defval
#
#  $NoKeywords: $
#---------------------------------------------------------------------------
include	$(NITROSDK_ROOT)/build/buildtools/commondefs

TARGETS		= defval.exe
SOURCES		= defval.c set_defval.c get_defval.c misc.c
OBJECTS		= $(SOURCES:.c=.o)
HEADERS		= defval.h misc.h

INSTALL_DIR     = $(NITRO_INSTALL_TOOLSDIR)/bin
INSTALL_TARGETS	= $(TARGETS)

LDIRT_CLEAN	= $(OBJECTS) $(TARGETS) version.h

include	$(NITROSDK_ROOT)/build/buildtools/modulerules.x86

#----------------------------------------------------------------------------
#  build
#----------------------------------------------------------------------------
do-build:		$(TARGETS)

$(TARGETS):		$(OBJECTS)
			$(CC_X86) $+ -o $@

defval.o:		defval.c     defval.h version.h
set_defval.o:		set_defval.c defval.h
get_defval.o:		get_defval.c defval.h
misc.o:			misc.c misc.h

defval.h:		misc.h

version.h:		$(SOURCES) $(HEADERS) $(MAKEFILE)
			@for i in $^ ;				\
			do					\
				date -r $$i +'#define SDK_DATE_OF_LATEST_FILE %Y%m%dUL';	\
			done | sort | tail -1 > $@
