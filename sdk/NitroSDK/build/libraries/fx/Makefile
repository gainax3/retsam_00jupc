#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - libraries - gx
# File:     Makefile
#
# Copyright 2003,2004,2007 Nintendo.  All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law.  They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Log: Makefile,v $
# Revision 1.18  2007/01/18 04:07:26  okubata_ryoma
# FX_AsinIdx, FX_AcosIdx ŠÖ”‚Ì’Ç‰Á
#
# Revision 1.17  2004/08/27 09:22:05  yasu
# Fix around NITRO_CODEGEN_ALL
#
# Revision 1.16  2004/07/14 14:12:07  yasu
# Change NITRO_CODEGEN=ALL -> NITRO_CODEGEN_ALL=True
#
# Revision 1.15  2004/07/14 11:52:57  yasu
# Build ARM & THUMB code always
#
# Revision 1.14  2004/06/16 11:24:21  seiki_masashi
# FX_AtanIdx, FX_Atan2Idx
#
# Revision 1.13  2004/05/06 10:18:18  yasu
# fix typo
#
# Revision 1.12  2004/04/15 05:59:23  takano_makoto
# Add fx_mtx22.c
#
# Revision 1.11  2004/02/09 11:17:49  yasu
# support thumb version library
#
# Revision 1.10  2004/02/05 07:09:02  yasu
# change SDK prefix iris -> nitro
#
# Revision 1.9  2004/01/16 10:40:09  nishida_kenji
# add fx.c
#
# $NoKeywords: $
#----------------------------------------------------------------------------

SUBDIRS		=


#----------------------------------------------------------------------------

# build ARM & THUMB libraries
NITRO_CODEGEN_ALL 	?= True

SRCS			= fx_sincos.c 		\
			  fx_mtx22.c		\
			  fx_mtx33.c		\
			  fx_mtx43.c		\
			  fx_mtx44.c		\
			  fx_cp.c		\
			  fx_vec.c		\
                          fx_trig.c		\
                          fx_atan.c		\
                          fx_atanidx.c		\
                          fx.c			\
                          fx_asinacosidx.c

TARGET_LIB		= libfx$(NITRO_LIBSUFFIX).a


include	$(NITROSDK_ROOT)/build/buildtools/commondefs

INSTALL_TARGETS		= $(TARGETS)
INSTALL_DIR		= $(NITRO_INSTALL_LIBDIR)


#----------------------------------------------------------------------------

do-build:		$(TARGETS)


include	$(NITROSDK_ROOT)/build/buildtools/modulerules


#===== End of Makefile =====
