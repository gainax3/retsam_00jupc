#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - libraries - spi - tp.c
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
# Revision 1.12  2005/05/26 02:50:01  terui
# ライブラリを作成するコア部分を Makefile.CORE に切り出し
# SUBDIRS に TS-Prototype/undefine を追加
#
# Revision 1.11  2004/08/27 08:49:12  yasu
# Create libspi.a for all version of TS
#
# Revision 1.10  2004/07/29 08:59:58  yada
# add pm.c
#
# Revision 1.9  2004/07/14 14:12:07  yasu
# Change NITRO_CODEGEN=ALL -> NITRO_CODEGEN_ALL=True
#
# Revision 1.8  2004/07/14 11:52:57  yasu
# Build ARM & THUMB code always
#
# Revision 1.7  2004/05/31 08:51:37  terui
# リビジョン1.6のコメント修正。
# Add mic.c in SRCS.
#
# Revision 1.6  2004/05/31 08:47:40  terui
# Add mic.c in SRCS.
#
# Revision 1.5  2004/05/20 09:45:04  terui
# Delete "nvram.c" from SRCS.
#
# Revision 1.4  2004/05/19 12:36:52  terui
# Add nvram in SRCS.
#
# Revision 1.3  2004/05/06 10:18:18  yasu
# fix typo
#
# Revision 1.2  2004/04/15 11:42:32  takano_makoto
# Add TP to ARM9.
#
# Revision 1.1  2004/04/05 04:47:35  terui
# Initial upload
#
# $NoKeywords: $
#----------------------------------------------------------------------------

ifeq		($(NITRO_PLATFORM),TEG)
SUBDIRS		=	TS-Prototype/undefine
else
SUBDIRS		=	TS-Prototype/0			\
				TS-Prototype/100		\
				TS-Prototype/200		\
				TS-Prototype/undefine
endif

#----------------------------------------------------------------------------

include	$(NITROSDK_ROOT)/build/buildtools/commondefs

include	$(NITROSDK_ROOT)/build/buildtools/modulerules


#===== End of Makefile =====
