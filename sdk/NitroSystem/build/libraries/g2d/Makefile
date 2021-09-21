#! make -f
#----------------------------------------------------------------------------
# Project:  NITRO-System - libraries - g2d
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
# $Revision: 1.17 $
#----------------------------------------------------------------------------


#----------------------------------------------------------------------------

SRCS		=	g2d_NAN_load.c			\
				g2d_NOB_load.c			\
				g2d_NMC_load.c			\
				g2d_NEN_load.c			\
				g2d_NCG_load.c			\
				g2d_NCL_load.c			\
				g2d_NSC_load.c			\
				g2d_Load.c				\
				g2d_Softsprite.c		\
				g2d_Animation.c 		\
				g2d_OAM.c				\
				g2d_OAMEx.c				\
				g2d_PaletteTable.c		\
				g2d_Image.c				\
				g2d_SRTControl.c		\
				g2d_Node.c 				\
				g2d_CullingUtility.c	\
				g2d_CellAnimation.c		\
				g2d_MultiCellAnimation.c \
				g2d_Entity.c			\
				g2d_Renderer.c			\
				g2di_Mtx32.c			\
				g2di_debug.c			\
				g2d_CellTransferManager.c 		\
				g2d_OamSoftwareSpriteDraw.c		\
				g2d_Screen.c			\
				g2d_RendererCore.c		\
				g2d_Font.c			\
				g2d_CharCanvas.c	\
				g2d_TextCanvas.c	\
				g2d_NFT_load.c		\
				g2di_BitReader.c	\
				g2di_SplitChar.c	\
				g2di_BGManipulator.c	\
				
TARGET_LIB	=	libnnsg2d$(NITRO_LIBSUFFIX).a

#SRCDIR		=	# using default
SRCDIR		= 	src src/internal src/load
#LCFILE		=	# using default



INSTALL_TARGETS	=	$(TARGETS)
INSTALL_DIR		=	$(NITROSYSTEM_INSTALL_LIBDIR)

#----------------------------------------------------------------------------
include	$(NITROSYSTEM_ROOT)/build/buildtools/commondefs

do-build:		$(TARGETS)


include	$(NITROSYSTEM_ROOT)/build/buildtools/modulerules

#===== End of Makefile =====
