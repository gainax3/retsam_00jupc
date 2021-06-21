#! make -f
#---------------------------------------------------------------------------
#  Project:  NitroSDK - tools
#  File:     Makefile
#
#  Copyright 2003,2004 Nintendo.  All rights reserved.
#
#  These coded instructions, statements, and computer programs contain
#  proprietary information of Nintendo of America Inc. and/or Nintendo
#  Company Ltd., and are protected by Federal copyright law.  They may
#  not be disclosed to third parties or copied or duplicated in any form,
#  in whole or in part, without the prior written consent of Nintendo.
#
#  $Log: Makefile,v $
#  Revision 1.2  2004/09/21 02:23:20  yasu
#  Use makebanner -N option
#
#  Revision 1.1  2004/09/17 09:10:59  yasu
#  Build default banner file for SDK demos
#
#  $NoKeywords: $
#---------------------------------------------------------------------------
include $(NITROSDK_ROOT)/build/buildtools/commondefs

BANNER_ICON		= default.bmp
BANNER_SPEC		= default.bsf

TARGETS			= $(BANNER_SPEC:.bsf=.bnr)
INSTALL_DIR		= $(NITRO_INSTALL_INCDIR)/nitro/specfiles
INSTALL_TARGETS		= $(TARGETS)

BANNER_ICON_NAME	= $(basename $(BANNER_ICON))

LDIRT_CLEAN		= $(TARGETS) \
			  $(BANNER_ICON_NAME).nbfs \
			  $(BANNER_ICON_NAME).nbfc \
			  $(BANNER_ICON_NAME).nbfp \
			  $(TARGETS:.bnr=.srl)

include $(NITROSDK_ROOT)/build/buildtools/modulerules

#----------------------------------------------------------------------------
#  build
#----------------------------------------------------------------------------
do-build:		$(TARGETS)

$(TARGETS):		$(BANNER_SPEC) $(BANNER_ICON_NAME).nbfc $(BANNER_ICON_NAME).nbfp
			$(MAKEBANNER) -N $(BANNER_ICON_NAME) $(BANNER_SPEC) $(TARGETS)

#----------------------------------------------------------------------------
#  test
#----------------------------------------------------------------------------
include $(NITROSDK_ROOT)/build/buildtools/commondefs.emtype.ISD

test:			$(TARGETS)
			$(ADDBANNER) $(TARGETS) $(TARGETS:.bnr=.srl)
ifdef	ISD_NITLOAD
			$(ISD_NITLOAD) $(TARGETS:.bnr=.srl)
endif

#
