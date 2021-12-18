#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - libraries - ctrdg
# File:     Makefile
#
# Copyright 2003,2004,2006 Nintendo.  All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law.  They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Log: Makefile,v $
# Revision 1.4  2006/04/05 10:47:59  okubata_ryoma
# AGBバックアップライブラリのSDK収録のための変更
#
# Revision 1.3  2006/03/02 10:12:00  okubata_ryoma
# ctrdg_vib.cの追加
#
# Revision 1.2  2004/09/14 09:58:34  yada
# add ctrdg_proc.c
#
# Revision 1.1  2004/09/13 10:45:25  yada
# initial release.
#
# $NoKeywords: $
#----------------------------------------------------------------------------


SUBDIRS		=

#----------------------------------------------------------------------------

# build ARM & THUMB libraries
NITRO_CODEGEN_ALL =	True

NITRO_PROC	=	ARM9

SRCDIR		=	../common/src src

SRCS		=	ctrdg.c				\
			ctrdg_proc.c			\
			ctrdg_vib.c			\
			ctrdg_backup.c			\
			ctrdg_flash_common.c		\
			ctrdg_flash_AT29LV512.c		\
			ctrdg_flash_LE26FV10N1TS-10.c	\
			ctrdg_flash_LE39FW512.c		\
			ctrdg_flash_MX29L010.c		\
			ctrdg_flash_MX29L512.c		\
			ctrdg_sram.c		\
			ctrdg_task.c


TARGET_LIB	=	libctrdg$(NITRO_LIBSUFFIX).a

include	$(NITROSDK_ROOT)/build/buildtools/commondefs

INSTALL_TARGETS	=	$(TARGETS)
INSTALL_DIR	=	$(NITRO_INSTALL_LIBDIR)

#----------------------------------------------------------------------------

do-build:		$(TARGETS)


include	$(NITROSDK_ROOT)/build/buildtools/modulerules

#===== End of Makefile =====
