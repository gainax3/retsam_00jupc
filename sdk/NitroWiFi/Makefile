#! make -f
#----------------------------------------------------------------------------
# Project:  NitroWiFi - root
# File:     Makefile
#
# Copyright 2005-2007 Nintendo.  All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law.  They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Log: Makefile,v $
# Revision 1.10  2007/03/08 07:13:57  kitase_hirotake
# 2006->2007 対応
#
# Revision 1.9  2006/08/11 03:43:12  yasu
# 英語チェックの修正
#
# Revision 1.8  2006/03/15 09:52:36  kitase_hirotake
# sdkindent to wifiindent
#
# Revision 1.7  2006/01/25 00:10:31  yasu
# 年号の前に空白がある場合に判定ミスしていたのを修正
#
# Revision 1.6  2006/01/16 00:40:01  kitase_hirotake
# 不必要なインデントを行わないように変更
#
# Revision 1.5  2006/01/11 06:07:22  yasu
# 2005->2006対応
#
# Revision 1.4  2005/07/20 03:59:56  yasu
# 名前が '_' のファイルがあれば検索をスキップするようにした．
#
# Revision 1.3  2005/07/07 01:14:43  yasu
# NitroSDK/Makefile との内容のリンク作業
#
# Revision 1.2  2005/06/29 00:49:57  yasu
# code のチェックを追加
#
# Revision 1.1  2005/06/27 00:07:07  yasu
# 初版追加
#
# $NoKeywords: $
#----------------------------------------------------------------------------

include	$(NITROWIFI_ROOT)/build/buildtools/commondefs


#----------------------------------------------------------------------------

SUBDIRS 	= build


#----------------------------------------------------------------------------

include	$(NITROWIFI_ROOT)/build/buildtools/modulerules


#----------------------------------------------------------------------------
.PHONY:	indent-all do-indent do-rename do-diff

WIFIINDENT       = $(NWIFI_INSTALL_TOOLSDIR)/bin/wifiindent
WIFIDIFF         = $(NWIFI_INSTALL_TOOLSDIR)/bin/wifiindent.libdiff
WIFIDIR_BUILD		?= build
WIFIDIR_INCLUDE		 = include
WIFIDIR_BUILDOPT		 = 
WIFICOPYRIGHT		 = $(NWIFI_INSTALL_TOOLSDIR)/bin/wifiindent.copyright

indent-all:
	( export LMACRO_FLAGS=-DSDK_NO_MESSAGE=1;		\
	  export NITRO_NO_OPT_G=1;				\
	  $(MAKE) -kB $(WIFIDIR_BUILDOPT) -C $(WIFIDIR_BUILD) $(WIFIINDENT_BUILD);	\
	  $(REMAKE) do-rename;					\
	  $(REMAKE) do-indent;					\
	  $(REMAKE) do-copyright;				\
	  $(MAKE) -skB $(WIFIDIR_BUILDOPT) -C $(WIFIDIR_BUILD) $(WIFIINDENT_BUILD); \
	  $(REMAKE) do-diff )

indent-sub:
	( export LMACRO_FLAGS=-DSDK_NO_MESSAGE=1;		\
	  export NITRO_NO_OPT_G=1;				\
	  $(REMAKE) do-indent;					\
	  $(MAKE) -skB -C $(WIFIDIR_BUILD) $(WIFIINDENT_BUILD);	\
	  $(REMAKE) do-diff )

indent-diff:
	  $(REMAKE) do-diff

do-indent:
	@echo ===== INDENT STAGE =====
	/bin/find $(WIFIDIR_BUILD)   -type f \
		-name '*.[ch]' ! -name '*.yy.c' ! -name '*.yy.patched.c' \
		-size -128k -print -exec $(WIFIINDENT) {} \;
	/bin/find $(WIFIDIR_INCLUDE) -type f \
		-name '*.[ch]' ! -name '*.yy.c' ! -name '*.yy.patched.c' \
		-size -128k -print -exec $(WIFIINDENT) {} \;

do-copyright:
	@echo ===== COPYRIGHT STAGE =====
	/bin/find $(WIFIDIR_BUILD)   -type f \
		-name '*.[ch]' ! -name '*.yy.c' ! -name '*.yy.patched.c' \
		-size -128k -print \
		-mtime -`/bin/date +%j` \
		-exec $(WIFICOPYRIGHT) {} \;
	/bin/find $(WIFIDIR_INCLUDE) -type f \
		-name '*.[ch]' ! -name '*.yy.c' ! -name '*.yy.patched.c' \
		-size -128k -print \
		-mtime -`/bin/date +%j` \
		-exec $(WIFICOPYRIGHT) {} \;

do-rename:
	@echo ===== RENAME STAGE =====
	/bin/find $(WIFIDIR_BUILD) -type f -name '*.o' -exec $(MV) {} {}.backup \;

do-diff:
	@echo ===== COMPARE STAGE =====
	/bin/find $(WIFIDIR_BUILD) -type f -name '*.o' -exec $(WIFIDIFF) {} {}.backup \;

#----------------------------------------------------------------------------
.PHONY:	check_code check_lang_en check_copyright_date_2007

CHECKEES	= build include

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
