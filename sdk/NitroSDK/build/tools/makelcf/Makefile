#! make -f
#---------------------------------------------------------------------------
#  Project:  NitroSDK - tools - makelcf
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
#  Revision 1.8  2004/10/03 09:41:52  yasu
#  Fix for make clobber
#
#  Revision 1.7  2004/09/02 12:46:11  yasu
#  Add dependancies
#
#  Revision 1.6  2004/08/05 02:20:22  yasu
#  Expand scanner buffer size
#
#  Revision 1.5  2004/07/15 05:20:53  yasu
#  Support showing version number at help screen
#
#  Revision 1.4  2004/03/26 05:07:11  yasu
#  support variables like as -DNAME=VALUE
#
#  Revision 1.3  2004/02/05 07:09:03  yasu
#  change SDK prefix iris -> nitro
#
#  Revision 1.2  2004/01/18 06:32:26  nishida_kenji
#  small fix in dependency
#
#  Revision 1.1  2004/01/05 02:32:59  yasu
#  Initial version
#
#  $NoKeywords: $
#---------------------------------------------------------------------------
include	$(NITROSDK_ROOT)/build/buildtools/commondefs

TARGETS		= makelcf.exe

SOURCES_C	= makelcf.c defval.c misc.c container.c createlcf.c

SOURCES		= $(SOURCES_C) spec.l spec.y tlcf.l tlcf.y

OBJECTS		= $(SOURCES_C:.c=.o) \
		  spec.tab.o spec.yy.patched.o \
		  tlcf.tab.o tlcf.yy.o 

HEADERS		= defval.h makelcf.h misc.h

INSTALL_DIR     = $(NITRO_INSTALL_TOOLSDIR)/bin
INSTALL_TARGETS	= $(TARGETS)

LDIRT_CLEAN	= $(OBJECTS) $(TARGETS) version.h \
		  spec.tab.c spec.tab.h spec.yy.c spec.yy.patched.c \
		  tlcf.tab.c tlcf.tab.h tlcf.yy.c

include	$(NITROSDK_ROOT)/build/buildtools/modulerules.x86

#----------------------------------------------------------------------------
#  build
#----------------------------------------------------------------------------
do-build:		$(TARGETS)

$(TARGETS):		$(OBJECTS)
			$(CC_X86) $+ -o $@

makelcf.o:		makelcf.c makelcf.h version.h
container.o:		container.c makelcf.h
createlcf.o:		createlcf.c makelcf.h tlcf.tab.c
defval.o:		defval.c misc.h defval.h makelcf.h
misc.o:			misc.c misc.h defval.h makelcf.h

spec.tab.o:		spec.tab.c makelcf.h
spec.yy.patched.o:	spec.yy.patched.c spec.tab.c makelcf.h
spec.yy.c:		spec.l spec.tab.c
spec.tab.c:             spec.y
tlcf.yy.o:		tlcf.yy.c tlcf.tab.c makelcf.h
tlcf.yy.c:		tlcf.l
tlcf.tab.c:       	tlcf.y

# Platform support note: Require GNU coreutils on MacOS
SED := sed
ifneq ($(OS),Windows_NT)
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
SED := gsed
endif
endif

spec.yy.patched.o:	WARNING	+= -Wno-unused-label -Wno-unused-function

spec.yy.patched.c:	spec.yy.c
			$(SED) 's/^#define YY_BUF_SIZE 16384/#define YY_BUF_SIZE (1024*1024)/' $< > $@

version.h:		$(SOURCES) $(HEADERS) $(MAKEFILE)
			@for i in $^ ;				\
			do					\
				date -r $$i +'#define SDK_DATE_OF_LATEST_FILE %Y%m%dUL';	\
			done | sort | tail -1 > $@
