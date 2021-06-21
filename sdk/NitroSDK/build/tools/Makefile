#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - tools
# File:     Makefile
#
# Copyright 2003-2006 Nintendo.  All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law.  They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Log: Makefile,v $
# Revision 1.30  2006/11/22 00:38:36  okubata_ryoma
# small fix
#
# Revision 1.29  2006/11/21 06:20:57  yasu
# tools/bin 以下の bash スクリプトの改行を LF のみに自動変更する
#
# Revision 1.28  2005/11/30 01:31:48  yasu
# chscope の追加
#
# Revision 1.27  2005/10/21 00:04:12  okubata_ryoma
# xml2envの追加
#
# Revision 1.26  2005/10/20 02:13:58  kitase_hirotake
# init2env 追加
#
# Revision 1.25  2005/08/31 06:38:47  yada
# add loadrun
#
# Revision 1.24  2005/07/21 02:41:01  yada
# add buryarg
#
# Revision 1.23  2005/06/21 06:22:33  yasu
# 初版作成
#
# Revision 1.22  2005/06/20 07:07:38  yasu
# add defval
#
# Revision 1.21  2005/06/13 23:58:49  yasu
# Copyright 表示修正
#
# Revision 1.20  2005/06/13 02:57:50  yasu
# add bin2obj
#
# Revision 1.19  2004/11/02 11:30:20  takano_makoto
# emuchild.exeを公開
#
# Revision 1.18  2004/10/03 09:03:19  yasu
# add showversion
#
# Revision 1.17  2004/09/29 02:30:52  yasu
# add sdkindent
#
# Revision 1.16  2004/09/17 09:14:39  yasu
# Add addbanner and makebanner-default
#
# Revision 1.15  2004/09/01 07:17:45  yasu
# Add compstatic
#
# Revision 1.14  2004/09/01 01:29:25  miya
# add attachsign
#
# Revision 1.13  2004/08/30 08:41:14  yasu
# makebanner moves into CVS tree
#
# Revision 1.12  2004/07/28 07:00:21  yasu
# 'ppmconv' as new tool.
#
# Revision 1.11  2004/06/17 04:41:04  yosizaki
# add mic2wav.
#
# Revision 1.10  2004/05/06 10:18:18  yasu
# fix typo
#
# Revision 1.9  2004/04/06 13:10:53  yasu
# add slicerom
#
# Revision 1.8  2004/03/04 05:48:49  yasu
# add ntrcomp
#
# Revision 1.7  2004/02/23 06:33:47  yasu
# NITRO_PRIVATES -> NITRO_PRIVATE
#
# Revision 1.6  2004/02/05 07:09:03  yasu
# change SDK prefix iris -> nitro
#
# Revision 1.5  2004/01/19 23:55:15  yasu
# fix dir name
#
# Revision 1.4  2004/01/19 14:21:42  yasu
# makerom を一旦外す
#
# Revision 1.3  2004/01/14 02:11:38  yasu
# Connect make chain
#
# Revision 1.2  2003/10/30 05:51:52  yasu
# build: ターゲットの削除
#
# Revision 1.1  2003/10/29 13:27:27  yasu
# make の伝播のフレームワーク完成
# all/install/clean/clobber が使える
#
# $NoKeywords: $
#----------------------------------------------------------------------------

include	$(NITROSDK_ROOT)/build/buildtools/commondefs


#----------------------------------------------------------------------------

SUBDIRS 	=

ifdef	NITRO_PRIVATE
SUBDIRS 	+= makelcf		\
		   makelcf/test		\
		   makerom		\
		   makerom/test		\
		   addbanner		\
		   makebanner		\
		   makebanner/test	\
		   makebanner-default	\
		   makebanner		\
		   makebanner/test	\
		   compstatic		\
		   compstatic/test	\
		   ntrcompbw		\
		   ntrcomp		\
		   slicerom		\
		   mic2wav		\
		   ppmconv		\
		   attachsign		\
		   showversion		\
		   sdkindent		\
		   emuchild		\
		   bin2obj		\
		   defval		\
		   defval/test		\
		   buryarg		\
		   loadrun		\
		   xml2env		\
		   init2env		\
		   chscope

ifdef	IS_NITRO_DIR
SUBDIRS 	+= nitload-gcc
endif
endif

#------------------------------------------------------------------
#  インストール後、tools/bin 以下の bash スクリプトの CRLF を LF 
#  に変換
#------------------------------------------------------------------
ifeq	($(MAKECMDGOALS),install)
INSTALL_TARGETS		= TRUE
override DO_INSTALL	= $(REMAKE) changeD2U
endif

changeD2U:
	for i in $(NITRO_INSTALL_TOOLSDIR)/bin/*; do				\
	  if [ -f $$i ]; then							\
	    if [ "`sed '1!d;/^#!.*bash/!d' $$i | wc -l`" -eq "1" ]; then		\
	      tr -d '\r' < $$i > $$i.new;	\
	      mv $$i.new $$i;			\
	    fi					\
	  fi					\
	done

#----------------------------------------------------------------------------

include	$(NITROSDK_ROOT)/build/buildtools/modulerules


#===== End of Makefile =====
