#! make -f
#----------------------------------------------------------------------------
# Project:  NITRO-System - libraries - gfd
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
# $Revision: 1.14 $
#----------------------------------------------------------------------------


#----------------------------------------------------------------------------

SRCS		=	gfd_TexVramMan.c			\
				gfd_PlttVramMan.c			\
				gfd_FrameTexVramMan.c		\
				gfd_FramePlttVramMan.c		\
				gfd_VramTransferManager.c	\
				gfdi_LinkedListVramMan_Common.c	\
				gfd_LinkedListTexVramMan.c	\
				gfd_LinkedListPlttVramMan.c	\

				
TARGET_LIB	=	libnnsgfd$(NITRO_LIBSUFFIX).a

SRCDIR		= 	src							\
				src/VramManager 			\
				src/VramTransferMan			
				
#LCFILE		=	# using default


INSTALL_TARGETS	=	$(TARGETS)
INSTALL_DIR		=	$(NITROSYSTEM_INSTALL_LIBDIR)

NITRO_BLXCHECKED = yes
#----------------------------------------------------------------------------
include	$(NITROSYSTEM_ROOT)/build/buildtools/commondefs

do-build:		$(TARGETS)


include	$(NITROSYSTEM_ROOT)/build/buildtools/modulerules

#===== End of Makefile =====
