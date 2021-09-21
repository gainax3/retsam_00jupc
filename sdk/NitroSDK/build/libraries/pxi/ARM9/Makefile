#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - libraries - pxi
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
# Revision 1.7  2004/08/27 09:22:05  yasu
# Fix around NITRO_CODEGEN_ALL
#
# Revision 1.6  2004/07/14 14:12:07  yasu
# Change NITRO_CODEGEN=ALL -> NITRO_CODEGEN_ALL=True
#
# Revision 1.5  2004/07/14 11:52:57  yasu
# Build ARM & THUMB code always
#
# Revision 1.4  2004/05/06 10:18:18  yasu
# fix typo
#
# Revision 1.3  2004/03/08 01:55:18  yasu
# add pxi_init.c
#
# Revision 1.2  2004/03/04 09:07:11  yasu
# add pxi_fifo.c
#
# Revision 1.1  2004/02/13 02:24:16  yasu
# add new pxi
#
# $NoKeywords: $
#----------------------------------------------------------------------------

SUBDIRS			=

#----------------------------------------------------------------------------

# build ARM & THUMB libraries
NITRO_CODEGEN_ALL 	?= True

NITRO_PROC		= ARM9

SRCDIR			= ../common/src src

SRCS			= pxi_init.c pxi_fifo.c

TARGET_LIB		= libpxi$(NITRO_LIBSUFFIX).a


include	$(NITROSDK_ROOT)/build/buildtools/commondefs

INSTALL_TARGETS		= $(TARGETS)
INSTALL_DIR		= $(NITRO_INSTALL_LIBDIR)


#----------------------------------------------------------------------------

do-build:		$(TARGETS)


include	$(NITROSDK_ROOT)/build/buildtools/modulerules


#===== End of Makefile =====
