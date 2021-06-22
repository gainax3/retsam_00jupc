#! make -f
#----------------------------------------------------------------------------
# Project:  NitroDWC - account - libraries
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
# Revision 1.4  2006/04/27 11:17:33  takayama
# NITRODWC_NOGS�I�v�V�������u���E�U��������ʏ�̂��̂ɏC���B
#
# Revision 1.3  2006/04/21 01:33:43  takayama
# NITORDWC_NOGS�ł̃r���h���̒����B
#
# Revision 1.2  2005/08/31 11:24:38  sasakit
# dwc_report��account���W���[���Ɉړ�
#
# Revision 1.1  2005/08/31 02:29:44  sasakit
# �������p�̃R�[�h��overlay�ŕ�������悤�ɁA���W���[���𕪂����B
#
# $NoKeywords: $
#----------------------------------------------------------------------------

SUBDIRS		=

#----------------------------------------------------------------------------

SRCS  = dwc_init.c   \
        dwc_report.c

ifndef NITRODWC_NOGS
SRCS += dwc_account.c
endif

NITRODWC_MODULE_NAME :=   account
TARGET_LIB	=	lib$(NITRODWC_LIB_PREFIX)$(NITRODWC_MODULE_NAME)$(NITRO_LIBSUFFIX).a

CCFLAGS_MSGSTYLE = -msgstyle gcc

#----------------------------------------------------------------------------

include	$(NITRODWC_ROOT)/build/buildtools/commondefs

INSTALL_TARGETS	=	$(TARGETS)
INSTALL_DIR		=	$(NITRODWC_INSTALL_LIBDIR)

#----------------------------------------------------------------------------

do-build:		$(TARGETS)

include	$(NITRODWC_ROOT)/build/buildtools/modulerules

#----------------------------------------------------------------------------

#===== End of Makefile =====