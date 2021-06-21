#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - CARD - libraries
# File:     Makefile
#
# Copyright 2003-2005 Nintendo.  All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law.  They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Log: Makefile,v $
# Revision 1.10  2005/10/24 10:49:02  yosizaki
# add include path.
#
# Revision 1.9  2005/05/18 10:00:53  yosizaki
# fix copyright header.
#
# Revision 1.8  2005/05/12 11:20:20  yosizaki
# delete card_device.c
#
# Revision 1.7  2004/09/15 05:48:20  yada
# add card_pullOut.c
#
# Revision 1.6  2004/08/27 09:22:05  yasu
# Fix around NITRO_CODEGEN_ALL
#
# Revision 1.5  2004/07/23 08:27:31  yosizaki
# add card_device, card_request.
#
# Revision 1.4  2004/07/14 14:12:07  yasu
# Change NITRO_CODEGEN=ALL -> NITRO_CODEGEN_ALL=True
#
# Revision 1.3  2004/07/14 11:52:57  yasu
# Build ARM & THUMB code always
#
# Revision 1.2  2004/07/10 10:19:37  yosizaki
# add card_common.c
#
# Revision 1.1  2004/06/28 01:53:17  yosizaki
# (none)
#
# $NoKeywords: $
#----------------------------------------------------------------------------

SUBDIRS			=

#----------------------------------------------------------------------------

# build ARM & THUMB libraries
NITRO_CODEGEN_ALL 	?= True

NITRO_PROC		= ARM9

SRCDIR			= ../common/src src
INCDIR			= ../common/include include

SRCS			= card_common.c card_spi.c	\
			  card_backup.c card_rom.c card_request.c	\
			  card_pullOut.c

TARGET_LIB		= libcard$(NITRO_LIBSUFFIX).a


include	$(NITROSDK_ROOT)/build/buildtools/commondefs

INSTALL_TARGETS		= $(TARGETS)
INSTALL_DIR		= $(NITRO_INSTALL_LIBDIR)


#----------------------------------------------------------------------------

do-build:		$(TARGETS)


include	$(NITROSDK_ROOT)/build/buildtools/modulerules


#===== End of Makefile =====
