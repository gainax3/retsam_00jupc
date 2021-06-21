#! make -f
#----------------------------------------------------------------------------
# Project:  NitroWiFi - add-ins - env - build
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
# Revision 1.3  2005/08/19 12:28:20  yasu
# add-ins のサポート
#
# Revision 1.2  2005/08/19 08:16:59  yasu
# commondefs.add-ins.env により WiFi のメインツリーへインストールするように修正
#
# Revision 1.1  2005/08/17 06:31:16  yada
# initial release
#
# $NoKeywords: $
#----------------------------------------------------------------------------

include	$(NITROWIFI_ROOT)/build/buildtools/commondefs

#----------------------------------------------------------------------------
SUBDIRS_SETUP		= buildtools
SUBDIRS_LIBS		= libraries
SUBDIRS_DEMOS		= demos
SUBDIRS_CLEANUP		= buildtools

# if no ONLY defs, do build all
ifndef	NITRO_ONLY_SETUP
ifndef	NITRO_ONLY_LIBS
ifndef	NITRO_ONLY_DEMOS
ifndef	NITRO_ONLY_CLEANUP
NITRO_ONLY_SETUP	= True
NITRO_ONLY_LIBS		= True
NITRO_ONLY_DEMOS	= True
NITRO_ONLY_CLEANUP	= True
endif
endif
endif
endif

# build SETUP
ifdef	NITRO_ONLY_SETUP
SUBDIRS		+= $(SUBDIRS_SETUP)
endif

# build LIBS
ifdef	NITRO_ONLY_LIBS
SUBDIRS		+= $(SUBDIRS_LIBS)
endif

# build DEMOS
ifdef	NITRO_ONLY_DEMOS
SUBDIRS		+= $(SUBDIRS_DEMOS)
endif

# build CLEANUP
ifdef	NITRO_ONLY_CLEANUP
SUBDIRS		+= $(SUBDIRS_CLEANUP)
endif

#----------------------------------------------------------------------------
include	$(NITROWIFI_ROOT)/build/buildtools/modulerules

#===== End of Makefile =====
