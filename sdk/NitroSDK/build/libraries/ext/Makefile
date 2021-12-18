#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - libraries - EXT
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
# Revision 1.8  2004/08/27 09:22:05  yasu
# Fix around NITRO_CODEGEN_ALL
#
# Revision 1.7  2004/07/14 14:12:07  yasu
# Change NITRO_CODEGEN=ALL -> NITRO_CODEGEN_ALL=True
#
# Revision 1.6  2004/07/14 11:52:57  yasu
# Build ARM & THUMB code always
#
# Revision 1.5  2004/05/27 09:15:41  takano_makoto
# Add ext_keycontrol.c
#
# Revision 1.4  2004/05/06 10:18:18  yasu
# fix typo
#
# Revision 1.3  2004/02/09 11:17:49  yasu
# support thumb version library
#
# Revision 1.2  2004/02/05 07:09:01  yasu
# change SDK prefix iris -> nitro
#
# Revision 1.1  2004/02/04 12:37:55  kitani_toshikazu
# Initial check in.
#
# $NoKeywords: $
#----------------------------------------------------------------------------

SUBDIRS			=


#----------------------------------------------------------------------------

# build ARM & THUMB libraries
NITRO_CODEGEN_ALL 	?= True

SRCS			= ext_scrntest.c ext_keycontrol.c

TARGET_LIB		= libext$(NITRO_LIBSUFFIX).a


include	$(NITROSDK_ROOT)/build/buildtools/commondefs

INSTALL_TARGETS		= $(TARGETS)
INSTALL_DIR		= $(NITRO_INSTALL_LIBDIR)


#----------------------------------------------------------------------------

do-build:		$(TARGETS)


include	$(NITROSDK_ROOT)/build/buildtools/modulerules


#===== End of Makefile =====
