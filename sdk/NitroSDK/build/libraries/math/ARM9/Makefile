#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - libraries - math
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
# Revision 1.7  2005/05/13 09:23:09  seiki_masashi
# FFT 関数の追加
#
# Revision 1.6  2005/04/12 11:12:27  seiki_masashi
# checksum.h の追加
#
# Revision 1.5  2005/04/12 10:15:36  takano_makoto
# Add qsort.c.
#
# Revision 1.4  2005/04/11 10:35:56  seiki_masashi
# dgt.c の追加
#
# Revision 1.3  2005/04/11 01:48:53  seiki_masashi
# crc.c の追加
#
# Revision 1.2  2005/01/11 07:39:59  takano_makoto
# fix copyright header.
#
# Revision 1.1  2005/01/06 06:25:50  seiki_masashi
# ARM7用ライブラリも生成するように変更
#
# $NoKeywords: $
#----------------------------------------------------------------------------

SUBDIRS		=


#----------------------------------------------------------------------------

# build ARM & THUMB libraries
NITRO_CODEGEN_ALL 	?= True

SRCDIR			= ../common/src src

SRCS			= math.c dgt.c crc.c checksum.c qsort.c fft.c

TARGET_LIB		= libmath$(NITRO_LIBSUFFIX).a


include	$(NITROSDK_ROOT)/build/buildtools/commondefs

INSTALL_TARGETS		= $(TARGETS)
INSTALL_DIR		= $(NITRO_INSTALL_LIBDIR)


#----------------------------------------------------------------------------

do-build:		$(TARGETS)


include	$(NITROSDK_ROOT)/build/buildtools/modulerules


#===== End of Makefile =====
