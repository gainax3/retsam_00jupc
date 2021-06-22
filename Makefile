#! make -f
#----------------------------------------------------------------------------
# Project:  ポケットモンスター　ダイヤモンド＆パール
# File:     Makefile
#
# Copyright 2005 GameFreak.inc  All rights reserved.
#
#----------------------------------------------------------------------------

#export	NITRO_DEBUG=TRUE
export MAKEROM_ROMSPEED=1TROM
#export MAKEROM_ROMSPEED=MROM

# NITROSDK3.0付属ののプリコンパイルを読み込まない
NITRO_NO_STD_PCHDR = True

################################################################################
#
#
#		マクロスイッチの定義など
#
#
################################################################################
#----------------------------------------------------------------------------
#	ロム作成用定義　（ＲＯＭ提出の際は有効にしてsrl作成）
#----------------------------------------------------------------------------
#NITRO_FINALROM = yes

#----------------------------------------------------------------------------
#	バージョン定義
#----------------------------------------------------------------------------
VER_FILE	=	version

include	$(VER_FILE)

#(PM_DEBUG == yes)のときのみデバッグバージョン
ifeq	($(PM_DEBUG),yes)
MACRO_FLAGS	+=	-DPM_DEBUG
endif

ifdef	PM_VERSION
MACRO_FLAGS	+=	-DPM_VERSION=$(PM_VERSION)
endif

ifdef	PM_LANG
MACRO_FLAGS	+=	-DPM_LANG=$(PM_LANG)
endif

# -----------------------------------------------------------------------------
# localize_spec_mark(LANG_ALL) imatake 2008/07/29
# PG5_WIFIRELEASE = yes なら、Wi-Fi関係がリリースサーバに接続
ifeq	($(PG5_WIFIRELEASE),yes)
MACRO_FLAGS	+=	-DPG5_WIFIRELEASE
endif
# -----------------------------------------------------------------------------

#----------------------------------------------------------------------------
# LIBSYSCALL: Provided to every product by NINTENDO
#	システムライブラリの追加
#----------------------------------------------------------------------------
LIBSYSCALL	= platinum_rs/libsyscall.a

#----------------------------------------------------------------------------
#	インクルードファイルのディレクトリの追加
#----------------------------------------------------------------------------
LINCLUDES      = ./include/gflib \
				 ./include/library

#----------------------------------------------------------------------------
#	ライブラリディレクトリの追加
#----------------------------------------------------------------------------

#NITROWIFI_LIBSSL	 = $(if $(NITROWIFI_NOSSL),libstubsssl$(NITRO_LIBSUFFIX).a,libssl$(NITRO_LIBSUFFIX).a)
#
#NITROWIFI_LIBS_DEFAULT	 = libsoc$(NITRO_LIBSUFFIX).a		\
#			   libcps$(NITRO_LIBSUFFIX).a		\
#			   $(NITROWIFI_LIBSSL)			\
#			   libwcm$(NITRO_LIBSUFFIX).a		\
#			   libstubsmd5$(NITRO_LIBSUFFIX).a

NITROWIFI_LIBS		= 

NITROVCT_LIBS		=

NITRODWC_LIB_MODULES_   = bm account

NITRODWC_LIB_GSMODULES_ = 

LDEPENDS_NEF = $(NITRODWC_LIB_DEPENDS)

LLIBRARY_DIRS	= ./src/library/spl/$(NITRO_BUILDTYPE)

#イクニューモンコンポーネントのためのライブラリを追加 by Mori 2005.7.27
LLIBRARIES		= ./libjn_spl.a libwvr$(CODEGEN).a

# 任天堂Cryptライブラリを追加 by mituhara 2006.07.05
LLIBRARY_DIRS	+= ./src/library/crypto/$(NITRO_BUILDTYPE)
LLIBRARIES	+= ./libcrypto.a

# -----------------------------------------------------------------------------
# localize_spec_mark(LANG_ALL) imatake 2008/07/29
# PG5_WIFIRELEASE = yes なら、Wi-Fi関係がリリースサーバに接続

ifeq	($(PG5_WIFIRELEASE),yes)
LLIBRARY_DIRS	+= ./src/library/gds/$(NITRO_BUILDTYPE)
else
LLIBRARY_DIRS	+= ./src/library/gds_test/$(NITRO_BUILDTYPE)
endif

# -----------------------------------------------------------------------------

#----------------------------------------------------------------------------
#	ロムスペックファイル読み込みのため追加
#----------------------------------------------------------------------------
ROM_SPEC= platinum.rsf

#----------------------------------------------------------------------------
#		lsfファイル読み込みのため追加 by matsuda
#----------------------------------------------------------------------------
LCFILE_SPEC	=	main.lsf

MAKELCF_FLAGS	+=	-DADDRESS_MAIN=0x02000000

#ifeq	($(NITRO_PLATFORM),TEG)
#MAKELCF_FLAGS	+=	-DADDRESS_MAIN=0x02004000
#else
#MAKELCF_FLAGS	+=	-DADDRESS_MAIN=0x02000000
#endif

#----------------------------------------------------------------------------
#		makelcf.exeの引数オーバー対処のため追加 by tomoya
#----------------------------------------------------------------------------
LCFDEF_FILE = lcf_def.txt


#------------------------------------------------------#
# イクニューモンコンポーネントを追加 by Mori 2005.7.27
# VRAM内に無線ライブラリが配置されるコンポーネント定義 #
MY_COMPONENT_NAME	= ichneumon
MAKEROM_ARM7	= $(NITRO_COMPONENTSDIR)/$(MY_COMPONENT_NAME)/$(NITRO_BUILDTYPE_ARM7)/$(MY_COMPONENT_NAME)_sub$(TS_VERSION).nef
#------------------------------------------------------#

#----------------------------------------------------------------------------
#	ターゲットファイル設定
#----------------------------------------------------------------------------
#TARGET_NEF	=	main.nef	TARGET_BINに.srlを指定すると.nefも一緒に生成される
TARGET_BIN	=	main.srl


#----------------------------------------------------------------------------
#	データ・サブディレクトリへのパス指定
#----------------------------------------------------------------------------
DATABIN			= src/data/
DATASRC			= $(DATABIN)rsc

MSGDATA_DIR		= src/msgdata/
MSGDATA_FILENAME	= pl_msg.narc
MSGDATA_FILEPATH	= $(MSGDATA_DIR)$(MSGDATA_FILENAME)
DATASCENARIO		= $(MSGDATA_DIR)scenario/


SOUNDDIR		= $(DATABIN)sound/

POKEGRADIR		= src/poketool/pokegra/
TRGRADIR		= src/poketool/trgra/
#バージョンによってパーソナルデータを変更
ifeq	($(PM_VERSION),VERSION_PLATINUM)
PERSONALDIR		= src/poketool/personal/
else
PERSONALDIR		= src/poketool/personal_pearl/
endif
EVOWOTDIR		= src/poketool/personal/
TRAINERDIR		= src/poketool/trainer/
TRMSGDIR		= src/poketool/trmsg/
EFFECTDATADIR	= src/wazaeffect/effectdata/
BATTLEGRADIR	= src/battle/graphic/
WAZAEFFDIR		= src/wazaeffect/
WAZASEQDIR		= src/battle/skill/
WAZATBLDIR		= src/poketool/waza/
SCRIPTDIR		= src/fielddata/script/
EVENTDIR		= src/fielddata/eventdata/
ENCOUNTDIR		= src/fielddata/encountdata/
MAPMATRIXDIR	= src/fielddata/mapmatrix/
AREADATADIR		= src/fielddata/areadata/
AREABUILDMDLDIR	= src/fielddata/areadata/area_build_model/
AREAMAPTEXDIR	= src/fielddata/areadata/area_map_tex/
AREAMOVEMDLDIR	= src/fielddata/areadata/area_move_model/
FIELDMAPDIR		= src/fielddata/land_data/
BUILDMODELDIR	= src/fielddata/build_model/
GRAPHICDIR		= src/graphic/
ITEMDATADIR		= src/itemtool/itemdata/
POKEICONGRADIR	= src/poketool/icongra/
POKEFOOTGRADIR	= src/poketool/pokefoot/
WAZAEFFCLACTDIR	= src/wazaeffect/effectclact/
WAZAEFFCLACTRDIR= src/wazaeffect/effectclact/resource/
CONTESTGRADIR	= src/contest/graphic/
CONTESTDATADIR	= src/contest/data/
CLACTUTILDIR	= src/data/clact_util/
PARTICLEDATADIR	= src/particledata/
ZUKANDATADIR 	= src/application/zukanlist/zkn_data/
MMODELDIR		= src/data/mmodel/
WIFIEARTHDIR	= src/application/wifi_earth/
POKEANIMEDIR	= src/pokeanime/
CBDATADIR	 	= src/application/custom_ball/data/
MMLISTDIR		= src/fielddata/mm_list/
DPTITLEDIR 		= src/demo/title/
TRAIDIR 		= src/battle/tr_ai/
ARCDIR			= src/arc/
POKEZUKANDIR	= src/poketool/
POKEANMTBLDIR	= src/poketool/pokeanm/
ZONEDATADIR		= src/fielddata/maptable/
FLDTRADEDIR		= src/fielddata/pokemon_trade/
EGGDATADIR		= src/demo/egg/data/
EGGPTDIR		= src/demo/egg/data/particle/
SHINKAPTDIR		= src/demo/shinka/data/particle/
INTRODIR		= src/demo/intro/
SYOUJYOUDIR		= src/demo/syoujyou/
BTOWERDIR		= src/battle/b_tower/
PMEDITDIR		= src/poketool/poke_edit/
TESTDIR			= src/debug/cb_edit/
CBEDITDIR		= application/custom_ball/edit/
FRONTIERSCRDIR	= src/frontier/script/
FRONTIERGRADIR	= src/frontier/graphic/
BUILDMODELANMDIR   = src/fielddata/build_model_anime/
FRONTIERPARTICLEDIR	= src/particledata/pl_frontier/
FLDTEXANMDIR = src/data/trans_anime/
BALLOONGRADIR	= src/application/balloon/graphic/
WFLBY_MAPDIR	= src/application/wifi_lobby/map_conv/
TWORLDDIR		= src/fielddata/tornworld/
POKELISTPARTICLEDIR	= src/particledata/pl_pokelist/
PL_STAGE_PM_DIR	= src/battle/b_pl_stage/
PL_TOWER_PM_DIR	= src/battle/b_pl_tower/
BUCKET_DIR	= src/application/bucket/
PL_ETC_PARTICLEDIR	= src/particledata/pl_etc/

#----------------------------------------------------------------------------
#	コンバート対象になるMayaデータの指定
#----------------------------------------------------------------------------
include	make_g3_files

G3D_NSBMD = $(G3D_IMD:.imd=.nsbmd)
G3D_NSBMT = $(G3D_IMT:.imd=.nsbmt)
#G3D_NSBMTG = $(G3D_GROUND:.imd=.nsbmt)
G3D_NSBTX = $(G3D_TX_IMD:.imd=.nsbtx)
G3D_NSBCA = $(G3D_ICA:.ica=.nsbca)
G3D_NSBVA = $(G3D_IVA:.iva=.nsbva)
G3D_NSBMA = $(G3D_IMA:.ima=.nsbma)
G3D_NSBTP = $(G3D_ITP:.itp=.nsbtp)
G3D_NSBTA = $(G3D_ITA:.ita=.nsbta)

G3D_DATA = $(G3D_NSBMD) $(G3D_NSBTX) $(G3D_NSBCA) $(G3D_NSBVA) $(G3D_NSBMA) $(G3D_NSBTP) $(G3D_NSBTA) $(G3D_NSBMT) #$(G3D_NSBMTG)

G3D_TARGETS	=$(subst rsc/,,$(G3D_DATA))


#----------------------------------------------------------------------------
#	makeサブディレクトリ指定
#
#	SUBDIRSの定義を行っている
#----------------------------------------------------------------------------
include sub_make_files


#----------------------------------------------------------------------------
#	コンパイル対象ソースファイル指定
#
#	SRCSとSRCS_OVERLAYFILESの定義を行っている
#----------------------------------------------------------------------------
include	make_prog_files
-include overlay_files
SRCS := $(subst \,/,$(SRCS))
SRCS_OVERLAY := $(subst \,/,$(SRCS_OVERLAY))

#----------------------------------------------------------------------------
#	ロムに含まれるファイルの指定
#----------------------------------------------------------------------------
FILE_TREE	= $(DATABIN)*.* \
			  $(SOUNDDIR)sound_data.sdat \
			  $(SOUNDDIR)pl_sound_data.sdat \
			  $(MSGDATA_FILEPATH) \
			  $(MSGDATA_DIR)msg.narc \
			  $(DATASCENARIO)*.narc \
			  $(POKEGRADIR)*.narc \
			  $(TRGRADIR)*.narc \
			  $(PERSONALDIR)personal.narc \
			  $(PERSONALDIR)pl_personal.narc \
			  $(EVOWOTDIR)evo.narc \
			  $(EVOWOTDIR)growtbl.narc \
			  $(EVOWOTDIR)pl_growtbl.narc \
			  $(EVOWOTDIR)pms.narc \
			  $(EVOWOTDIR)wotbl.narc \
			  $(TRAINERDIR)*.narc \
			  $(TRMSGDIR)*.narc \
			  $(BATTLEGRADIR)*.narc \
			  $(WAZAEFFDIR)we.arc \
			  $(WAZAEFFDIR)we_sub.narc \
			  $(WAZASEQDIR)*.narc \
			  $(WAZAEFFCLACTDIR)*.narc \
			  $(WAZATBLDIR)*.narc \
			  $(SCRIPTDIR)*.narc \
			  $(GRAPHICDIR)*.narc \
			  $(ITEMDATADIR)*.narc \
			  $(POKEICONGRADIR)*.narc \
			  $(POKEFOOTGRADIR)pokefoot.narc \
			  $(MMODELDIR)mmodel.narc \
			  $(MMODELDIR)fldeff.narc \
			  $(EFFECTDATADIR)*.narc \
			  $(EVENTDIR)*.narc \
			  $(ENCOUNTDIR)d_enc_data.narc \
			  $(ENCOUNTDIR)p_enc_data.narc \
			  $(ENCOUNTDIR)pl_enc_data.narc \
			  $(BUILDMODELDIR)*.narc \
			  $(BUILDMODELDIR)*.dat \
			  $(MAPMATRIXDIR)*.narc \
			  $(AREADATADIR)*.narc \
			  $(AREABUILDMDLDIR)*.narc \
			  $(AREAMAPTEXDIR)*.narc \
			  $(AREAMOVEMDLDIR)*.narc \
			  $(FIELDMAPDIR)*.narc \
			  $(CONTESTGRADIR)*.narc \
			  $(CONTESTDATADIR)*.narc \
			  $(PARTICLEDATADIR)*.narc \
			  $(ZUKANDATADIR)zukan_data.narc \
			  $(ZUKANDATADIR)zukan_data_gira.narc \
			  $(WIFIEARTHDIR)*.narc \
			  $(DPTITLEDIR)titledemo.narc \
			  $(CBDATADIR)cb_data.narc \
			  $(EGGDATADIR)egg_data.narc \
			  $(EGGPTDIR)egg_demo_particle.narc \
			  $(MMLISTDIR)move_model_list.narc \
			  $(POKEANIMEDIR)poke_anm.narc \
			  $(POKEANIMEDIR)pl_poke_anm.narc \
			  $(TRAIDIR)tr_ai_seq.narc \
			  $(ARCDIR)*.narc \
			  $(ZONEDATADIR)mapname.bin \
			  $(POKEZUKANDIR)pokezukan.narc \
			  $(POKEZUKANDIR)pl_pokezukan.narc \
			  $(POKEZUKANDIR)shinzukan.narc \
			  $(POKEANMTBLDIR)pokeanm.narc \
			  $(POKEANMTBLDIR)pl_pokeanm.narc \
			  $(FLDTRADEDIR)fld_trade.narc \
			  $(SHINKAPTDIR)shinka_demo_particle.narc \
			  $(INTRODIR)*.narc \
			  $(DPTITLEDIR)op_demo.narc \
			  $(SYOUJYOUDIR)syoujyou.narc \
			  $(BTOWERDIR)*.narc \
			  $(ZUKANDATADIR)zukan_enc_diamond.narc \
			  $(ZUKANDATADIR)zukan_enc_pearl.narc	\
			  $(ZUKANDATADIR)zukan_enc_platinum.narc	\
			  $(PMEDITDIR)pl_poke_data.narc \
			  $(TESTDIR)d_test.narc \
			  $(CBEDITDIR)pl_cb_data.narc \
			  $(FRONTIERSCRDIR)fr_script.narc \
			  $(FRONTIERGRADIR)frontier_bg.narc \
			  $(FRONTIERGRADIR)frontier_obj.narc \
			  $(FRONTIERPARTICLEDIR)*.narc \
			  $(BALLOONGRADIR)*.narc \
			  $(WFLBY_MAPDIR)*.narc \
              $(TWORLDDIR)tw_arc.narc \
              $(TWORLDDIR)tw_arc_attr.narc \
			  $(POKELISTPARTICLEDIR)*.narc \
              $(PL_STAGE_PM_DIR)*.narc \
              $(PL_TOWER_PM_DIR)*.narc \
              $(BUCKET_DIR)*.narc \
              $(PL_ETC_PARTICLEDIR)*.narc
			  
#ロムファイルシステムのルートディレクトリ指定
MAKEROM_ROMROOT  = ./src/

MAKEROM_ROMFILES = $(subst src/,,$(FILE_TREE))

#----------------------------------------------------------------------------
#	make cleanで消去するファイルを追加
#----------------------------------------------------------------------------
LDIRT_CLEAN	= $(G3D_TARGETS) $(MSGDATA_FILEPATH) \
			  $(wildcard $(DATABIN)*.nsbmt) $(wildcard $(DATABIN)*.nsbmd) \
			  $(wildcard $(DATABIN)*.nsbta) $(wildcard $(DATABIN)*.nsbtp) \
			  $(wildcard $(DATABIN)*.nsbtx) $(wildcard $(DATABIN)*.nsbma)


#----------------------------------------------------------------------------
#	ツールへのパス指定
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
#	共通変数定義
#----------------------------------------------------------------------------
include commondefs.GF

#----------------------------------------------------------------------------
#	コンパイルルール定義
#----------------------------------------------------------------------------
include modulerules.GF


#----------------------------------------------------------------------------
#	WiFi定義
#----------------------------------------------------------------------------
#include	$(NITRODWC_ROOT)/build/buildtools/commondefs
#include	$(NITRODWC_ROOT)/build/buildtools/modulerules
include	$(NITROVCT_ROOT)/build/buildtools/commondefs
include	$(NITROVCT_ROOT)/build/buildtools/modulerules

################################################################################
#
#
#		コンパイルルール定義
#
#
################################################################################

do-build: precompile binData msgData pbrdiff $(LCFILE_SPEC) $(TARGETS) 

binData: $(G3D_TARGETS)


#----------------------------------------------------------------------------
# precompile
#----------------------------------------------------------------------------
PRECOMPILE_SRC		 =	include/precompile/precompile.pch
PRECOMPILE_CC_MCH	 =	$(OBJDIR)/precompile_cc.mch
PRECOMPILE_CCFLAGS	:=	$(CCFLAGS)
CCFLAGS			+=	-prefix $(PRECOMPILE_CC_MCH)
ifeq	($(USERNAME),ohno)
CCFLAGS			+=	-msgstyle gcc
endif
#DEPENDDIR		 =	$(OBJDIR)

precompile: $(PRECOMPILE_CC_MCH)

#バージョン定義ファイルが更新されたらプリコンパイルヘッダも更新する
$(PRECOMPILE_CC_MCH):	$(VER_FILE)

-include $(PRECOMPILE_CC_MCH).d
$(PRECOMPILE_CC_MCH):%_cc.mch:
		$(CC) $(PRECOMPILE_CCFLAGS) $(INCLUDES) $(PRECOMPILE_SRC) -MD -precompile $(PRECOMPILE_CC_MCH)
		$(MOVE_SRC_DEPEND)
		$(MV) $(DEPENDDIR)/$(*F).d $@.d
$(OBJS):  $(PRECOMPILE_CC_MCH)

CCFLAGS +=	-thumb
#----------------------------------------------------------------------------
# msgdata
#----------------------------------------------------------------------------
MSG_NAIX_COPYSCRIPT	= convert/message/naixcopy.pl
MSG_PLNAIX_CHANGESCRIPT	= convert/message/pl_naix_change.pl
MSG_CONVERTDATA_DIR	= convert/message/data/

MSG_HEADERFILE_DIR	= include/msgdata/

msgData: $(MSGDATA_FILEPATH)

$(MSGDATA_FILEPATH): $(wildcard $(MSG_CONVERTDATA_DIR)*.dat)
	nnsarc -c -l -n -i $(MSGDATA_FILENAME) $(MSG_CONVERTDATA_DIR)*.dat
	$(MV) $(MSGDATA_FILENAME) $(MSGDATA_FILEPATH)
	perl $(MSG_PLNAIX_CHANGESCRIPT) pl_msg.naix
	perl $(MSG_NAIX_COPYSCRIPT)  pl_msg.naix $(MSG_HEADERFILE_DIR)msg.naix
	-$(RM) msg.naix
	-$(RM) pl_msg.naix



#----------------------------------------------------------------------------
#	makefileがインクルードするファイルが更新されたらmakelcfが動作するようにした
#----------------------------------------------------------------------------
$(LCFILE_SPEC):	commondefs.GF modulerules.GF make_prog_files make_g3_files overlay_files makefile
	-$(RM) $(BINDIR)/$(LCFILE_SPEC:.lsf=.*)
	-rm $(LCFDEF_FILE)
	make $(LCFDEF_FILE)
	make do-build

main.lsf: overlay_files
overlay_files: overlaytool.rb
	ruby overlaytool.rb

#overlaytool.rb実行後のoverlay_filesを新たにincludeしてlcf_def.txtを作成する
#ために、LCFDEF_FILEだけ作成する文を作成
$(LCFDEF_FILE):
	echo OBJS_STATIC=$(OBJS_STATIC) > $(LCFDEF_FILE)
	echo OBJS_AUTOLOAD=$(OBJS_AUTOLOAD) >> $(LCFDEF_FILE)
	echo OBJS_OVERLAY=$(OBJS_OVERLAY) >> $(LCFDEF_FILE)

#----------------------------------------------------------------------------
#	ソースの類が１個でもbuilddate.cより新しい場合builddate.cをtouch&コンパイル
#----------------------------------------------------------------------------
./src/system/builddate.c: $(SRCS) $(SRCS_OVERLAY)
	touch ./src/system/builddate.c


#----------------------------------------------------------------------------
#	セルアクターヘッダー内のアーカイブファイルIDと
#	arc_tool.dat内のアーカイブファイルIDが統一されているかをチェックする
#	perlを実行する
#----------------------------------------------------------------------------
#clact_arcfile_check:
#	perl src/data/clact_util/clact_check.pl

#----------------------------------------------------------------------------
#	強制コンパイル
#
#	builddate.cをtouchしてコンパイルしている
#----------------------------------------------------------------------------
force:
	touch ./src/system/builddate.c
	$(MAKE)

#----------------------------------------------------------------------------
#	ソースのみのリコンパイル
#----------------------------------------------------------------------------
source:
	-$(RM) $(GDIRT_CLEAN)
	-$(RM) $(DEPENDDIR)/*.d
	$(MAKE)

#----------------------------------------------------------------------------
#	このMakefile管轄下のモデリングデータリコンパイル
#----------------------------------------------------------------------------
model:
	-$(RM)	$(G3D_TARGETS) \
		  	$(wildcard $(DATABIN)*.nsbmt) $(wildcard $(DATABIN)*.nsbmd) \
			$(wildcard $(DATABIN)*.nsbta) $(wildcard $(DATABIN)*.nsbtp) \
			$(wildcard $(DATABIN)*.nsbtx) $(wildcard $(DATABIN)*.nsbma)
	-$(RM)	$(wildcard $(FIELDMAPDIR)/land_nsbmt/*.nsbmt) \
			$(FIELDMAPDIR)land_data.narc
	$(MAKE) model -C $(BUILDMODELDIR)

#----------------------------------------------------------------------------
#	地形データのモデリングデータリコンパイル
#----------------------------------------------------------------------------
land:
	-$(RM)	$(wildcard $(FIELDMAPDIR)/land_nsbmt/*.nsbmt) \
			$(FIELDMAPDIR)land_data.narc
	$(MAKE) imdconv -C $(FIELDMAPDIR)
	$(MAKE) force

landall:
	-$(RM)	$(wildcard $(FIELDMAPDIR)/land_nsbmt/*.nsbmt) \
			$(FIELDMAPDIR)land_data.narc
	-$(MAKE) imdconv -C $(FIELDMAPDIR)
	-$(MAKE) ctex

#----------------------------------------------------------------------------
#	Buildモデルのみのリコンパイル
#----------------------------------------------------------------------------
#src/fielddata/build_model/make all
#bmall:
#	$(MAKE) -C $(BUILDMODELDIR) clean all
#	$(MAKE) force
bmall:
	$(MAKE) -C $(BUILDMODELDIR) clean
	$(MAKE) -C $(BUILDMODELDIR)
	$(MAKE) -C $(BUILDMODELANMDIR) clean
	$(MAKE) -C $(BUILDMODELANMDIR)
	$(MAKE) -C $(AREABUILDMDLDIR) clean
	$(MAKE) -C $(AREABUILDMDLDIR)
	$(MAKE) force

#src/fielddata/build_model/make model 
bimd:
	$(MAKE) model -C $(BUILDMODELDIR)
	$(MAKE) force

#src/fielddata/build_model/make 
bm:
	$(MAKE) -C $(BUILDMODELDIR)
	$(MAKE) force

#src/fielddata/areadata/area_build_model/make all
abmall:
	$(MAKE) -C $(AREABUILDMDLDIR) clean all
	$(MAKE) force

#src/fielddata/areadata/area_build_model/make
abm:
	$(MAKE) -C $(AREABUILDMDLDIR)
	$(MAKE) force

#src/data/clact_util/make
clact:
	$(MAKE) -C $(CLACTUTILDIR)
	$(MAKE) force

#src/data/clact_util/make clean
clact_clean:
	$(MAKE) -C $(CLACTUTILDIR) clean

#src/fielddata/build_model/make
#src/fielddata/build_model_anime/
bmanm:
	$(MAKE) -C $(BUILDMODELDIR)
	$(MAKE) -C $(BUILDMODELANMDIR)
	$(MAKE) -C $(AREABUILDMDLDIR)
	$(MAKE) force

#----------------------------------------------------------------------------
# 	地形チップテクスチャリコンパイル	
#----------------------------------------------------------------------------
ctex:
	$(MAKE) -C $(AREAMAPTEXDIR) clean all
	$(MAKE) force

areadata:
	$(MAKE) -C $(AREADATADIR) clean
	$(MAKE) -C $(AREADATADIR)
	$(MAKE) force
	
#----------------------------------------------------------------------------
# 	地形モデルテクスチャ転送アニメリコンパイル
#----------------------------------------------------------------------------
ftexanm:
	$(MAKE) -C $(FLDTEXANMDIR) clean
	$(MAKE) -C $(FLDTEXANMDIR)
	$(MAKE) force

#----------------------------------------------------------------------------
#	動作モデル＆動作モデルエフェクトリコンパイル
#----------------------------------------------------------------------------
fldobj:
	$(MAKE) -C $(MMODELDIR) clean all
	$(MAKE) -C $(MMODELDIR)
	$(MAKE) force

#----------------------------------------------------------------------------
#	やぶれたせかいマップデータコンパイル
#----------------------------------------------------------------------------
tworld:
	$(MAKE) -C $(TWORLDDIR)
	$(MAKE) force
	
tworldall:
	$(MAKE) -C $(TWORLDDIR) clean all
	$(MAKE) force
	
#----------------------------------------------------------------------------
#	バイナリ比較
#----------------------------------------------------------------------------
diff:
	cp ./bin/ARM9-TS/Release/main.srl ./bin/ARM9-TS/Release/old.srl
	$(MAKE) do-build
	$(MAKE) onlydiff

onlydiff:
	fc /b .\\bin\\ARM9-TS\\Release\\main.srl .\\bin\\ARM9-TS\\Release\\old.srl

#一時コマンド04/04 iwasawa 後で消します
del_land_list:
	$(RM) $(FIELDMAPDIR)land_list

#----------------------------------------------------------------------------
#	PBR用保持ファイルとのバイナリ比較	2007.01.24(水) matsuda
#----------------------------------------------------------------------------
PBRDIR = pbr\\

pbrdiff:
	@ cmp $(PBRDIR)b_plist_gra.narc src\\battle\\graphic\\b_plist_gra.narc
	@ cmp $(PBRDIR)bag_gra.narc src\\graphic\\bag_gra.narc
	@ cmp $(PBRDIR)batt_bg.narc src\\battle\\graphic\\batt_bg.narc
	@ cmp $(PBRDIR)batt_obj.narc src\\battle\\graphic\\batt_obj.narc
	@ cmp $(PBRDIR)font.narc src\\graphic\\font.narc
	@ cmp $(PBRDIR)growtbl.narc src\\poketool\\personal\\growtbl.narc
	@ cmp $(PBRDIR)item_data.narc src\\itemtool\\itemdata\\item_data.narc
	@ cmp $(PBRDIR)msg.narc src\\msgdata\\msg.narc
	@ cmp $(PBRDIR)otherpoke.narc src\\poketool\\pokegra\\otherpoke.narc
	@ cmp $(PBRDIR)personal.narc src\\poketool\\personal\\personal.narc
	@ cmp $(PBRDIR)plist_gra.narc src\\graphic\\plist_gra.narc
	@ cmp $(PBRDIR)poke_anm.narc src\\pokeanime\\poke_anm.narc
	@ cmp $(PBRDIR)poke_icon.narc src\\poketool\\icongra\\poke_icon.narc
	@ cmp $(PBRDIR)pokeanm.narc src\\poketool\\pokeanm\\pokeanm.narc
	@ cmp $(PBRDIR)pokegra.narc src\\poketool\\pokegra\\pokegra.narc
	@ cmp $(PBRDIR)pokezukan.narc src\\poketool\\pokezukan.narc
	@ cmp $(PBRDIR)pst_gra.narc src\\graphic\\pst_gra.narc
	@ cmp $(PBRDIR)sound_data.sdat src\\data\\sound\\sound_data.sdat
	@ cmp $(PBRDIR)waza_tbl.narc src\\poketool\\waza\\waza_tbl.narc
	@ cmp $(PBRDIR)wifi.ncgr src\\data\\wifi.ncgr
	@ cmp $(PBRDIR)winframe.narc src\\graphic\\winframe.narc
	@ cmp $(PBRDIR)wm.ncgr src\\data\\wm.ncgr
	@ cmp $(PBRDIR)wm.nclr src\\data\\wm.nclr

