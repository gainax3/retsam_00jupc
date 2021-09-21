#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - WFS - libraries
# File:     Makefile
#
# Copyright 2007 Nintendo.  All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law.  They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Log: Makefile,v $
# Revision 1.4  2007/06/14 13:14:35  yosizaki
# add wfs_thread.c
#
# Revision 1.3  2007/06/11 09:04:11  yosizaki
# add wfs_archive.c
#
# Revision 1.2  2007/04/16 08:39:01  seiki_masashi
# fix INSTALL_DIR
#
# Revision 1.1  2007/04/13 04:12:37  yosizaki
# initial upload.
#
# $NoKeywords: $
#----------------------------------------------------------------------------

SUBDIRS			=

#----------------------------------------------------------------------------
# build ARM & THUMB libraries
NITRO_CODEGEN_ALL 	?= True

# Codegen for sub processer
NITRO_PROC		= ARM9

SRCS			=  wfs_format.c wfs_server.c wfs_client.c
SRCS			+= wfs_archive.c wfs_thread.c

TARGET_LIB		= libwfs$(NITRO_LIBSUFFIX).a

include	$(NITROSDK_ROOT)/build/buildtools/commondefs

INSTALL_TARGETS		= $(TARGETS)
INSTALL_DIR		= $(NITRO_INSTALL_LIBDIR)

#----------------------------------------------------------------------------

do-build:		$(TARGETS)


include	$(NITROSDK_ROOT)/build/buildtools/modulerules

#===== End of Makefile =====
