#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - libraries - PRC
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
# Revision 1.9  2005/01/11 07:43:05  takano_makoto
# fix copyright header.
#
# Revision 1.8  2005/01/06 07:35:19  seiki_masashi
# コメントの修正
#
# Revision 1.7  2004/08/27 09:22:05  yasu
# Fix around NITRO_CODEGEN_ALL
#
# Revision 1.6  2004/07/14 14:12:07  yasu
# Change NITRO_CODEGEN=ALL -> NITRO_CODEGEN_ALL=True
#
# Revision 1.5  2004/07/14 11:52:57  yasu
# Build ARM & THUMB code always
#
# Revision 1.4  2004/06/30 08:49:24  seiki_masashi
# algo_simple -> algo_light
# algo_standard -> algo_fine
# algo_accurate -> algo_superfine
# 新しい algo_standard の追加
#
# Revision 1.3  2004/06/25 13:32:11  seiki_masashi
# 認識アルゴリズム 'Standard' の追加
#
# Revision 1.2  2004/06/25 03:02:55  seiki_masashi
# 各関数から workAreaSize の指定を削除
# GetRecognitionWorkAreaSize* の追加
#
# Revision 1.1  2004/06/23 11:59:30  seiki_masashi
# add 'PRC*' to SDK
#
#
# $NoKeywords: $
#----------------------------------------------------------------------------

SUBDIRS			=


#----------------------------------------------------------------------------

# build ARM & THUMB libraries
NITRO_CODEGEN_ALL 	?= True

SRCS			= prc.c 		\
			  prc_algo_common.c	\
			  prc_algo_light.c	\
			  prc_algo_standard.c	\
			  prc_algo_fine.c	\
			  prc_algo_superfine.c	\
			  prc_resample.c

TARGET_LIB		= libprc$(NITRO_LIBSUFFIX).a


include	$(NITROSDK_ROOT)/build/buildtools/commondefs

INSTALL_TARGETS		= $(TARGETS)
INSTALL_DIR		= $(NITRO_INSTALL_LIBDIR)


#----------------------------------------------------------------------------

do-build:		$(TARGETS)


include	$(NITROSDK_ROOT)/build/buildtools/modulerules


#===== End of Makefile =====
