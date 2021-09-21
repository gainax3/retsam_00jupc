#! make -f
#---------------------------------------------------------------------------
#  Project:  NitroSDK - tools
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
#  Revision 1.3  2005/06/13 02:56:34  yasu
#  オプションの修正、テスト作成による動作の確認
#
#  Revision 1.2  2005/06/10 12:55:43  yasu
#  1st リファクタリング
#
#  Revision 1.1  2005/06/10 00:43:01  yasu
#  初期版
#
#  $NoKeywords: $
#---------------------------------------------------------------------------
include $(NITROSDK_ROOT)/build/buildtools/commondefs

TARGETS			= bin2obj.exe
SOURCES			= bin2obj.c		\
			  cookargs.c		\
			  section.c		\
			  object.c		\
			  output.c

OBJECTS			= $(SOURCES:.c=.o)
HEADERS			= $(filter-out version.h,$(wildcard *.h))
MACROS			+= -I$(NITRO_INCDIR)

INSTALL_DIR		= $(NITRO_INSTALL_TOOLSDIR)/bin
INSTALL_TARGETS		= $(TARGETS)

LDIRT_CLEAN		= $(OBJECTS) $(TARGETS) version.h

include $(NITROSDK_ROOT)/build/buildtools/modulerules.x86

#----------------------------------------------------------------------------
#  build
#----------------------------------------------------------------------------
do-build:		$(TARGETS)

$(TARGETS):		$(OBJECTS)
			$(CC_X86) $+ -o $@

bin2obj.o:		bin2obj.c bin2obj.h
cookargs.o:		cookargs.c bin2obj.h version.h
section.o:		section.c bin2obj.h
object.o:		object.c bin2obj.h
output.o:		output.c bin2obj.h
bin2obj.h:		elf.h object.h types.h

version.h:		$(SOURCES) $(HEADERS) $(MAKEFILE)
			@for i in $^ ;			\
			do				\
				date -r $$i +'#define SDK_DATE_OF_LATEST_FILE %Y%m%dUL'; \
			done | sort | tail -1 > $@
#
