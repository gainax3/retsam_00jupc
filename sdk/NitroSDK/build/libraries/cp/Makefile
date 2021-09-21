#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - CP - libraries
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
# Revision 1.7  2006/04/10 09:14:13  okubata_ryoma
# Copyright fix
#
# Revision 1.6  2006/04/10 09:05:27  kitase_hirotake
# add about SDK_TCM_APPLY
#
# Revision 1.5  2004/08/27 09:22:05  yasu
# Fix around NITRO_CODEGEN_ALL
#
# Revision 1.4  2004/07/14 14:12:07  yasu
# Change NITRO_CODEGEN=ALL -> NITRO_CODEGEN_ALL=True
#
# Revision 1.3  2004/07/14 11:52:57  yasu
# Build ARM & THUMB code always
#
# Revision 1.2  2004/05/06 10:18:18  yasu
# fix typo
#
# Revision 1.1  2004/05/06 02:52:33  takano_makoto
# Initial Update.
#
# $NoKeywords: $
#----------------------------------------------------------------------------

SUBDIRS			=

SRCS			= cp_context.c

#----------------------------------------------------------------------------

# build ARM & THUMB libraries
NITRO_CODEGEN_ALL 	?= True

# to avoid compiler-bug "blx**"
#NITRO_CONSERVATIVE=TRUE

TARGET_LIB		= libcp$(NITRO_LIBSUFFIX).a

include	$(NITROSDK_ROOT)/build/buildtools/commondefs


INSTALL_TARGETS		= $(TARGETS)
INSTALL_DIR		= $(NITRO_INSTALL_LIBDIR)

ifdef NITRO_TCM_APPLY
CCFLAGS			+= -DSDK_TCM_APPLY
endif

#----------------------------------------------------------------------------

do-build:		$(TARGETS)


include	$(NITROSDK_ROOT)/build/buildtools/modulerules


#===== End of Makefile =====
