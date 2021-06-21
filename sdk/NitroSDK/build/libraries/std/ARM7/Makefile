#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - libraries - std/ARM7
# File:     Makefile
#
# Copyright 2005-2007 Nintendo.  All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law.  They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Log: Makefile,v $
# Revision 1.4  2007/04/24 12:02:59  yosizaki
# update copyright.
#
# Revision 1.3  2007/04/20 09:47:12  yosizaki
# add std_sprintf.c
#
# Revision 1.2  2006/07/05 07:44:00  kitase_hirotake
# std ‚É sjis <-> unicode •ÏŠ·ƒ‰ƒCƒuƒ‰ƒŠ‚Ì’Ç‰Á
#
# Revision 1.1  2005/08/19 11:04:28  yada
# initial release
#
# $NoKeywords: $
#----------------------------------------------------------------------------

SUBDIRS			=

#----------------------------------------------------------------------------

# build ARM & THUMB libraries
NITRO_CODEGEN_ALL 	?= True

# Codegen for sub processer
NITRO_PROC		= ARM7

SRCDIR			= ../common/src
INCDIR			= ../common/include

SRCS			= std_string.c std_sprintf.c std_unicode.c

TARGET_LIB		= libstd_sp$(NITRO_LIBSUFFIX).a


include	$(NITROSDK_ROOT)/build/buildtools/commondefs

INSTALL_TARGETS		= $(TARGETS)
INSTALL_DIR		= $(NITRO_INSTALL_LIBDIR)


#----------------------------------------------------------------------------

do-build:		$(TARGETS)


include	$(NITROSDK_ROOT)/build/buildtools/modulerules


#===== End of Makefile =====
