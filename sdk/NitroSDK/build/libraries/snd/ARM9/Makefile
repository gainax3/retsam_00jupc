#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - SND - libraries
# File:     Makefile
#
# Copyright 2004,2005 Nintendo.  All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law.  They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Log: Makefile,v $
# Revision 1.9  2005/04/13 00:13:11  ida
# 著作権年次表記を更新
#
# Revision 1.8  2005/04/12 06:16:12  ida
# SNDをNITRO-SDKへ移設
#
# Revision 1.9  2004/11/22 04:28:45  ida
# delete snd_init.c
#
# Revision 1.8  2004/07/16 06:42:49  ida
# fix Project name
#
# Revision 1.7  2004/07/15 00:01:28  ida
# Build ARM & THUMB code always
#
# Revision 1.6  2004/06/28 07:14:11  ida
# add snd_alarm.c
#
# Revision 1.5  2004/06/14 04:42:49  nishida
# スペルミス（insructions）を修正。
#
# Revision 1.4  2004/06/11 01:52:37  ida
# port -> command
#
# Revision 1.3  2004/05/13 01:02:42  ida
# add snd_data.c
#
# Revision 1.2  2004/05/06 04:48:20  ida
# snd_work.c : ARM7/src -> commom/src
#
# Revision 1.1  2004/03/26 06:25:52  ida
# create snd_drv
#
# Revision 1.6  2004/03/08 06:55:25  ida
# add newline
#
# Revision 1.5  2004/03/08 04:41:13  ida
# rename c files
#
# Revision 1.4  2004/03/08 03:03:47  ida
# ARM7/src/bank.c -> common/src/snd_bank.c
#
# Revision 1.3  2004/02/25 04:25:56  ida
# snd_util.c ARM7 -> common
#
# Revision 1.2  2004/02/20 13:07:51  yasu
# Change INSTALL_DIR
#
# Revision 1.1  2004/02/20 03:56:59  ida
# add ARM9 snd library
#
# $NoKeywords: $
#----------------------------------------------------------------------------

SUBDIRS		=

#----------------------------------------------------------------------------

# build ARM & THUMB libraries
NITRO_CODEGEN_ALL =	True

SRCDIR		=	../common/src src

SRCS		=	\
			snd_interface.c	\
			snd_main.c	\
			snd_command.c		\
			snd_alarm.c			\
			snd_work.c	\
			snd_util.c	\
			snd_bank.c	\
			snd_data.c

TARGET_LIB	=	libsnd$(NITRO_LIBSUFFIX).a

include	$(NITROSDK_ROOT)/build/buildtools/commondefs

INSTALL_TARGETS	=	$(TARGETS)
INSTALL_DIR	=	$(NITRO_INSTALL_LIBDIR)

#----------------------------------------------------------------------------

do-build:		$(TARGETS)

include	$(NITROSDK_ROOT)/build/buildtools/modulerules


#===== End of Makefile =====
