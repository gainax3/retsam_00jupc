#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - libraries - gx
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
# Revision 1.24  2004/08/27 09:22:05  yasu
# Fix around NITRO_CODEGEN_ALL
#
# Revision 1.23  2004/07/14 14:12:07  yasu
# Change NITRO_CODEGEN=ALL -> NITRO_CODEGEN_ALL=True
#
# Revision 1.22  2004/07/14 11:52:57  yasu
# Build ARM & THUMB code always
#
# Revision 1.21  2004/05/06 10:18:18  yasu
# fix typo
#
# Revision 1.20  2004/02/09 11:17:49  yasu
# support thumb version library
#
# Revision 1.19  2004/02/09 11:08:00  nishida_kenji
# Add gxasm.c
#
# Revision 1.18  2004/02/05 07:09:02  yasu
# change SDK prefix iris -> nitro
#
# Revision 1.17  2004/01/13 07:38:20  nishida_kenji
# g3.c
#
# Revision 1.16  2003/12/25 12:01:47  nishida_kenji
# g3util.c -> g3_util.c
#
# Revision 1.15  2003/12/25 11:54:19  nishida_kenji
# gx_load2d.c gx_load3d.c
# $NoKeywords: $
#----------------------------------------------------------------------------

SUBDIRS		=


#----------------------------------------------------------------------------

# build ARM & THUMB libraries
NITRO_CODEGEN_ALL 	?= True

SRCS			= gx.c			\
			  gxstate.c		\
			  gx_vramcnt.c		\
			  gx_bgcnt.c		\
			  g2.c			\
			  g3b.c			\
			  g3c.c			\
			  g3imm.c		\
			  g3x.c			\
			  g3_util.c		\
                          gx_load2d.c		\
                          gx_load3d.c		\
                          g3.c			\
                          gxasm.c

TARGET_LIB		= libgx$(NITRO_LIBSUFFIX).a


include	$(NITROSDK_ROOT)/build/buildtools/commondefs

INSTALL_TARGETS		= $(TARGETS)
INSTALL_DIR		= $(NITRO_INSTALL_LIBDIR)


#----------------------------------------------------------------------------

do-build:		$(TARGETS)


include	$(NITROSDK_ROOT)/build/buildtools/modulerules


#===== End of Makefile =====
