#! make -f
#----------------------------------------------------------------------------
# Project:  NitroDWC
# File:     Makefile
#
# Copyright 2003-2005 Nintendo.  All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law.  They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Log: Makefile,v $
# Revision 1.5  2006/03/27 06:37:41  takayama
# add-insディレクトリをプライベートにした。
#
# Revision 1.4  2005/08/20 04:11:10  sasakit
# 無駄なコメントの削除
#
# Revision 1.3  2005/08/04 12:45:52  sasakit
# スナップショット作成の仕組みを作った。
#
# Revision 1.1  2005/08/04 09:33:05  sasakit
# NitroSDKリリース用のMakefileを移植してみた。
#
#
# $NoKeywords: $
#----------------------------------------------------------------------------

include $(NITRODWC_ROOT)/build/buildtools/commondefs

#----------------------------------------------------------------------------

ifdef NITRODWC_PRIVATE
SUBDIRS = add-ins
endif

SUBDIRS += build

#----------------------------------------------------------------------------

include $(NITRODWC_ROOT)/build/buildtools/modulerules

#===== End of Makefile =====
