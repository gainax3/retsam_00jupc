#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - root
# File:     Makefile
#
# Copyright 2003-2007 Nintendo.  All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law.  They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Log: Makefile,v $
# Revision 1.24  2007/01/09 04:48:00  kitase_hirotake
# 2006->2007対応
#
# Revision 1.23  2006/08/11 03:40:50  yasu
# 英語チェックの修正
#
# Revision 1.22  2006/01/29 23:56:59  kitase_hirotake
# 年号の前に空白がある場合に判定ミスしていたのを修正
#
# Revision 1.21  2006/01/16 00:39:42  kitase_hirotake
# 不必要なインデントを行わないように変更
#
# Revision 1.20  2006/01/11 06:25:45  kitase_hirotake
# 2005->2006対応
#
# Revision 1.19  2005/07/20 03:59:49  yasu
# 名前が '_' のファイルがあれば検索をスキップするようにした．
#
# Revision 1.18  2005/07/18 01:33:00  yasu
# copyright 判定の修正
#
# Revision 1.17  2005/07/15 01:12:24  yasu
# Copyright チェックコードの修正
#
# Revision 1.16  2005/07/07 00:31:37  yasu
# 著作表示チェックの修正
#
# Revision 1.15  2005/07/06 00:03:49  yasu
# ターゲット check_code を追加する
#
# Revision 1.14  2005/02/28 04:36:25  yosizaki
# add do-copyright.
#
# Revision 1.13  2005/02/25 07:40:45  yasu
# indent 処理の確認時には CC のみを有効にする
#
# Revision 1.12  2004/10/25 11:25:26  yasu
# Use libdiff
#
# Revision 1.11  2004/10/05 02:30:37  yasu
# Set LMACRO_FLAGS=1 instead of MACRO_FLAGS
#
# Revision 1.10  2004/10/05 00:19:31  yasu
# Fix SDK_NO_MESSAGE
#
# Revision 1.9  2004/10/04 11:47:41  yasu
# find 条件修正
#
# Revision 1.8  2004/10/03 10:01:18  yasu
# Fix sdkindent rules
#
# Revision 1.7  2004/10/03 09:32:10  yasu
# renew indent rules
#
# Revision 1.6  2004/06/29 00:59:49  yasu
# Add setup libcxx for workaround against lcf error
#
# Revision 1.5  2004/05/06 10:18:18  yasu
# fix typo
#
# Revision 1.4  2004/03/17 07:31:04  yasu
# add indent correction rule
#
# Revision 1.3  2004/02/05 07:12:05  yasu
# change SDK prefix iris -> nitro
#
# Revision 1.2  2003/10/30 05:51:52  yasu
# build: ターゲットの削除
#
# Revision 1.1  2003/10/29 13:27:26  yasu
# make の伝播のフレームワーク完成
# all/install/clean/clobber が使える
#
# $NoKeywords: $
#----------------------------------------------------------------------------

include	$(NITROSDK_ROOT)/build/buildtools/commondefs


#----------------------------------------------------------------------------

SUBDIRS 	= build


#----------------------------------------------------------------------------

include	$(NITROSDK_ROOT)/build/buildtools/modulerules


#----------------------------------------------------------------------------
.PHONY:		setup

setup:
	$(MAKE) -C build/buildsetup/libcxx

#----------------------------------------------------------------------------
.PHONY:	indent-all do-indent do-rename do-diff

SDKINDENT	= $(NITRO_TOOLSDIR)/bin/sdkindent
SDKDIFF		= $(NITRO_TOOLSDIR)/bin/sdkindent.libdiff
SDKDIR_BUILD	?= build
SDKDIR_INCLUDE	= include
#SDKDIR_BUILDOPT	= LD=true MAKEROM=true MAKELCF=true
SDKDIR_BUILDOPT	= 
SDKCOPYRIGHT	= $(NITRO_TOOLSDIR)/bin/sdkindent.copyright

indent-all:
	( export LMACRO_FLAGS=-DSDK_NO_MESSAGE=1;		\
	  export NITRO_NO_OPT_G=1;				\
	  $(MAKE) -kB $(SDKDIR_BUILDOPT) -C $(SDKDIR_BUILD) $(SDKINDENT_BUILD);	\
	  $(REMAKE) do-rename;					\
	  $(REMAKE) do-indent;					\
	  $(REMAKE) do-copyright;				\
	  $(MAKE) -skB $(SDKDIR_BUILDOPT) -C $(SDKDIR_BUILD) $(SDKINDENT_BUILD); \
	  $(REMAKE) do-diff )

indent-sub:
	( export LMACRO_FLAGS=-DSDK_NO_MESSAGE=1;		\
	  export NITRO_NO_OPT_G=1;				\
	  $(REMAKE) do-indent;					\
	  $(MAKE) -skB -C $(SDKDIR_BUILD) $(SDKINDENT_BUILD);	\
	  $(REMAKE) do-diff )

indent-diff:
	  $(REMAKE) do-diff

do-indent:
	@echo ===== INDENT STAGE =====
	/bin/find $(SDKDIR_BUILD)   -type f \
		-name '*.[ch]' ! -name '*.yy.c' ! -name '*.yy.patched.c' \
		-size -128k -print -exec $(SDKINDENT) {} \;
	/bin/find $(SDKDIR_INCLUDE) -type f \
		-name '*.[ch]' ! -name '*.yy.c' ! -name '*.yy.patched.c' \
		-size -128k -print -exec $(SDKINDENT) {} \;

do-copyright:
	@echo ===== COPYRIGHT STAGE =====
	/bin/find $(SDKDIR_BUILD)   -type f \
		-name '*.[ch]' ! -name '*.yy.c' ! -name '*.yy.patched.c' \
		-size -128k -print \
		-mtime -`/bin/date +%j` \
		-exec $(SDKCOPYRIGHT) {} \;
	/bin/find $(SDKDIR_INCLUDE) -type f \
		-name '*.[ch]' ! -name '*.yy.c' ! -name '*.yy.patched.c' \
		-size -128k -print \
		-mtime -`/bin/date +%j` \
		-exec $(SDKCOPYRIGHT) {} \;

do-rename:
	@echo ===== RENAME STAGE =====
	/bin/find $(SDKDIR_BUILD) -type f -name '*.o' -exec $(MV) {} {}.backup \;

do-diff:
	@echo ===== COMPARE STAGE =====
	/bin/find $(SDKDIR_BUILD) -type f -name '*.o' -exec $(SDKDIFF) {} {}.backup \;

#----------------------------------------------------------------------------
.PHONY:	check_code check_lang_en check_copyright_date_2007

CHECKEES	= build include add-ins

check_code: check_lang_en check_copyright_date_2007

#
#  Check backslash code for language EN
#	Inverse result code of grep (e.g. grep returns 1 if not found)
#	 -> Error if grep finds 0x5c(\) on 2nd byte of 16bit-wide 
#           Japanese kanji code at the end of line
#
CHECK_EXCLUDES	= *.txt *.o *.a *.d *.map *.elf Entries* Repository Root

check_lang_en:
	@echo --- $@ ---
	-@(! grep -I -n -r $(patsubst %, --exclude='%',$(CHECK_EXCLUDES)) '[^[:space:] -~]\\[^ -~]\?$$' $(CHECKEES))

#
#  Check Copyright date for 2007
#
check_copyright_date_2007:
	@echo --- $@ ---
	-@for dir in $(CHECKEES); do \
	    /bin/find $$dir \
		\! \( -type d \( -name '_*' -o \
				 -name CVS  -o \
				 -exec /bin/test -e {}/_ \; \) -prune \) \
		-type f \
		\! -name '*.exe' \! -name '*.[dao]' \! -name '*.sbin' \
		\! -name '*.nlf' \! -name '*.nef'   \! -name '*.xMAP' \
		-mtime -`/bin/date +%j` \
		-exec grep -q 'Copyright [0-9 ,-]* Nintendo' {} \; \
		\! -exec grep -q 'Copyright [0-9 ,-]*2007' {} \; \
		-printf "Error: Copyright Date: %p\n"; \
	done

#===== End of Makefile =====
