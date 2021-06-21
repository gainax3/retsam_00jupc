#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - buildsetup - versions
# File:     Makefile
#
# Copyright 2003-2006 Nintendo.  All rights reserved.
#
# These coded insructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law.  They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Log: Makefile,v $
# Revision 1.10  2006/12/11 09:35:20  okubata_ryoma
# small fix
#
# Revision 1.9  2006/12/11 09:25:23  okubata_ryoma
# NitroSDKのメジャーバージョンアップによる変更
#
# Revision 1.8  2006/05/01 01:16:18  okubata_ryoma
# CWバージョン比較方法の変更にともなう変更
#
# Revision 1.7  2005/09/17 05:52:46  yasu
# コンパイラのバージョン情報を version.h に追加
#
# Revision 1.6  2005/07/06 23:57:13  yasu
# 著作年度修正
#
# Revision 1.5  2005/07/06 13:20:56  yasu
# SDK_VERSION_MAJOR を NITROSDK_VERSION_MAJOR に変更
#
# Revision 1.4  2005/07/06 13:16:55  yasu
# SDK_VERSION_MAJOR による makefile の選択
#
# Revision 1.3  2004/09/17 11:23:31  yasu
# update only newly build
#
# Revision 1.2  2004/09/17 10:45:56  yasu
# fix install directory
#
# Revision 1.1  2004/09/15 13:17:23  yasu
# Generate version.h before build tree
#
# $NoKeywords: $
#----------------------------------------------------------------------------

NITRO_FORCE_BUILD	= True
include	$(NITROSDK_ROOT)/build/buildtools/commondefs
-include $(SDK_VERINFO_CW_CC) $(SDK_VERINFO_CW_LD)

TARGETS			= version.h

ifeq	($(NITROSDK_VERSION_MAJOR),4)
TARGETS_MAKEFILE	= $(ROOT)/release/Makefile.4x
else
ifeq	($(NITROSDK_VERSION_MAJOR),3)
TARGETS_MAKEFILE	= $(ROOT)/release/Makefile.3x
else
ifeq	($(NITROSDK_VERSION_MAJOR),2)
TARGETS_MAKEFILE	= $(ROOT)/release/Makefile.2x
else
TARGETS_MAKEFILE	= $(ROOT)/release/Makefile
endif
endif
endif

INSTALL_TARGETS		= $(TARGETS)
INSTALL_DIR		= $(NITRO_INSTALL_INCDIR)/nitro
LDIRT_CLEAN		= $(TARGETS)

include	$(NITROSDK_ROOT)/build/buildtools/modulerules

do-build:	$(TARGETS)

SDK_BUILDVER_CW_CC	= $(shell echo $(SDK_CW_BUILD_VERSION_CC) | sed -e 's/^\(.\)/\1/')
SDK_BUILDVER_CW_LD	= $(shell echo $(SDK_CW_BUILD_VERSION_LD) | sed -e 's/^\(.\)/\1/')
SDK_BUILDNUM_CW_CC	= $(shell echo $(SDK_CW_BUILD_NUMBER_CC) | sed -e 's/^0*\(.\)/\1/')
SDK_BUILDNUM_CW_LD	= $(shell echo $(SDK_CW_BUILD_NUMBER_LD) | sed -e 's/^0*\(.\)/\1/')

MAKEVER_FLAGS		= VERSIONFILE=$(TARGETS) \
			  SDK_BUILDVER_CW_CC=$(SDK_BUILDVER_CW_CC) \
			  SDK_BUILDVER_CW_LD=$(SDK_BUILDVER_CW_LD) \
			  SDK_BUILDNUM_CW_CC=$(SDK_BUILDNUM_CW_CC) \
			  SDK_BUILDNUM_CW_LD=$(SDK_BUILDNUM_CW_LD)

$(TARGETS):	$(TARGETS_MAKEFILE)
		@$(ECHO) SDK_BUILDVER_CW_CC=$(SDK_BUILDVER_CW_CC)
		@$(ECHO) SDK_BUILDVER_CW_LD=$(SDK_BUILDVER_CW_LD)
		@$(ECHO) SDK_BUILDNUM_CW_CC=$(SDK_BUILDNUM_CW_CC)
		@$(ECHO) SDK_BUILDNUM_CW_LD=$(SDK_BUILDNUM_CW_LD)
		@$(MAKE) -s -f $(TARGETS_MAKEFILE) $(MAKEVER_FLAGS) version > /dev/null


#===== End of Makefile =====
