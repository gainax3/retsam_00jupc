#! make -f
#----------------------------------------------------------------------------
#  Project:  NitroSDK - buildtools - compiler dependant settings for CodeWarrior
#  File:     modulerules.cctype.CW
#
#  Copyright 2003-2007 Nintendo.  All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law.  They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Log: modulerules.cctype.CW,v $
# Revision 1.74  2007/07/09 12:53:01  yasu
# NITRO_TRGNAME のサポート
#
# Revision 1.73  2007/04/23 04:22:40  yasu
# 著作年度の修正
#
# Revision 1.72  2007/04/10 05:52:41  yasu
# MAKELCF_FORMAT を ?= によって定義するように変更
#
# Revision 1.71  2006/06/06 04:39:45  okubata_ryoma
# CWのコンパイラとリンカのバージョン情報を追記
#
# Revision 1.70  2006/06/06 04:32:06  yasu
# build バージョンで 2～3桁の場合は、100 倍する
#
# Revision 1.69  2006/06/06 01:02:28  yasu
# build バージョンにおいて小数点以下の値を拾うように変更
#
# Revision 1.68  2006/05/10 02:07:35  yasu
# makelcf の -V オプションの対応
#
# Revision 1.67  2006/05/01 01:15:43  okubata_ryoma
# CWバージョン比較方法の変更にともなう変更
#
# Revision 1.66  2006/03/20 08:01:28  okubata_ryoma
# sed コマンドのバージョンによるビルドエラーを防ぐための修正
#
# Revision 1.65  2006/02/21 00:11:15  okubata_ryoma
# Copyright fix
#
# Revision 1.64  2006/02/13 08:32:39  okubata_ryoma
# CW2.0betaへの対応
#
# Revision 1.63  2005/09/17 06:17:51  yasu
# コンパイラのバージョン情報処理の修正
#
# Revision 1.62  2005/09/17 04:56:24  yasu
# コンパイラのバージョン情報の更新
#
# Revision 1.61  2005/08/23 05:12:25  yasu
# precompile ヘッダ作成時に SDK 関連でない -D オプションを排除する
#
# Revision 1.60  2005/08/08 00:06:56  yasu
# mongoose のビルド対策のため TARGET_BIN_BASENAME を定義し、TARGET_NAME と使い分ける
#
# Revision 1.59  2005/08/07 13:15:48  yasu
# ライブラリパスを vpath に設定
#
# Revision 1.58  2005/07/26 07:29:58  yasu
# プリコンパイルヘッダのキャッシュ位置を変更した
#
# Revision 1.57  2005/07/26 00:40:56  seiki_masashi
# プリコンパイルヘッダの作成時に -include をダミーに置換
#
# Revision 1.56  2005/07/25 05:02:02  yasu
# PCH の削除のトリガを make clean から make clobber に変更
#
# Revision 1.55  2005/07/25 01:31:12  yasu
# .pch -> .mch への変更
#
# Revision 1.54  2005/07/21 11:55:21  yasu
# EDEPENDS_NEF/BIN を追加
#
# Revision 1.53  2005/07/14 05:11:49  yasu
# プリコンパイルヘッダを C と C++ で分別する
#
# Revision 1.52  2005/07/14 02:27:36  yasu
# NITRO_STD_PCHDR をモジュール毎に抑制できる仕組みの追加
#
# Revision 1.51  2005/07/11 06:12:37  yasu
# pch ファイルの依存関係のチェックの追加
#
# Revision 1.50  2005/07/08 13:25:23  yasu
# プリコンパイルヘッダのテストサポート
#
# Revision 1.49  2005/06/22 09:40:07  yasu
# ELIBRARIES のサポート
#
# Revision 1.48  2005/06/02 00:10:41  seiki_masashi
# copyright 表記の修正
#
# Revision 1.47  2005/06/01 07:33:30  seiki_masashi
# #include のサーチパスの先頭に呼び出し元ファイルのあるディレクトリを追加
#
# Revision 1.46  2004/12/27 03:51:22  yasu
# Change CC->MWCC , AS->MWAS, LD->MWLD and AR->MWAR
#
# Revision 1.45  2004/11/05 06:42:09  yasu
# Add version info for CW NITRO V1.0
#
# Revision 1.44  2004/10/03 02:04:03  yasu
# Support component file list for compstatic tool
#
# Revision 1.43  2004/09/17 03:35:34  yasu
# Invalidate NITRO_COMPRESS for ARM7 build
#
# Revision 1.42  2004/09/09 11:41:32  yasu
# Support NITRO_DIGEST
#
# Revision 1.41  2004/09/08 00:06:26  yasu
# Set NITRO_STDOUT to '> /dev/null' if -s option defined
#
# Revision 1.40  2004/09/06 05:34:38  yasu
# Add -a option on compstatic
#
# Revision 1.39  2004/09/06 01:08:41  yasu
# Add -c option on compstatic
#
# Revision 1.38  2004/09/04 12:37:52  yasu
# Add OverlayTable for compstatic tool
#
# Revision 1.37	 2004/09/01 07:30:03  yasu
# Support NITRO_COMPRESS
#
# Revision 1.36	 2004/08/10 14:18:48  yasu
# Add dependancies for makelcf
#
# Revision 1.35	 2004/08/10 00:13:46  yasu
# Support CodeWarrior 0.6.1
#
# Revision 1.34	 2004/07/26 08:11:08  yasu
# Rename ADDRESS_ARM7 -> ADDRESS_DTCM
#
# Revision 1.33	 2004/07/26 04:59:02  yasu
# Add ADDRESS_ARM7 on makelcf
#
# Revision 1.32	 2004/07/18 12:20:05  yasu
# Change ["] -> ['] of MAKELCF_DEFS for Mingw
#
# Revision 1.31	 2004/07/18 06:10:46  yasu
# show makelcf command line on making *.autogen.lcf
#
# Revision 1.30	 2004/07/13 12:34:19  yasu
# Check libcxx workaround on invoking linker
#
# Revision 1.29	 2004/07/10 04:55:30  yasu
# support USE_MY_RULE_A, USE_MY_RULE_NEF
#
# Revision 1.28	 2004/07/10 04:20:23  yasu
# Use response file to support overlay group
#
# Revision 1.27	 2004/06/28 10:52:11  yasu
# Delete group setting
#
# Revision 1.26	 2004/05/01 06:10:33  yada
# add build number rule for CW0.5
#
# Revision 1.25	 2004/04/21 11:22:06  yasu
# Support codecheck as workaround for blxeq/ne problem
#
# Revision 1.24	 2004/04/20 03:49:38  yasu
# add LDEPENDS_BIN/NEF for dependancies
#
# Revision 1.23	 2004/04/14 10:14:53  yasu
# delete LIBRARIES_STATIC
#
# Revision 1.22	 2004/04/03 08:43:35  yasu
# with MINGW, support detect CW versions
#
# Revision 1.21	 2004/04/03 08:01:43  yasu
# auto-detect CW verinfo and switch strb parameters
#
# Revision 1.20	 2004/04/01 12:54:38  yasu
# support LIBRARIES_STATIC for makerom
#
# Revision 1.19	 2004/03/23 09:28:57  yasu
# add $(MAKEFILE) for dependancy list of %.nef
#
# Revision 1.18	 2004/03/18 02:29:44  yasu
# change condition to switch rom packing method (makerom or elftobin)
#
# Revision 1.17	 2004/02/23 05:01:38  yasu
# axf->nef
#
# Revision 1.16	 2004/02/17 09:22:48  yasu
# support dependancies with MinGW
#
# Revision 1.15	 2004/02/16 01:24:24  yasu
# support file dependancies
#
# Revision 1.14	 2004/02/05 07:09:01  yasu
# change SDK prefix iris -> nitro
#
# Revision 1.13	 2004/01/15 10:49:31  yasu
# support overlay
#
# Revision 1.12	 2003/12/10 05:12:48  yasu
# ソースファイル指定に xxxx/src.c の形式を使えるように変更
# Makefile.srcs の追加
# IS_NITRO_DIR が定義してある場合に IS Debugger ライブラリをリンクするよう修正
#
# Revision 1.11	 2003/12/02 04:53:06  yasu
# ヘッダの表記修正
#
# Revision 1.10	 2003/12/02 03:45:11  yasu
# IS-NITRO-EMULATOR 用ライブラリ向けの修正
#
# Revision 1.9	2003/12/01 11:30:55  yasu
# ROM 作成ツールを objcopy -> elftobin へ変更した
#
# Revision 1.8	2003/11/28 02:01:12  yasu
# CW for NITRO の標準ライブラリを取り替えた
#
# Revision 1.7	2003/11/17 01:30:43  yasu
# C++ の定義を C の定義と分離
#
# Revision 1.6	2003/11/11 03:06:31  yasu
# base.a の位置の指定は lcf ファイルで行なうことにしたので前回の修正を無効にした
#
# Revision 1.5	2003/11/11 02:12:16  yasu
# base.a を OBJS の前にリンクするように修正
#
# Revision 1.4	2003/11/06 10:38:39  yasu
# ライブラリのサーチパスの設定ミスの修正
#
# Revision 1.3	2003/11/05 09:14:17  yasu
# ライブラリ生成用ルール追加
#
# Revision 1.2	2003/11/05 07:12:36  yasu
# コンパイルの依存ルールの仮完成
#
# Revision 1.1	2003/10/29 06:47:28  yasu
# Support "make clean/clobber" only
#
# $NoKeywords: $
#----------------------------------------------------------------------------

AINCLUDE_DIRS		= $(LINCLUDES)	   $(EINCLUDES)     $(GINCLUDES)
ALIBRARY_DIRS		= $(LLIBRARY_DIRS) $(ELIBRARY_DIRS) $(GLIBRARY_DIRS)
ALIBRARIES		= $(LLIBRARIES)	   $(ELIBRARIES)    $(GLIBRARIES)

#
# (make version 3.80)
# 
# VPATH does not support the path name included white space like "C:/Program Files"
#	Please, be careful !!!
#
ifneq ($(OS),Windows_NT)
ALIBVPATH		 = $(subst : ,,$(patsubst %:,/:,$(subst :,: ,$(ALIBRARY_DIRS))))
else
ifdef	MSYSTEM
NITRO_LIBVPATH_		 = $(subst \:,\ ,$(subst $(SPACE),:,$(ALIBRARY_DIRS)))
else
CYGDRV			?= $(shell cygpath -u c:)/..
ALIBVPATH		 = $(subst : ,,$(patsubst %:,$(CYGDRV)/%:,$(subst :,: ,$(ALIBRARY_DIRS))))
endif
endif

vpath	%.a		$(ALIBVPATH)
vpath	%.o		$(ALIBVPATH)

INCLUDE_DIRS		= $(subst \ -I,\ ,$(addprefix -I,$(call PATH_UNIX2DOS,$(AINCLUDE_DIRS))))
LIBRARY_DIRS		= $(subst \ -L,\ ,$(addprefix -L,$(call PATH_UNIX2DOS,$(ALIBRARY_DIRS))))
LIBRARIES		= $(subst \ -l,\ ,$(addprefix -l,$(ALIBRARIES)))

INCLUDES		= -gccinc -I. $(INCLUDE_DIRS)

ifdef	MSYSTEM		# for MinGW32
QUOTE			='
else
QUOTE			="
endif

#----------------------------------------------------------------------------

.PRECIOUS:	$(OBJDIR)/%.o $(BINDIR)/%.nef $(LCFILE) $(PCHDR_H) $(PCHDR_PCH)

#
# Generate dependency files
#
DEPEND_RULE	= $(QUOTE)/ARM_EABI_Support/d;s/\\x0D//g;s/\\\\\\([^ ]\\)/\\/\\1/g;$(QUOTE)

define MOVE_SRC_DEPEND
	$(SED) -e $(DEPEND_RULE) -e $(PATH_DEPEND_RULE) $(*F).d > $(DEPENDDIR)/$(*F).d && $(RM) $(*F).d
endef


ifdef	NITRO_STD_PCHDR
ifndef	NITRO_NO_STD_PCHDR

# .mch
PCHDR_SRC	= $(ROOT)/include/nitro.h
PCHDR_CACHE	= $(ROOT)/cache/include

PCHDR_C_DEFS	= $(sort $(filter -DSDK_%,$(CCFLAGS)))
PCHDR_C_NODEF	= $(filter-out -D%,$(CCFLAGS))
PCHDR_C_NAME	= $(subst $(SPACE),,$(subst -DSDK_,.,$(subst =,-,$(PCHDR_C_DEFS))))
PCHDR_C_BASE	= $(PCHDR_CACHE)/$(notdir $(PCHDR_SRC))$(PCHDR_C_NAME).C
PCHDR_C_MCH	= $(PCHDR_C_BASE).mch
PCHDR_C_DEPEND	= $(PCHDR_C_BASE).depend
PCHDR_CCFLAGS	= -include $(call PATH_UNIX2DOS,$(PCHDR_C_MCH))
CCFLAGS_NOLDEF	= $(PCHDR_C_DEFS) $(PCHDR_C_NODEF) 

PCHDR_CXX_DEFS	= $(sort $(filter -DSDK_%,$(CXXFLAGS)))
PCHDR_CXX_NODEF	= $(filter-out -D%,$(CXXFLAGS))
PCHDR_CXX_NAME	= $(subst $(SPACE),,$(subst -DSDK_,.,$(subst =,-,$(PCHDR_CXX_DEFS))))
PCHDR_CXX_BASE	= $(PCHDR_CACHE)/$(notdir $(PCHDR_SRC))$(PCHDR_CXX_NAME).CXX
PCHDR_CXX_MCH	= $(PCHDR_CXX_BASE).mch
PCHDR_CXX_DEPEND= $(PCHDR_CXX_BASE).depend
PCHDR_CXXFLAGS	= -include $(call PATH_UNIX2DOS,$(PCHDR_CXX_MCH))
CXXFLAGS_NOLDEF	= $(PCHDR_CXX_DEFS) $(PCHDR_CXX_NODEF) 

GDIRT_CLOBBER	+= $(PCHDR_CACHE)/$(notdir $(PCHDR_SRC)).*

ifeq	($(MAKECMDGOALS),do-build)

## $(call BUILD_PCHDR,$(BASE),$(CCFLAGS))
define BUILD_PCHDR
	@$(CP) $(PCHDR_SRC) $(1).h
	($(CD) $(dir $(1).mch) && \
	 $(CC) $(subst -include ,-DSDK_PCHDR_DUMMY_,$(2)) $(INCLUDES) -MD -precompile $(notdir $(1).mch) $(notdir $(1).h))
	@($(ECHO) $(1).mch: $(PCHDR_SRC) \\       >  $(1).depend && \
	  $(SED) -e '1d' -e $(DEPEND_RULE) -e $(PATH_DEPEND_RULE) $(1).d >> $(1).depend && $(RM) $(1).d $(1).h)
endef

# precomipled header for C
ifneq	($(strip $(OBJS_CC)),)
-include $(PCHDR_C_DEPEND)

$(PCHDR_C_DEPEND): $(PCHDR_C_MCH)

$(PCHDR_C_MCH):
	$(call BUILD_PCHDR,$(PCHDR_C_BASE),$(CCFLAGS_NOLDEF))
endif

# precomipled header for C++
ifneq	($(strip $(OBJS_CXX)),)
-include $(PCHDR_CXX_DEPEND)

$(PCHDR_CXX_DEPEND): $(PCHDR_CXX_MCH)

$(PCHDR_CXX_MCH):
	$(call BUILD_PCHDR,$(PCHDR_CXX_BASE),$(CXXFLAGS_NOLDEF))
endif

endif	# do-build
endif	# NITRO_NO_STD_PCHDR
endif	# NITRO_STD_PCHDR

# .c
$(OBJS_CC):%.o:
	$(CC) $(CCFLAGS) $(INCLUDES) $(PCHDR_CCFLAGS) -c $< -MD -o $@
	@$(MOVE_SRC_DEPEND)

# .cpp
$(OBJS_CXX):%.o:
	$(CC) $(CXXFLAGS) $(INCLUDES) $(PCHDR_CXXFLAGS) -c $< -MD -o $@
	@$(MOVE_SRC_DEPEND)

# .s
$(OBJS_AS):%.o:
	$(AS) $(ASFLAGS) $(INCLUDES) -c $< -MD -o $@
	@$(MOVE_SRC_DEPEND)

# .lcf .par
ifdef	LCFILE_SPEC

ADDRESS_DTCM	?= $(DEFAULT_ADDRESS_DTCM)

# exporting
GLIBRARIES_LINK	= $(filter-out $(LIBRARIES_UNLINK),$(ELIBRARIES) $(GLIBRARIES))
LLIBRARIES_LINK	= $(filter-out $(LIBRARIES_UNLINK),$(LLIBRARIES))

MAKELCF_DEFS	= -DNITROSDK_ROOT='$(ROOT)'			\
		  -DTARGET_NAME='$(TARGET_NAME)'		\
		  -DOBJS_STATIC='$(OBJS_STATIC)'		\
		  -DOBJS_AUTOLOAD='$(OBJS_AUTOLOAD)'		\
		  -DOBJS_OVERLAY='$(OBJS_OVERLAY)'		\
		  -DGLIBS='$(GLIBRARIES_LINK)'			\
		  -DLLIBS='$(LLIBRARIES_LINK)'			\
		  -DCW_LIBS='$(CW_LIBS)'			\
		  -DOBJDIR='$(OBJDIR)'				\
		  -DLIBDIR='$(LIBDIR)'				\
		  -DBINDIR='$(BINDIR)'				\
		  -DPROC='$(CODEGEN_PROC)'			\
		  -DPLATFORM='$(NITRO_PLATFORM)'		\
		  -DCODEGEN='$(CODEGEN_ARCH)'			\
		  -DBUILD='$(NITRO_BUILD_DIR)'			\
		  -DADDRESS_DTCM='$(ADDRESS_DTCM)'

MAKELCF_FORMAT	?= -V$(CW_MAJOR_VER)

#
# NITRO_TRGNAME が定義されていたら static セクションの名前の代わりに 
# TARGET_NAME の値をファイルの PREFIX として使う
#
MAKELCF_TRGNAME	?= $(if $(NITRO_TRGNAME),-T $(TARGET_NAME))

MAKELCF_OPTS	= $(MAKELCF_FLAGS) $(MAKELCF_DEFS) $(MAKELCF_FORMAT) $(MAKELCF_TRGNAME)

$(LCFILE_AUTOGEN): $(LCFILE_SPEC) $(LCFILE_TEMPLATE) $(MAKEFILE) $(NITRO_BUILDTOOLSDIR)/commondefs
	$(MAKELCF) $(MAKELCF_OPTS)  $< $(LCFILE_TEMPLATE) $@

$(LDRES_AUTOGEN):  $(LCFILE_SPEC) $(LDRES_TEMPLATE)  $(MAKEFILE) $(NITRO_BUILDTOOLSDIR)/commondefs
	@$(MAKELCF) $(MAKELCF_OPTS)  $< $(LDRES_TEMPLATE)  $@

endif

# compress/digest
ifeq	($(CODEGEN_PROC),ARM9)
ifdef	NITRO_COMPRESS
COMPSTATIC_CMDS		= -9 -c
#COMPSTATIC_CMDS	= -7 -c
endif
ifdef	NITRO_DIGEST
COMPSTATIC_CMDS		+= -a
endif
endif

ifdef	COMPSTATIC_CMDS
COMPSUFFIX		?= $(DEFAULT_COMPSUFFIX)
ifneq	($(COMPSUFFIX),)
COMPSTATIC_FLAGS	+= -e$(COMPSUFFIX)
endif
endif


# libcxx workaround
ifdef	SDK_CW_WA_LIBCXX
CW_LIBCXX	= $(CW_LIBCXX_DIR)/$(CW_LIBCXX_ARM)
$(CW_LIBCXX):	$(ROOT)/build/buildsetup/libcxx/Makefile
	$(MAKE) -f $<
endif


# .nef
ifndef	USE_MY_RULE_NEF
ifneq	($(NITRO_MAKEROM),)
$(BINDIR)/$(TARGET_BIN_BASENAME).nef: $(OBJS) $(LCFILE) $(MAKEFILE) $(LDEPENDS_NEF) $(EDEPENDS_NEF) $(ALIBRARIES) $(LDRES_FILE) $(CW_LIBCXX)
	$(LD) $(LDFLAGS) $(LIBRARY_DIRS) @$(LDRES_FILE) $(LCFILE) -o $@
ifdef	COMPSTATIC_CMDS
	$(COMPSTATIC) $(COMPSTATIC_CMDS) $(COMPSTATIC_FLAGS) -f $(BINDIR)/$(if $(NITRO_TRGNAME),$(TARGET_NAME)_)component.files $(NITRO_STDOUT)
endif
else
$(BINDIR)/$(TARGET_BIN_BASENAME).nef: $(OBJS) $(LCFILE) $(MAKEFILE) $(LDEPENDS_NEF) $(EDEPENDS_NEF) $(ALIBRARIES) $(CW_LIBCXX)
	$(LD) $(LDFLAGS) $(LIBRARY_DIRS) $(OBJS) $(LIBRARIES) $(LCFILE) -o $@
endif
endif

# .a
ifndef	USE_MY_RULE_A
$(LIBDIR)/$(TARGET_LIB): $(OBJS)
	$(AR) $(ARFLAGS) $(OBJS) -o $@
endif


#----------------------------------------------------------------------------
# Get SDK_CW_BUILD_VERSION SDK_CW_BUILD_NUMBER
#
# IRIS	V0.2 Hotfix 1
#	SDK_CW_BUILD_VERSION_CC=2.0
#	SDK_CW_BUILD_VERSION_LD=2.0
#	SDK_CW_BUILD_NUMBER_CC=0038
#	SDK_CW_BUILD_NUMBER_LD=0050
#
# NITRO V0.3
#	SDK_CW_BUILD_NUMBER_CC=0038
#	SDK_CW_BUILD_NUMBER_LD=0056
#
# NITRO V0.4.1
#	SDK_CW_BUILD_NUMBER_CC=0043
#	SDK_CW_BUILD_NUMBER_LD=0057
#
# NITRO V0.5
#	SDK_CW_BUILD_NUMBER_CC=0046
#	SDK_CW_BUILD_NUMBER_LD=0058
#
# NITRO V0.5 + HotFix1
#	SDK_CW_BUILD_NUMBER_CC=0049
#	SDK_CW_BUILD_NUMBER_LD=0061
#
# NITRO V0.6
#	SDK_CW_BUILD_NUMBER_CC=0050
#	SDK_CW_BUILD_NUMBER_LD=0062
#
# NITRO V0.6.1
#	SDK_CW_BUILD_NUMBER_CC=0050
#	SDK_CW_BUILD_NUMBER_LD=0063
#
# NITRO V0.7
#	SDK_CW_BUILD_NUMBER_CC=0050
#	SDK_CW_BUILD_NUMBER_LD=0063
#
# NITRO V1.0
#	SDK_CW_BUILD_NUMBER_CC=0056
#	SDK_CW_BUILD_NUMBER_LD=0067
#
# NITRO V1.0.2
#	SDK_CW_BUILD_NUMBER_CC=0068
#	SDK_CW_BUILD_NUMBER_LD=0068
#
# NITRO V1.2
#	SDK_CW_BUILD_NUMBER_CC=0073
#	SDK_CW_BUILD_NUMBER_LD=0072
#
# NITRO V1.2SP1
#	SDK_CW_BUILD_NUMBER_CC=0074
#	SDK_CW_BUILD_NUMBER_LD=0072
#
# NITRO V1.2SP2+patch
#	SDK_CW_BUILD_NUMBER_CC=0080
#	SDK_CW_BUILD_NUMBER_LD=0074
#
# NITRO V1.2SP3
#	SDK_CW_BUILD_NUMBER_CC=0084
#	SDK_CW_BUILD_NUMBER_LD=0077
#
# NITRO V1.2SP4
#	SDK_CW_BUILD_NUMBER_CC=0087
#	SDK_CW_BUILD_NUMBER_LD=7702
#
# NITRO V2.0beta
#	SDK_CW_BUILD_VERSION_CC=3.0
#	SDK_CW_BUILD_VERSION_LD=2.0
#	SDK_CW_BUILD_NUMBER_CC=0107
#	SDK_CW_BUILD_NUMBER_LD=0077
#
# NITRO V2.0
#	SDK_CW_BUILD_NUMBER_CC=0114
#	SDK_CW_BUILD_NUMBER_LD=0082
#
ifneq	($(findstring $(MAKECMDGOALS),do-build make-verinfo clean-verinfo),)

.PHONY: make-verinfo
make-verinfo: $(SDK_VERINFO_CW_CC) $(SDK_VERINFO_CW_LD)

$(SDK_VERINFO_CW_CC):
	@$(MWCC) -version | \
	 $(SED) -e $(QUOTE)/^Version /!d;s/\(build [0-9]\{2,3\}\) /\100 /;s/^Version \([0-9.]*\).*build \([0-9]*\).*(build \([0-9]*\)).*/SDK_CW_BUILD_VERSION_CC=\1\nSDK_CW_BUILD_NUMBER_CC=\3\nSDK_CW_BUILD_NUMBER2_CC=\2/i;$(QUOTE) >$(SDK_VERINFO_CW_CC)

$(SDK_VERINFO_CW_LD):
	@$(MWLD) -version | \
	 $(SED) -e $(QUOTE)/^Version /!d;s/\(build [0-9]\{2,3\}\) /\100 /;s/^Version \([0-9.]*\).*build \([0-9]*\).*(build \([0-9]*\)).*/SDK_CW_BUILD_VERSION_LD=\1\nSDK_CW_BUILD_NUMBER_LD=\3\nSDK_CW_BUILD_NUMBER2_LD=\2/i;$(QUOTE) >$(SDK_VERINFO_CW_LD)

endif

.PHONY: clean-verinfo
clean-verinfo:
	@$(RM) $(NITRO_BUILDTOOLSDIR)/verinfo.*


#----------------------------------------------------------------------------
# Check codegen bug around blxeq/blxne
#

.PHONY: codecheck

codecheck:
	@$(ECHO) --- checking codegen ---
	@$(NITRO_TOOLSDIR)/bin/blxchecker

#----- End of modulerules.cctype.CW -----
