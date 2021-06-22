#! make -f
#----------------------------------------------------------------------------
# Project:  NitroWiFi - SOC - libraries
# File:     Makefile
#
# Copyright 2005 Nintendo.  All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law.  They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Log: Makefile,v $
# Revision 1.17  2005/08/23 04:42:25  yasu
# NITROWIFI_INSTALL_ROOT �֘A�̏C��
#
# Revision 1.16  2005/08/18 13:19:23  yasu
# cleanup �� list �̒ǉ�
#
# Revision 1.15  2005/08/08 06:16:43  seiki_masashi
# socl_ssl.c �𕜋A
# sed �ɓn���p�X�� cygwin �`���ɂȂ�悤�C��
#
# Revision 1.14  2005/08/07 13:03:20  yasu
# socl_misc.c �ǉ�/socl_ssl.c ���ꎞ�R�����g�A�E�g
#
# Revision 1.13  2005/08/06 11:55:54  seiki_masashi
# SOCL_EnableSsl ��ǉ�
#
# Revision 1.12  2005/07/30 15:31:14  yasu
# �R�}���h�p�C�v�����ɂƂ��Ȃ��C��
#
# Revision 1.11  2005/07/28 11:16:25  yasu
# SOC_Poll �̎���
#
# Revision 1.10  2005/07/25 10:15:01  yasu
# SOC ���C�u�����̒ǉ�
#
# Revision 1.9  2005/07/22 13:55:53  yasu
# listen accept �̒ǉ�
#
# Revision 1.8  2005/07/22 12:45:05  yasu
# �񓯊�����������
#
# Revision 1.7  2005/07/19 14:06:39  yasu
# SOCL_Read/Write �ǉ�
#
# Revision 1.6  2005/07/18 13:16:36  yasu
# ���M��M�X���b�h���쐬����
#
# Revision 1.5  2005/07/15 13:33:11  yasu
# soc ���C�u�����̃\�[�X����
#
# Revision 1.4  2005/06/28 02:35:14  yasu
# �\�P�b�g�X���b�h�쐬�֐���ʂ�
#
# Revision 1.3  2005/06/23 11:13:17  yasu
# NitroWiFi �� commondefs, modulerules ��ǂݍ���
#
# Revision 1.2  2005/06/22 10:52:13  yasu
# �A���t�@�ō쐬
#
# Revision 1.1  2005/06/20 08:42:29  terui
# �c���[�\����V�K�쐬
#
# $NoKeywords: $
#----------------------------------------------------------------------------

SUBDIRS			 =

#----------------------------------------------------------------------------
# build ARM & THUMB libraries
NITRO_CODEGEN_ALL 	?= True

#SRCDIR			= src
#INCDIR			=

SRCS			= socl_startup.c	\
			  socl_command.c	\
			  socl_create.c		\
			  socl_listen_accept.c	\
			  socl_bind.c		\
			  socl_read.c		\
			  socl_write.c		\
			  socl_shutdown.c	\
			  socl_close.c		\
			  socl_cleanup.c	\
			  socl_resolve.c	\
			  socl_poll.c		\
			  socl_const.c		\
			  socl_misc.c		\
			  socl_list.c		\
			  soc.c

SRCS			+= socl_ssl.c

TARGET_LIB		= libsoc$(NITRO_LIBSUFFIX).a

include	$(NITROWIFI_ROOT)/build/buildtools/commondefs

#----------------------------------------------------------------------------

INSTALL_TARGETS		= $(TARGETS)
INSTALL_DIR		= $(NWIFI_INSTALL_LIBDIR)

#----------------------------------------------------------------------------

do-build:		$(TARGETS)

include	$(NITROWIFI_ROOT)/build/buildtools/modulerules

ERRTABLE_H	= include/errtable.h

$(OBJDIR)/socl_misc.o:	$(ERRTABLE_H)

$(ERRTABLE_H): $(NWIFI_ROOT)/include/nitroWiFi/socl.h
	mkdir -p include
	sed -e '/^\/\/ ERROR-CODE BEGINS/,/^\/\/ ERROR-CODE ENDS/!d;\
		/SOC_/!d;\
		s/^.*SOC_\([A-Z]*\).*$$/{ SOC_\1, "\1" },/g' $< > $@

LDIRT_CLEAN	= $(ERRTABLE_H)

#===== End of Makefile =====