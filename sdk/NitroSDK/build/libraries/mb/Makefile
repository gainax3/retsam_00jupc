#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - MB - libraries - ARM9
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
# Revision 1.7  2005/02/21 00:39:34  yosizaki
# replace prefix MBw to MBi.
#
# Revision 1.6  2004/11/22 12:56:56  takano_makoto
# mb_fake_child.c mb_wm.c mb_block.cを追加
#
# Revision 1.5  2004/11/11 11:45:24  yosizaki
# add mb_task.c
#
# Revision 1.4  2004/11/10 13:14:48  yosizaki
# add mb_cache.
#
# Revision 1.3  2004/09/17 04:02:37  miya
# 不要な部分の削除
#
# Revision 1.2  2004/09/03 07:04:36  sato_masaki
# ファイルを機能別に分割。
#
#
# $NoKeywords: $
#----------------------------------------------------------------------------

SUBDIRS			=

#----------------------------------------------------------------------------

# build ARM & THUMB libraries
NITRO_CODEGEN_ALL 	?= True

NITRO_PROC		= ARM9

SRCDIR			= ./src
INCDIR			= ./include


SRCS			= mb_parent.c mb_child.c mb_fileinfo.c \
			  mb_common.c mb_gameinfo.c  mb_wm_base.c mb_cache.c mb_task.c \
  			  mb_fake_child.c mb_wm.c mb_block.c

TARGET_LIB		= libmb$(NITRO_LIBSUFFIX).a


include	$(NITROSDK_ROOT)/build/buildtools/commondefs

INSTALL_TARGETS		= $(TARGETS)
INSTALL_DIR		= $(NITRO_INSTALL_LIBDIR)


#----------------------------------------------------------------------------

do-build:		$(TARGETS)


include	$(NITROSDK_ROOT)/build/buildtools/modulerules

#===== End of Makefile =====
