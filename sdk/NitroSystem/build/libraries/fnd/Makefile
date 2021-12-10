#! make -f
#----------------------------------------------------------------------------
# Project:  NITRO-System - libraries - fnd
# File:     Makefile
#
# Copyright 2004-2007 Nintendo.  All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law.  They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Revision: 1.10 $
#----------------------------------------------------------------------------

SUBDIRS		=


#----------------------------------------------------------------------------

SRCS	= \
	list.c \
	heapcommon.c \
	expheap.c \
	frameheap.c \
	unitheap.c \
	archive.c \
	allocator.c


TARGET_LIB	= libnnsfnd$(NITRO_LIBSUFFIX).a


include	$(NITROSYSTEM_ROOT)/build/buildtools/commondefs

INSTALL_TARGETS	=	$(TARGETS)
INSTALL_DIR		=	$(NITROSYSTEM_INSTALL_LIBDIR)


#----------------------------------------------------------------------------

do-build:		$(TARGETS)


include	$(NITROSYSTEM_ROOT)/build/buildtools/modulerules


#===== End of Makefile =====
