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
#LLIBRARIES	+= ./libcrypto.a

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
FILE_TREE	= ./data/UTF16.dat \
./data/arealight.narc \
./data/battle_win.NSCR \
./data/btower_canm.resdat \
./data/btower_celact.cldat \
./data/btower_cell.resdat \
./data/btower_chr.resdat \
./data/btower_pal.resdat \
./data/cell0.NCGR \
./data/cell0.NCLR \
./data/clact_default.NANR \
./data/crystal.nsbmd \
./data/demo_climax.narc \
./data/dp_areawindow.NCGR \
./data/dp_areawindow.NCLR \
./data/dt_test_celact.txt \
./data/dt_test_res_cell.txt \
./data/dt_test_res_cellanm.txt \
./data/dt_test_res_char.txt \
./data/dt_test_res_multi.txt \
./data/dt_test_res_multianm.txt \
./data/dt_test_res_pltt.txt \
./data/dun_sea.nsbtx \
./data/eoo.dat \
./data/exdata.dat \
./data/field_cutin.narc \
./data/fld_anime0.bin \
./data/fld_anime1.bin \
./data/fld_anime10.bin \
./data/fld_anime2.bin \
./data/fld_anime3.bin \
./data/fld_anime4.bin \
./data/fld_anime5.bin \
./data/fld_anime6.bin \
./data/fld_anime7.bin \
./data/fld_anime8.bin \
./data/fld_anime9.bin \
./data/fldtanime.narc \
./data/fs_kanban.nsbca \
./data/ground0.NCGR \
./data/ground0.NCLR \
./data/ground0.NSCR \
./data/guru2.narc \
./data/kemu_itpconv.dat \
./data/lake_anim.nsbtx \
./data/miniasahamabe.nsbtx \
./data/miniasasea.nsbtx \
./data/minihamabe.nsbtx \
./data/minimum.nsbtx \
./data/minirhana.nsbtx \
./data/namein.narc \
./data/nfont.NCGR \
./data/nfont.NCLR \
./data/pc.nsbca \
./data/pl_wifi.ncgr \
./data/pl_wm.ncgr \
./data/pl_wm.nclr \
./data/plbr.dat \
./data/plist_canm.resdat \
./data/plist_cell.resdat \
./data/plist_chr.resdat \
./data/plist_h.cldat \
./data/plist_pal.resdat \
./data/porucase_canm.resdat \
./data/porucase_celact.cldat \
./data/porucase_cell.resdat \
./data/porucase_chr.resdat \
./data/porucase_pal.resdat \
./data/pst_canm.resdat \
./data/pst_cell.resdat \
./data/pst_chr.resdat \
./data/pst_h.cldat \
./data/pst_pal.resdat \
./data/shop_canm.resdat \
./data/shop_cell.resdat \
./data/shop_chr.resdat \
./data/shop_h.cldat \
./data/shop_pal.resdat \
./data/slot.narc \
./data/smptm_koori.NANR \
./data/smptm_koori.NCER \
./data/smptm_koori.NCGR \
./data/smptm_koori.NCLR \
./data/smptm_nemuri.NANR \
./data/smptm_nemuri.NCER \
./data/smptm_nemuri.NCGR \
./data/smptm_nemuri.NCLR \
./data/str2uni.bin \
./data/t3_fl_b.nsbtx \
./data/t3_fl_p.nsbtx \
./data/t3_fl_r.nsbtx \
./data/t3_fl_y.nsbtx \
./data/test.atr \
./data/tmap_block.dat \
./data/tmap_flags.dat \
./data/tmapn_canm.resdat \
./data/tmapn_celact.cldat \
./data/tmapn_celact.txt \
./data/tmapn_cell.resdat \
./data/tmapn_chr.resdat \
./data/tmapn_pal.resdat \
./data/tmapn_res_canm.txt \
./data/tmapn_res_cell.txt \
./data/tmapn_res_chr.txt \
./data/tmapn_res_pal.txt \
./data/tradelist.narc \
./data/trapmark.narc \
./data/tw_arc_etc.naix \
./data/tw_arc_etc.narc \
./data/ug_anim.narc \
./data/ug_base_cur.nsbmd \
./data/ug_boygirl.NCGR \
./data/ug_boygirl.NCLR \
./data/ug_fossil.narc \
./data/ug_hero.NANR \
./data/ug_hero.NCER \
./data/ug_hole.NANR \
./data/ug_hole.NCER \
./data/ug_hole.NCGR \
./data/ug_parts.narc \
./data/ug_radar.narc \
./data/ug_trap.narc \
./data/ugeffect_obj_graphic.narc \
./data/uground_cell.resdat \
./data/uground_cellanm.resdat \
./data/uground_char.resdat \
./data/uground_char2.resdat \
./data/uground_clact.cldat \
./data/uground_pltt.resdat \
./data/uground_pltt2.resdat \
./data/ugroundeffect.naix \
./data/ugroundeffect.narc \
./data/underg_radar.narc \
./data/utility.bin \
./data/weather_sys.narc \
./data/wifi.ncgr \
./data/wifip2pmatch.narc \
./data/wm.ncgr \
./data/wm.nclr \
./data/sound/sound_data.sdat \
./data/sound/pl_sound_data.sdat \
./msgdata/pl_msg.narc \
./msgdata/msg.narc \
./msgdata/scenario/scr_msg.narc \
./poketool/pokegra/dp_height.narc \
./poketool/pokegra/dp_height_o.narc \
./poketool/pokegra/height.narc \
./poketool/pokegra/height_o.narc \
./poketool/pokegra/otherpoke.narc \
./poketool/pokegra/pl_otherpoke.narc \
./poketool/pokegra/pl_pokegra.narc \
./poketool/pokegra/poke_shadow.narc \
./poketool/pokegra/poke_shadow_ofx.narc \
./poketool/pokegra/poke_yofs.narc \
./poketool/pokegra/pokegra.narc \
./poketool/trgra/trbgra.narc \
./poketool/trgra/trfgra.narc \
./poketool/personal/personal.narc \
./poketool/personal/pl_personal.narc \
./poketool/personal/evo.narc \
./poketool/personal/growtbl.narc \
./poketool/personal/pl_growtbl.narc \
./poketool/personal/pms.narc \
./poketool/personal/wotbl.narc \
./poketool/trainer/trdata.narc \
./poketool/trainer/trpoke.narc \
./poketool/trmsg/trtbl.narc \
./poketool/trmsg/trtblofs.narc \
./battle/graphic/b_bag_gra.narc \
./battle/graphic/b_plist_gra.narc \
./battle/graphic/batt_bg.narc \
./battle/graphic/batt_obj.narc \
./battle/graphic/pl_b_plist_gra.narc \
./battle/graphic/pl_batt_bg.narc \
./battle/graphic/pl_batt_obj.narc \
./battle/graphic/vs_demo_gra.narc \
./wazaeffect/we.arc \
./wazaeffect/we_sub.narc \
./battle/skill/be_seq.narc \
./battle/skill/sub_seq.narc \
./battle/skill/waza_seq.narc \
./wazaeffect/effectclact/wecell.narc \
./wazaeffect/effectclact/wecellanm.narc \
./wazaeffect/effectclact/wechar.narc \
./wazaeffect/effectclact/wepltt.narc \
./poketool/waza/pl_waza_tbl.narc \
./poketool/waza/waza_tbl.narc \
./fielddata/script/scr_seq.narc \
./graphic/bag_gra.narc \
./graphic/box.narc \
./graphic/btower.narc \
./graphic/bucket.narc \
./graphic/config_gra.narc \
./graphic/demo_trade.narc \
./graphic/dendou_demo.narc \
./graphic/dendou_pc.narc \
./graphic/ending.narc \
./graphic/ev_pokeselect.narc \
./graphic/f_note_gra.narc \
./graphic/field_board.narc \
./graphic/field_encounteffect.narc \
./graphic/fld_comact.narc \
./graphic/font.narc \
./graphic/fontoam.narc \
./graphic/footprint_board.narc \
./graphic/hiden_effect.narc \
./graphic/imageclip.narc \
./graphic/library_tv.narc \
./graphic/lobby_news.narc \
./graphic/mail_gra.narc \
./graphic/menu_gra.narc \
./graphic/mysign.narc \
./graphic/mystery.narc \
./graphic/ntag_gra.narc \
./graphic/nutmixer.narc \
./graphic/oekaki.narc \
./graphic/opening.narc \
./graphic/pl_bag_gra.narc \
./graphic/pl_font.narc \
./graphic/pl_plist_gra.narc \
./graphic/pl_pst_gra.narc \
./graphic/pl_wifinote.narc \
./graphic/pl_winframe.narc \
./graphic/plist_gra.narc \
./graphic/pmsi.narc \
./graphic/poketch.narc \
./graphic/poru_gra.narc \
./graphic/poruact.narc \
./graphic/porudemo.narc \
./graphic/pst_gra.narc \
./graphic/ranking.narc \
./graphic/record.narc \
./graphic/shop_gra.narc \
./graphic/tmap_gra.narc \
./graphic/touch_subwindow.narc \
./graphic/trainer_case.narc \
./graphic/unionobj2d_onlyfront.narc \
./graphic/unionroom.narc \
./graphic/waza_oshie_gra.narc \
./graphic/wifi2dchar.narc \
./graphic/wifi_lobby.narc \
./graphic/wifi_lobby_other.narc \
./graphic/wifi_unionobj.narc \
./graphic/winframe.narc \
./graphic/worldtimer.narc \
./graphic/worldtrade.narc \
./itemtool/itemdata/item_data.narc \
./itemtool/itemdata/item_icon.narc \
./itemtool/itemdata/nuts_data.narc \
./itemtool/itemdata/pl_item_data.narc \
./poketool/icongra/pl_poke_icon.narc \
./poketool/icongra/poke_icon.narc \
./poketool/pokefoot/pokefoot.narc \
./data/mmodel/mmodel.narc \
./data/mmodel/fldeff.narc \
./wazaeffect/effectdata/ball_particle.narc \
./wazaeffect/effectdata/waza_particle.narc \
./fielddata/eventdata/zone_event.narc \
./fielddata/encountdata/d_enc_data.narc \
./fielddata/encountdata/p_enc_data.narc \
./fielddata/encountdata/pl_enc_data.narc \
./fielddata/build_model/build_model.narc \
./fielddata/build_model/build_model_matshp.dat \
./fielddata/mapmatrix/map_matrix.narc \
./fielddata/areadata/area_data.narc \
./fielddata/areadata/area_build_model/area_build.narc \
./fielddata/areadata/area_build_model/areabm_texset.narc \
./fielddata/areadata/area_map_tex/map_tex_set.narc \
./fielddata/areadata/area_move_model/move_model_list.narc \
./fielddata/land_data/land_data.narc \
./contest/data/contest_data.narc \
./particledata/particledata.narc \
./application/zukanlist/zkn_data/zukan_data.narc \
./application/zukanlist/zkn_data/zukan_data_gira.narc \
./application/wifi_earth/wifi_earth.narc \
./application/wifi_earth/wifi_earth_place.narc \
./application/custom_ball/data/cb_data.narc \
./demo/egg/data/egg_data.narc \
./demo/egg/data/particle/egg_demo_particle.narc \
./fielddata/mm_list/move_model_list.narc \
./pokeanime/poke_anm.narc \
./pokeanime/pl_poke_anm.narc \
./battle/tr_ai/tr_ai_seq.narc \
./arc/area_win_gra.narc \
./arc/balance_ball_gra.narc \
./arc/bm_anime.narc \
./arc/bm_anime_list.narc \
./arc/codein_gra.narc \
./arc/demo_tengan_gra.narc \
./arc/email_gra.narc \
./arc/encdata_ex.narc \
./arc/manene.narc \
./arc/plgym_ghost.narc \
./arc/ppark.narc \
./arc/ship_demo.narc \
./arc/ship_demo_pl.narc \
./arc/tv.narc \
./fielddata/maptable/mapname.bin \
./poketool/pokezukan.narc \
./poketool/pl_pokezukan.narc \
./poketool/shinzukan.narc \
./poketool/pokeanm/pokeanm.narc \
./poketool/pokeanm/pl_pokeanm.narc \
./demo/shinka/data/particle/shinka_demo_particle.narc \
./demo/intro/intro.narc \
./demo/intro/intro_tv.narc \
./demo/syoujyou/syoujyou.narc \
./battle/b_tower/btdpm.narc \
./battle/b_tower/btdtr.narc \
./application/zukanlist/zkn_data/zukan_enc_diamond.narc \
./application/zukanlist/zkn_data/zukan_enc_pearl.narc \
./application/zukanlist/zkn_data/zukan_enc_platinum.narc \
./poketool/poke_edit/pl_poke_data.narc \
./debug/cb_edit/d_test.narc \
./application/custom_ball/edit/pl_cb_data.narc \
./frontier/script/fr_script.narc \
./particledata/pl_frontier/frontier_particle.narc \
./application/balloon/graphic/balloon_gra.narc \
./application/wifi_lobby/map_conv/wflby_map.narc \
./fielddata/tornworld/tw_arc.narc \
./fielddata/tornworld/tw_arc_attr.narc \
./particledata/pl_pokelist/pokelist_particle.narc \
./battle/b_pl_stage/pl_bsdpm.narc \
./battle/b_pl_tower/pl_btdpm.narc \
./battle/b_pl_tower/pl_btdtr.narc \
./application/bucket/ballslow_data.narc \
./particledata/pl_etc/pl_etc_particle.narc \
./contest/graphic/contest_bg.narc \
./contest/graphic/contest_obj.narc \
./demo/title/op_demo.narc \
./demo/title/titledemo.narc \
./fielddata/pokemon_trade/fld_trade.narc \
./resource/eng/zukan/zukan.narc \
./resource/eng/batt_rec/batt_rec_gra.narc \
./resource/eng/wifi_lobby_minigame/wlmngm_tool.narc \
./resource/eng/frontier_graphic/frontier_bg.narc \
./resource/eng/frontier_graphic/frontier_obj.narc \
./resource/eng/scratch/scratch.narc \
./resource/eng/pms_aikotoba/pms_aikotoba.narc




#ロムファイルシステムのルートディレクトリ指定
MAKEROM_ROMROOT  = ./files/

MAKEROM_ROMFILES = $(FILE_TREE)

#----------------------------------------------------------------------------
#	make cleanで消去するファイルを追加
#----------------------------------------------------------------------------
#LDIRT_CLEAN	= $(G3D_TARGETS) $(MSGDATA_FILEPATH) \
#			  $(wildcard $(DATABIN)*.nsbmt) $(wildcard $(DATABIN)*.nsbmd) \
#			  $(wildcard $(DATABIN)*.nsbta) $(wildcard $(DATABIN)*.nsbtp) \
#			  $(wildcard $(DATABIN)*.nsbtx) $(wildcard $(DATABIN)*.nsbma)
LDIRT_CLEAN :=

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

#do-build: precompile binData msgData pbrdiff $(LCFILE_SPEC) $(TARGETS) 
do-build: precompile pbrdiff $(LCFILE_SPEC) $(TARGETS) 

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

tidy:
	-$(RM) $(GDIRT_CLEAN)
	-$(RM) $(DEPENDDIR)/*.d

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
PBRDIR = pbr/

pbrdiff:
	@ cmp $(PBRDIR)b_plist_gra.narc src/battle/graphic/b_plist_gra.narc
	@ cmp $(PBRDIR)bag_gra.narc src/graphic/bag_gra.narc
	@ cmp $(PBRDIR)batt_bg.narc src/battle/graphic/batt_bg.narc
	@ cmp $(PBRDIR)batt_obj.narc src/battle/graphic/batt_obj.narc
	@ cmp $(PBRDIR)font.narc src/graphic/font.narc
	@ cmp $(PBRDIR)growtbl.narc src/poketool/personal/growtbl.narc
	@ cmp $(PBRDIR)item_data.narc src/itemtool/itemdata/item_data.narc
	@ cmp $(PBRDIR)msg.narc src/msgdata/msg.narc
	@ cmp $(PBRDIR)otherpoke.narc src/poketool/pokegra/otherpoke.narc
	@ cmp $(PBRDIR)personal.narc src/poketool/personal/personal.narc
	@ cmp $(PBRDIR)plist_gra.narc src/graphic/plist_gra.narc
	@ cmp $(PBRDIR)poke_anm.narc src/pokeanime/poke_anm.narc
	@ cmp $(PBRDIR)poke_icon.narc src/poketool/icongra/poke_icon.narc
	@ cmp $(PBRDIR)pokeanm.narc src/poketool/pokeanm/pokeanm.narc
	@ cmp $(PBRDIR)pokegra.narc src/poketool/pokegra/pokegra.narc
	@ cmp $(PBRDIR)pokezukan.narc src/poketool/pokezukan.narc
	@ cmp $(PBRDIR)pst_gra.narc src/graphic/pst_gra.narc
	@ cmp $(PBRDIR)sound_data.sdat src/data/sound/sound_data.sdat
	@ cmp $(PBRDIR)waza_tbl.narc src/poketool/waza/waza_tbl.narc
	@ cmp $(PBRDIR)wifi.ncgr src/data/wifi.ncgr
	@ cmp $(PBRDIR)winframe.narc src/graphic/winframe.narc
	@ cmp $(PBRDIR)wm.ncgr src/data/wm.ncgr
	@ cmp $(PBRDIR)wm.nclr src/data/wm.nclr

