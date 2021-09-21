#! make -f
#----------------------------------------------------------------------------
# Project:  NitroDWC - base - libraries
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
#
# $NoKeywords: $
#----------------------------------------------------------------------------

SUBDIRS		=

#----------------------------------------------------------------------------

SRCS  = 

ifndef NITRODWC_NOGS
SRCS += \
	ppw_lobby.cpp \
	ppw_innerInfo.cpp \
	ppw_parser.cpp \
	dwci_encsessioncpp.cpp \
	ppw_profile.cpp
endif

NITRODWC_LIB_MODULES_   = ilobby

NITRODWC_MODULE_NAME :=   ppwlobby
TARGET_LIB	=	lib$(NITRODWC_LIB_PREFIX)$(NITRODWC_MODULE_NAME)$(NITRO_LIBSUFFIX).a

CCFLAGS_MSGSTYLE = -msgstyle gcc

#----------------------------------------------------------------------------

include	$(NITRODWC_ROOT)/build/buildtools/commondefs

CXXFLAGS		+=	-RTTI off
INSTALL_TARGETS	=	$(TARGETS)
INSTALL_DIR		=	$(NITRODWC_INSTALL_LIBDIR)

# ilobbyの共通ヘッダ、ユーティリティをインクルード
LINCLUDES += $(NITRODWC_ROOT)/build/libraries/ilobby/include

#----------------------------------------------------------------------------

do-build:		$(TARGETS)

include	$(NITRODWC_ROOT)/build/buildtools/modulerules

#----------------------------------------------------------------------------

#===== End of Makefile =====
