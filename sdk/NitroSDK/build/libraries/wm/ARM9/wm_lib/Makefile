#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - WM_LIB - libraries
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
# Revision 1.2  2004/08/27 09:22:05  yasu
# Fix around NITRO_CODEGEN_ALL
#
# Revision 1.1  2004/07/20 11:25:40  terui
# wm_libêÿÇËó£ÇµÇ…î∫Ç§â¸ë¢
#
# $NoKeywords: $
#----------------------------------------------------------------------------

SUBDIRS			=

# Codegen for sub processer
NITRO_PROC		=  ARM9
#NITRO_CODEGEN_ALL	?= True

SRCDIR			= ./src
INCDIR			= ../../inc

SRCS			= wm_lib.c wm_tool.c ni.c

TARGET_LIB		= libwm_lib$(NITRO_LIBSUFFIX).a

include	$(NITROSDK_ROOT)/build/buildtools/commondefs

INSTALL_TARGETS		= $(TARGETS)

INSTALL_DIR		= $(NITRO_INSTALL_LIBDIR)

GDIRT_INSTALLED		= $(addprefix $(INSTALL_DIR)/,$(notdir $(INSTALL_TARGETS)))

#----------------------------------------------------------------------------

do-build:		$(TARGETS)

include	$(NITROSDK_ROOT)/build/buildtools/modulerules


#===== End of Makefile =====
