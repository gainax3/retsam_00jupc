#! make -f
#---------------------------------------------------------------------------
#  Project:  NitroSDK - tools - loadrun
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
#  Revision 1.7  2005/09/05 01:37:43  yasu
#  TAB の空白化
#
#  Revision 1.6  2005/09/05 01:34:33  yasu
#  version.h を参照しソースファイルの最近のタイムスタンプを表示するようにする
#
#  Revision 1.5  2005/08/31 12:14:27  yada
#  applied for OS_Exit()
#
#  Revision 1.4  2005/08/30 07:43:04  yada
#  add nitrorun
#
#  Revision 1.3  2005/08/24 09:53:35  yada
#  applied for new ISD library
#
#  Revision 1.2  2005/08/24 08:02:26  yada
#  rm some include
#
#  Revision 1.1  2005/08/24 07:44:46  yada
#  (none)
#
#  $NoKeywords: $
#---------------------------------------------------------------------------
include	$(NITROSDK_ROOT)/build/buildtools/commondefs
include	$(NITROSDK_ROOT)/build/buildtools/commondefs.emtype.ISD

SRCPATH		= $(ISD_ROOT)/Samples/download/X86

LINCLUDES	+= $(ISD_ROOT)/X86/include ./ $(ROOT)/include

TARGETS		= loadrun.exe

SOURCES_CC	= loadrun.c isd_api.c
HEADERS_CC	= isd_api.h

OBJECTS		= loadrun.o isd_api.o

NITRORUN	= nitrorun



INSTALL_DIR     = $(NITRO_INSTALL_TOOLSDIR)/bin
INSTALL_TARGETS	= $(TARGETS) $(NITRORUN)

LDIRT_CLEAN	= $(OBJECTS) $(TARGETS)

include	$(NITROSDK_ROOT)/build/buildtools/modulerules.x86

LINCS		= $(subst \ -I,\ ,$(addprefix -I,$(LINCLUDES)))
WARNING		= -Wall -Wno-unknown-pragmas -Wno-unused-variable

%.o:		%.c
		$(CC_X86) $(MACROS) -DSDK_WIN32 $(WARNING) -c -I. $(LINCS) $< -o $@

#----------------------------------------------------------------------------
#  build
#----------------------------------------------------------------------------
do-build:	$(TARGETS)

$(TARGETS):	$(OBJECTS)
		$(CC_X86) $+ -o $@

loadrun.o:	isd_api.h loadrun.c version.h
isd_api.o:	isd_api.h isd_api.c

version.h:	$(SOURCES_CC) $(HEADERS_CC) $(MAKEFILE)
		@for i in $^ ; do \
			date -r $$i +'#define SDK_DATE_OF_LATEST_FILE %Y%m%dUL'; \
		done | sort | tail -1 > $@
