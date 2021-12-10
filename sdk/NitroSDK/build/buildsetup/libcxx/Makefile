#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - buildsetup - libcxx
# File:     Makefile
#
# Copyright 2003-2004 Nintendo.  All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law.  They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Log: Makefile,v $
# Revision 1.4  2004/08/10 02:32:20  yasu
# Fix when make install
#
# Revision 1.3  2004/07/28 09:45:30  yasu
# Support Mingw
#
# Revision 1.2  2004/06/28 11:41:44  yasu
# Don't install libcxx when SDK_CW_WA_LIBCXX is undefined
#
# Revision 1.1  2004/06/28 10:52:57  yasu
# add libcxx
#
# $NoKeywords:$
#----------------------------------------------------------------------------
NITRO_FORCE_BUILD	= True

include	$(NITROSDK_ROOT)/build/buildtools/commondefs

TARGET_SRC_LIBDIR	= $(CW_ARMDIR)/msl/MSL_C++/MSL_ARM/Lib
TARGET_SRC_LIBS		= MSL_C++_NITRO_A_LE.a		\
			  MSL_C++_NITRO_A_LE_strb.a	\
			  MSL_C++_NITRO_Ai_LE.a		\
			  MSL_C++_NITRO_Ai_LE_strb.a	\
			  MSL_C++_NITRO_T_LE.a		\
			  MSL_C++_NITRO_T_LE_strb.a

TMPDIR			= ./lib

# install
INSTALL_TARGETS	= $(addprefix $(TMPDIR)/,$(subst C++,CXX,$(TARGET_SRC_LIBS)))
INSTALL_DIR	= $(ROOT)/lib/CodeWarrior
LDIRT_CLEAN	= $(ROOT)/lib/CodeWarrior

#----------------------------------------------------------------------------

do-build:
	@$(MKDIRP) $(TMPDIR)
	@$(foreach FILE,$(TARGET_SRC_LIBS),\
		$(CP) $(TARGET_SRC_LIBDIR)/$(FILE) \
			$(TMPDIR)/$(subst _C++_,_CXX_,$(FILE)) $(AND)) true

include	$(NITROSDK_ROOT)/build/buildtools/modulerules

#----- End of Makefile -----
