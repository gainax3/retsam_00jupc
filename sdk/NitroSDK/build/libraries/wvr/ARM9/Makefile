#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - WVR - libraries
# File:     Makefile
#
# Copyright 2003,2004 Nintendo.  All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law.  They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Log: Makefile,v $
# Revision 1.1  2004/12/20 00:50:54  terui
# Initial upload.
#
# $NoKeywords: $
#----------------------------------------------------------------------------

SUBDIRS			=

#----------------------------------------------------------------------------

INCDIR			=	./include

SRCS			=	wvr.c

TARGET_LIB		=	libwvr$(NITRO_LIBSUFFIX).a


#----------------------------------------------------------------------------
include	$(NITROSDK_ROOT)/build/buildtools/commondefs

INSTALL_TARGETS		= $(TARGETS)
INSTALL_DIR     	= $(NITRO_INSTALL_LIBDIR)
GDIRT_INSTALLED		= $(addprefix $(INSTALL_DIR)/,$(notdir $(INSTALL_TARGETS)))

#----------------------------------------------------------------------------

do-build:		$(TARGETS)

include	$(NITROSDK_ROOT)/build/buildtools/modulerules


#===== End of Makefile =====

