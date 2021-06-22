#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - WM - libraries
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
# Revision 1.12  2004/09/10 11:04:35  terui
# wm.c�̕����ɔ����C���B
#
# Revision 1.11  2004/09/09 01:56:58  terui
# wm.c�t�@�C�������ɔ���SRCS��wm_asyncApi.c��ǉ��B
#
# Revision 1.10  2004/08/27 09:22:05  yasu
# Fix around NITRO_CODEGEN_ALL
#
# Revision 1.9  2004/08/19 04:32:32  yosiokat
# NITRO_CODEGEN_ALL = True��L���ɁB
#
# Revision 1.8  2004/07/24 02:25:38  yasu
# Dechain wm_lib subdirectory
#
# Revision 1.7  2004/07/20 10:50:36  terui
# wm_lib��wm�̕ʃ��C�u�������ɔ����ύX
#
# Revision 1.6  2004/07/20 07:53:14  terui
# wm_lib.c wm_tool.c ni.c���b���SRCS�ɒǉ�
#
# Revision 1.5  2004/07/20 01:26:43  terui
# SRCS�����ʃ��C�u��������U�폜
#
# Revision 1.4  2004/07/14 14:12:07  yasu
# Change NITRO_CODEGEN=ALL -> NITRO_CODEGEN_ALL=True
#
# Revision 1.3  2004/07/08 10:07:34  terui
# WM7�����J�n�ɔ����ύX
#
# Revision 1.2  2004/06/18 10:22:02  yasu
# fix typo
#
# Revision 1.1  2004/05/26 08:54:51  terui
# �����g�ݍ��݂ɔ����ǉ�
#
# $NoKeywords: $
#----------------------------------------------------------------------------

#SUBDIRS		= wm_lib

# Codegen for sub processer
NITRO_PROC		=  ARM9
NITRO_CODEGEN_ALL	?= True

SRCDIR			= ./src
INCDIR			= ../inc ./inc

SRCS			= 	wm_system.c	\
				wm_sync.c	\
				wm_standard.c	\
				wm_mp.c		\
				wm_dcf.c	\
				wm_ds.c		\
				wm_ks.c		\
				wm_etc.c	\


TARGET_LIB		= libwm$(NITRO_LIBSUFFIX).a

include	$(NITROSDK_ROOT)/build/buildtools/commondefs

INSTALL_TARGETS		= $(TARGETS)

INSTALL_DIR		= $(NITRO_INSTALL_LIBDIR)

GDIRT_INSTALLED		= $(addprefix $(INSTALL_DIR)/,$(notdir $(INSTALL_TARGETS)))

#----------------------------------------------------------------------------

do-build:		$(TARGETS)

include	$(NITROSDK_ROOT)/build/buildtools/modulerules


#===== End of Makefile =====