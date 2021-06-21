#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - libraries - stubs
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
# Revision 1.7  2006/05/14 23:52:58  yasu
# 著作年度の変更
#
# Revision 1.6  2006/05/11 04:08:32  yasu
# vsprintf.o のビルド停止
#
# Revision 1.5  2004/05/06 10:18:18  yasu
# fix typo
#
# Revision 1.4  2004/03/05 09:09:40  yasu
# add vsnprintf stubs
#
# Revision 1.3  2004/02/25 09:13:11  yasu
# add interwroking code for CW
#
# Revision 1.2  2004/02/05 07:09:02  yasu
# change SDK prefix iris -> nitro
#
# Revision 1.1  2003/12/10 12:19:03  yasu
# ISD ライブラリの無い環境のために空の関数をダミーで用意し、stubs 下に置いた．
#
# $NoKeywords: $
#----------------------------------------------------------------------------

include	$(NITROSDK_ROOT)/build/buildtools/commondefs


#----------------------------------------------------------------------------

SUBDIRS =	isd cw
#SUBDIRS =	isd cw vsnprintf


#----------------------------------------------------------------------------

include	$(NITROSDK_ROOT)/build/buildtools/modulerules


#===== End of Makefile =====
