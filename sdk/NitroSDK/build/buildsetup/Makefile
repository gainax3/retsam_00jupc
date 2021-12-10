#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - buildsetup - create register map
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
# Revision 1.14  2004/09/15 13:17:23  yasu
# Generate version.h before build tree
#
# Revision 1.13  2004/07/17 09:39:55  yasu
# Check modification time on install
#
# Revision 1.12  2004/06/28 10:52:49  yasu
# add libcxx
#
# Revision 1.11  2004/02/05 07:09:01  yasu
# change SDK prefix iris -> nitro
#
# Revision 1.10  2003/11/17 01:54:36  yasu
# ioreg 生成スクリプトを ioreg/ 以下へ移動, sp のリネーム
#
# Revision 1.9  2003/11/14 12:04:21  nishida_kenji
# fx追加
#
# Revision 1.8  2003/11/12 12:03:25  yasu
# make clobber で INSTALL_TARGETS 削除するようにした
#
# Revision 1.7  2003/11/06 13:42:01  yasu
# sp へのチェインを復活
#
# Revision 1.6  2003/11/05 10:32:23  yasu
# sp へのリンクを一時外しておく
#
# Revision 1.5  2003/11/05 09:58:40  yasu
# NITRO_INC_DIR -> NITRO_INCDIR の修正
#
# Revision 1.4  2003/11/05 07:05:14  yasu
# commondefs/modulerule 修正に伴う変更
# sp/ へのチェインの設定
#
# Revision 1.3  2003/10/30 05:42:36  yasu
# TARGET の指定まちがいの修正
#
# Revision 1.2  2003/10/30 04:48:06  yasu
# autogen ディレクトリ -> hw ディレクトリ
# インストール先を $NitroSDK/include/nitro/hw へ
#
# Revision 1.1  2003/10/29 13:27:27  yasu
# make の伝播のフレームワーク完成
# all/install/clean/clobber が使える
#
# $NoKeywords:$
#----------------------------------------------------------------------------

include	$(NITROSDK_ROOT)/build/buildtools/commondefs


#----------------------------------------------------------------------------
ifdef	NITRO_PRIVATE
SUBDIRS		+= versions
endif

SUBDIRS		+= ioreg ioreg_sp fx

#----------------------------------------------------------------------------

include	$(NITROSDK_ROOT)/build/buildtools/modulerules


#----- End of Makefile -----
