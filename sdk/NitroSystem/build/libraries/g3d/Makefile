#! make -f
#----------------------------------------------------------------------------
# Project:  NITRO-System - libraries - g3d
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

SRCS		=	kernel.c                     \
				glbstate.c                   \
				anm.c                        \
				cgtool.c                     \
				sbc.c                        \
				1mat1shp.c                   \
				gecom.c                      \
				util.c                       \
				mem.c                        \
				model.c                      \
				binres/res_print.c           \
                binres/res_print_nsbca.c     \
                binres/res_print_nsbma.c     \
                binres/res_print_nsbta.c     \
                binres/res_print_nsbtp.c     \
                binres/res_print_nsbva.c     \
                binres/res_struct.c          \
                binres/res_struct_accessor.c \
				binres/res_struct_accessor_anm.c \
				anm/nsbca.c                  \
				anm/nsbma.c                  \
				anm/nsbta.c                  \
				anm/nsbtp.c                  \
				anm/nsbva.c                  \
				cgtool/basic.c               \
				cgtool/maya.c                \
				cgtool/si3d.c                \
				cgtool/3dsmax.c				 \
				cgtool/xsi.c

TARGET_LIB	=	libnnsg3d$(NITRO_LIBSUFFIX).a


#SRCDIR		=	# using default
SRCDIR		= 	src src/binres src/anm src/cgtool
#LCFILE		=	# using default
#LINCLUDES  =   ./


INSTALL_TARGETS	=	$(TARGETS)
INSTALL_DIR		=	$(NITROSYSTEM_INSTALL_LIBDIR)

#----------------------------------------------------------------------------
include	$(NITROSYSTEM_ROOT)/build/buildtools/commondefs



do-build:		$(TARGETS)


include	$(NITROSYSTEM_ROOT)/build/buildtools/modulerules

#===== End of Makefile =====


