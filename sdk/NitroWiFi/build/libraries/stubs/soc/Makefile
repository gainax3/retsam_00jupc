#! make -f
#----------------------------------------------------------------------------
# Project:  NitroWiFi - libraries - stubs - soc
# File:     Makefile
#
# Copyright 2005 Nintendo.  All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law.  They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Log: Makefile,v $
# Revision 1.2  2005/08/23 04:42:25  yasu
# NITROWIFI_INSTALL_ROOT ?֘A?̏C??
#
# Revision 1.1  2005/08/04 08:17:23  yada
# stub added
#
# $NoKeywords: $
#----------------------------------------------------------------------------

SUBDIRS			 =

#----------------------------------------------------------------------------
# build ARM & THUMB libraries
NITRO_CODEGEN_ALL 	?= True

SRCDIR			= src
SRCS			= soc_stub.c

TARGET_LIB		= libstubsoc$(NITRO_LIBSUFFIX).a

include	$(NITROWIFI_ROOT)/build/buildtools/commondefs

INSTALL_TARGETS		= $(TARGETS)
INSTALL_DIR		= $(NWIFI_INSTALL_LIBDIR)

#----------------------------------------------------------------------------

do-build:		$(TARGETS)

include	$(NITROWIFI_ROOT)/build/buildtools/modulerules


#===== End of Makefile =====
