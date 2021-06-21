#! make -f
#----------------------------------------------------------------------------
# Project:  NitroWiFi - add-ins - libraries - env - ARM9
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
# Revision 1.3  2005/08/23 04:41:16  yasu
# NITROWIFI_INSTALL_ROOT 関連の修正
#
# Revision 1.2  2005/08/19 08:16:59  yasu
# commondefs.add-ins.env により WiFi のメインツリーへインストールするように修正
#
# Revision 1.1  2005/08/17 06:30:53  yada
# initial release
#
# $NoKeywords: $
#----------------------------------------------------------------------------

SUBDIRS		=

#----------------------------------------------------------------------------
TARGET_LIB		 = libenv$(NITRO_LIBSUFFIX).a
SRCS			 = env_system.c

include	$(NITROWIFI_ROOT)/build/buildtools/commondefs

#----------------------------------------------------------------------------
INSTALL_TARGETS		 = $(TARGETS)
INSTALL_DIR		 = $(NWIFI_ADDINS_ENV_INSTALL_LIBDIR)

#----------------------------------------------------------------------------
do-build:		$(TARGETS)

include	$(NITROWIFI_ROOT)/build/buildtools/modulerules


#===== End of Makefile =====
