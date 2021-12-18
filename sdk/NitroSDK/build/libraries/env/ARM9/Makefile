#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - libraries - env - ARM9
# File:     Makefile
#
# Copyright 2005  All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law.  They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Log: Makefile,v $
# Revision 1.1  2005/09/05 01:16:04  yada
# initial release
#
# $NoKeywords: $
#----------------------------------------------------------------------------

SUBDIRS		=

#----------------------------------------------------------------------------
TARGET_LIB		 = libenv$(NITRO_LIBSUFFIX).a
SRCS			 = env_system.c

include	$(NITROSDK_ROOT)/build/buildtools/commondefs

#----------------------------------------------------------------------------
INSTALL_TARGETS		 = $(TARGETS)
INSTALL_DIR		 = $(NITRO_INSTALL_LIBDIR)

#----------------------------------------------------------------------------
do-build:		$(TARGETS)

include	$(NITROSDK_ROOT)/build/buildtools/modulerules


#===== End of Makefile =====
