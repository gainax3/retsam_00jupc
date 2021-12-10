#! make -f
#---------------------------------------------------------------------------
#  Project:  NitroSDK - tools
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
#  Revision 1.1  2004/09/27 09:39:59  yasu
#  Add new tool
#
#  $NoKeywords: $
#---------------------------------------------------------------------------
include $(NITROSDK_ROOT)/build/buildtools/commondefs

TARGETS			= ntrcompbw.exe
SOURCES			= main.c 		\
			  compress.c		\
			  file.c 

OBJECTS			= $(SOURCES:.c=.o)
HEADERS			= $(filter-out version.h,$(wildcard *.h))	\
			  compress.h					\
			  file.h					\
			  common.h

vpath	%.c		. ../compstatic
vpath	%.h		. ../compstatic

MACROS			+= -I../compstatic -DDETAIL

INSTALL_DIR		= $(NITRO_INSTALL_TOOLSDIR)/bin
INSTALL_TARGETS		= $(TARGETS)

LDIRT_CLEAN		= $(OBJECTS) $(TARGETS)

include $(NITROSDK_ROOT)/build/buildtools/modulerules.x86

#----------------------------------------------------------------------------
#  build
#----------------------------------------------------------------------------
do-build:		$(TARGETS)

$(TARGETS):		$(OBJECTS)
			$(CC_X86) $+ -o $@

main.o:			main.c version.h
compress.o:		compress.c compress.h file.h
file.o:			file.c file.h

version.h:		$(SOURCES) $(HEADERS) $(MAKEFILE)
			@for i in $^ ;			\
			do				\
				date -r $$i +'#define SDK_DATE_OF_LATEST_FILE %Y%m%dUL'; \
			done | sort | tail -1 > $@
#
