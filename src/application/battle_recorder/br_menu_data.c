//==============================================================================
/**
 * @file	br_menu_data.c
 * @brief	バトルレコーダー
 * @author	goto
 * @date	2007.07.26(木)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================
#include "common.h"
#include "system/procsys.h"
#include "system/arc_tool.h"
#include "system/arc_util.h"
#include "system/pm_str.h"
#include "system/clact_tool.h"
#include "system/msgdata_util.h"
#include "system/wordset.h"
#include "system/window.h"
#include "msgdata/msg.naix"
#include "msgdata/msg_fightmsg_dp.h"
#include "system/wipe.h"
#include "system/brightness.h"
#include "system/fontproc.h"
#include "system/lib_pack.h"
#include "system/fontoam.h"
#include "system/msg_ds_icon.h"
#include "gflib/msg_print.h"
#include "gflib/touchpanel.h"
#include "poketool/poke_tool.h"
#include "poketool/monsno.h"
#include "poketool/pokeicon.h"
#include "poketool/boxdata.h"
#include "system/bmp_menu.h"
#include "system/snd_tool.h"
#include "gflib/strbuf_family.h"
#include "system/pm_overlay.h"

#include "br_private.h"


//==============================================================
// Prototype
//==============================================================
static const BR_MENU_DATA br_brows_menu_top[ 5 ];
static const BR_MENU_DATA br_brows_menu_top2[ 5 ];
static const BR_MENU_DATA br_brows_menu_2_1[ 5 ];
static const BR_MENU_DATA br_brows_menu_2_2[ 5 ];
static const BR_MENU_DATA br_brows_menu_2_3[ 5 ];
static const BR_MENU_DATA br_brows_menu_2_4[ 5 ];

static const BR_MENU_DATA br_gds_menu_top[ 5 ];
static const BR_MENU_DATA br_gds_menu_2_1[ 5 ];
static const BR_MENU_DATA br_gds_menu_2_2[ 5 ];
static const BR_MENU_DATA br_gds_menu_2_3[ 5 ];
static const BR_MENU_DATA br_gds_menu_2_4[ 5 ];

static const BR_MENU_DATA br_photo_menu_top[ 5 ];
static const BR_MENU_DATA br_box_menu_top[ 5 ];
static const BR_MENU_DATA br_weekly_rank_menu_top[ 5 ];

static const BR_MENU_DATA br_video_rank_1_menu_top[ 5 ];
static const BR_MENU_DATA br_video_rank_2_menu_top[ 5 ];

static const BR_MENU_DATA br_video_menu_top[ 5 ];
static const BR_MENU_DATA br_video_menu_2_top[ 5 ];

static const BR_MENU_DATA br_gds_video_end_top[ 5 ];
static const BR_MENU_DATA br_gds_vrank_end_top[ 5 ];
static const BR_MENU_DATA br_gds_wrank_end_top[ 5 ];
static const BR_MENU_DATA br_gds_box_dress_end_top[ 5 ];

#if 0

typedef struct _BR_MENU_DATA {
	
	BOOL			active;		///< 有効なデータかどうか	
	BOOL			seamless;	///< シームレス
	int				msgID;		///< message
	int				ID;			///< 識別用	
	int				seq;		///< 次のシーケンス
	int				tag_seq;	///< タグの動き
	int				ex_param1;	///< 拡張データ
	const BR_MENU_DATA*	data;	///< データ
	
} BR_MENU_DATA;

#endif


// -----------------------------------------
//
//	メニューデータ
//
// -----------------------------------------
static const BR_MENU_DATA br_brows_menu_base[ BR_MODE_MAX ] = {
	///< BROWS MODE
	{
		TRUE,
		SEAM_LESS_ON,
		0xFF,
		0xFFFF,
		eMENU_DATA_SEQ_NONE,
		eINPUT_NONE,
		BR_EX_DATA_NONE,
		BR_EX_DATA_NONE,		
		br_brows_menu_top,
	},	
	///< GDS MODE 1
	{
		TRUE,
		SEAM_LESS_ON,
		0xFF,
		0xFFFF,
		eMENU_DATA_SEQ_NONE,
		eINPUT_NONE,
		BR_EX_DATA_NONE,
		BR_EX_DATA_NONE,
		br_gds_menu_top,
	},	
	
	///< GDS MODE 2
	{
		TRUE,
		SEAM_LESS_ON,
		0xFF,
		0xFFFF,
		eMENU_DATA_SEQ_NONE,
		eINPUT_NONE,
		BR_EX_DATA_NONE,
		BR_EX_DATA_NONE,
		br_video_menu_top,
	},	
	
	///< GDS MODE ビデオランキング
	{
		TRUE,
		SEAM_LESS_ON,
		0xFF,
		0xFFFF,
		eMENU_DATA_SEQ_NONE,
		eINPUT_NONE,
		BR_EX_DATA_NONE,
		BR_EX_DATA_NONE,
		br_video_rank_2_menu_top,
	},	
	
	///< GDS MODE 週間ランキング
	{
		TRUE,
		SEAM_LESS_ON,
		0xFF,
		0xFFFF,
		eMENU_DATA_SEQ_NONE,
		eINPUT_NONE,
		BR_EX_DATA_NONE,
		BR_EX_DATA_NONE,
		br_weekly_rank_menu_top,
	},	
	
	///< GDS MODE 写真（ドレスアップ）
	{
		TRUE,
		SEAM_LESS_ON,
		0xFF,
		0xFFFF,
		eMENU_DATA_SEQ_NONE,
		eINPUT_NONE,
		BR_EX_DATA_NONE,
		BR_EX_DATA_NONE,
		br_photo_menu_top,
	},	
	
	///< GDS MODE ボックスショット
	{
		TRUE,
		SEAM_LESS_ON,
		0xFF,
		0xFFFF,
		eMENU_DATA_SEQ_NONE,
		eINPUT_NONE,
		BR_EX_DATA_NONE,
		BR_EX_DATA_NONE,
		br_box_menu_top,
	},
};

// -----------------------------------------
//
//	トップメニュー
//
// -----------------------------------------
static const BR_MENU_DATA br_brows_menu_top[ 5 ] = {
	///< しょうぶのきろく
	{
		TRUE,
		SEAM_LESS_ON,
		msg_01,
		eTAG_FIGHT,
		eMENU_DATA_SEQ_SELECT,
		eINPUT_TAG_SELECT,
		BR_EX_DATA_NONE,
		BR_EX_DATA_NONE,
		br_brows_menu_2_1,
	},
	///< フロンティアきろく
	{
		TRUE,
		SEAM_LESS_ON,
		msg_02,
		eTAG_FRONTIER,
		eMENU_DATA_FREC_INIT,	///< フロンティアの記録
		eINPUT_TAG_SELECT,
		BR_EX_DATA_NONE,
		BR_EX_DATA_NONE,		
		NULL,
	},
	///< やめる
	{
		TRUE,
		SEAM_LESS_ON,
		msg_04,
		eTAG_END,
		eMENU_DATA_SEQ_END,
		eINPUT_END,
		BR_EX_DATA_NONE,
		BR_EX_DATA_NONE,		
		NULL,
	},
	{	FALSE,FALSE,0,0x0000,0,0,0,0,NULL },
	{	FALSE,FALSE,0,0x0000,0,0,0,0,NULL },
};

///< 未クリア時
static const BR_MENU_DATA br_brows_menu_top2[ 5 ] = {
	///< しょうぶのきろく
	{
		TRUE,
		SEAM_LESS_ON,
		msg_01,
		eTAG_FIGHT,
		eMENU_DATA_SEQ_SELECT,
		eINPUT_TAG_SELECT,
		BR_EX_DATA_NONE,
		BR_EX_DATA_NONE,
		br_brows_menu_2_1,
	},
	///< やめる
	{
		TRUE,
		SEAM_LESS_ON,
		msg_04,
		eTAG_END,
		eMENU_DATA_SEQ_END,
		eINPUT_END,
		BR_EX_DATA_NONE,
		BR_EX_DATA_NONE,		
		NULL,
	},
	{	FALSE,FALSE,0,0x0000,0,0,0,0,NULL },
	{	FALSE,FALSE,0,0x0000,0,0,0,0,NULL },
	{	FALSE,FALSE,0,0x0000,0,0,0,0,NULL },
};

// -----------------------------------------
//
//	しょうぶのきろく
//
// -----------------------------------------
static const BR_MENU_DATA br_brows_menu_2_1[ 5 ] = {
	///< じぶんのきろく
	{
		TRUE,
		SEAM_LESS_ON,
		msg_06,
		eTAG_MYDATA,
		eMENU_DATA_PROF_INIT,
		eINPUT_TAG_SELECT,
		LOADDATA_MYREC,
		BR_EX_DATA_NONE,
		NULL,
	},
	///< だれかのきろく
	{
		TRUE,
		SEAM_LESS_ON,
		msg_07,
		eTAG_ATHORDATA,
		eMENU_DATA_SEQ_SELECT,
		eINPUT_TAG_SELECT,
		BR_EX_DATA_NONE,
		BR_EX_DATA_NONE,
		br_brows_menu_2_2,
	},
	///< きろくをけす
	{
		TRUE,
		SEAM_LESS_ON,
		msg_08,
		eTAG_DELETE,
		eMENU_DATA_SEQ_SELECT,
		eINPUT_TAG_SELECT,
		BR_EX_DATA_NONE,
		BR_EX_DATA_NONE,
		br_brows_menu_2_3,
	},
	///< もどる
	{
		TRUE,
		SEAM_LESS_ON,
		msg_05,
		eTAG_BACK,
		eMENU_DATA_SEQ_BACK,
		eINPUT_TAG_BACK,
		BR_EX_DATA_NONE,
		BR_EX_DATA_NONE,
		NULL,
	},
	{	FALSE,FALSE,0,0x0000,0,0,0,0,NULL },
};

static const BR_MENU_DATA br_brows_menu_2_2[ 5 ] = {
	///< あき
	{
		TRUE,
		SEAM_LESS_ON,
		msg_10,
		eTAG_ATHORDATA,
		eMENU_DATA_PROF_INIT,	///< プロフィール
		eINPUT_TAG_SELECT,
		LOADDATA_DOWNLOAD0,
		BR_EX_DATA_NONE,
		NULL,
	},
	///< あき
	{
		TRUE,
		SEAM_LESS_ON,
		msg_10,
		eTAG_ATHORDATA,
		eMENU_DATA_PROF_INIT,	///< プロフィール
		eINPUT_TAG_SELECT,
		LOADDATA_DOWNLOAD1,
		BR_EX_DATA_NONE,
		NULL,
	},
	///< あき
	{
		TRUE,
		SEAM_LESS_ON,
		msg_10,
		eTAG_ATHORDATA,
		eMENU_DATA_PROF_INIT,	///< プロフィール
		eINPUT_TAG_SELECT,
		LOADDATA_DOWNLOAD2,
		BR_EX_DATA_NONE,
		NULL,
	},
	///< もどる
	{
		TRUE,
		SEAM_LESS_ON,
		msg_05,
		eTAG_BACK,
		eMENU_DATA_SEQ_BACK,
		eINPUT_TAG_BACK,
		BR_EX_DATA_NONE,
		BR_EX_DATA_NONE,
		NULL,
	},
	{	FALSE,FALSE,0,0x0000,0,0,0,0,NULL },
};


static const BR_MENU_DATA br_brows_menu_2_3[ 5 ] = {
	///< じぶんのきろくをけす
	{
		TRUE,
		SEAM_LESS_ON,
		msg_100,
		eTAG_MYDATA,
		eMENU_DATA_REC_DELETE,
		eINPUT_TAG_SELECT,
		LOADDATA_MYREC,
		BR_EX_DATA_NONE,
		NULL,
	},
	///< だれかのきろくをけす
	{
		TRUE,
		SEAM_LESS_ON,
		msg_101,
		eTAG_ATHORDATA,
		eMENU_DATA_SEQ_NONE,
		eINPUT_TAG_SELECT,
		BR_EX_DATA_NONE,
		BR_EX_DATA_NONE,
		br_brows_menu_2_4,
	},
	///< もどる
	{
		TRUE,
		SEAM_LESS_ON,
		msg_05,
		eTAG_BACK,
		eMENU_DATA_SEQ_BACK,
		eINPUT_TAG_BACK,
		BR_EX_DATA_NONE,
		BR_EX_DATA_NONE,
		NULL,
	},
	{	FALSE,FALSE,0,0x0000,0,0,0,0,NULL },
	{	FALSE,FALSE,0,0x0000,0,0,0,0,NULL },
};

static const BR_MENU_DATA br_brows_menu_2_4[ 5 ] = {
	///< あき
	{
		TRUE,
		SEAM_LESS_ON,
		msg_102,
		eTAG_ATHORDATA,
		eMENU_DATA_REC_DELETE,
		eINPUT_TAG_SELECT,
		LOADDATA_DOWNLOAD0,
		BR_EX_DATA_NONE,
		NULL,
	},
	///< あき
	{
		TRUE,
		SEAM_LESS_ON,
		msg_102,
		eTAG_ATHORDATA,
		eMENU_DATA_REC_DELETE,
		eINPUT_TAG_SELECT,
		LOADDATA_DOWNLOAD1,
		BR_EX_DATA_NONE,
		NULL,
	},
	///< あき
	{
		TRUE,
		SEAM_LESS_ON,
		msg_102,
		eTAG_ATHORDATA,
		eMENU_DATA_REC_DELETE,
		eINPUT_TAG_SELECT,
		LOADDATA_DOWNLOAD2,
		BR_EX_DATA_NONE,
		NULL,
	},
	///< もどる
	{
		TRUE,
		SEAM_LESS_ON,
		msg_05,
		eTAG_BACK,
		eMENU_DATA_SEQ_BACK,
		eINPUT_TAG_BACK,
		BR_EX_DATA_NONE,
		BR_EX_DATA_NONE,
		NULL,
	},
	{	FALSE,FALSE,0,0x0000,0,0,0,0,NULL },
};

// -----------------------------------------
//
//
//	■ GDS
//
//
// -----------------------------------------

// -----------------------------------------
//
//	トップメニュー
//
// -----------------------------------------
static const BR_MENU_DATA br_gds_menu_top[ 5 ] = {
	///< データをみる
	{
		TRUE,
		SEAM_LESS_ON,
		msg_200,
		eTAG_GDS_LOOK,
		eMENU_DATA_SEQ_SELECT,
		eINPUT_TAG_SELECT,
		BR_EX_DATA_NONE,
		BR_EX_DATA_NONE,

		br_gds_menu_2_1,
	},
	///< データを送る
	{
		TRUE,
		SEAM_LESS_ON,
		msg_201,
		eTAG_GDS_SEND,
		eMUENU_DATA_DUMMY,
		eINPUT_EXECUTED,
		BR_EX_DATA_NONE,
		BR_EX_DATA_NONE,
		
		NULL,
	},
	///< やめる
	{
		TRUE,
		SEAM_LESS_ON,
		msg_04,
		eTAG_GDS_END,
		eMENU_DATA_SEQ_END,
		eINPUT_END,
		BR_EX_DATA_NONE,
		BR_EX_DATA_NONE,
		
		NULL,
	},
	{	FALSE,FALSE,0,0x0000,0,0,0,0,NULL },
	{	FALSE,FALSE,0,0x0000,0,0,0,0,NULL },
};

static const BR_MENU_DATA br_gds_menu_2_1[ 5 ] = {
	///< さいしん３０個
	{
		TRUE,
		SEAM_LESS_ON,
		msg_202,
		eTAG_GDS_NEW30,
		eMUENU_DATA_DUMMY,
		eINPUT_EXECUTED,
		BR_EX_DATA_NONE,
		BR_EX_DATA_NONE,
		NULL,
	},
	///< くわしくさがす
	{
		TRUE,
		SEAM_LESS_ON,
		msg_203,
		eTAG_GDS_DETAIL_S,
		eMENU_DATA_SEQ_SELECT,
		eINPUT_TAG_SELECT,
		BR_EX_DATA_NONE,
		BR_EX_DATA_NONE,
		br_gds_menu_2_2,
	},
	///< データナンバーでさがす
	{
		TRUE,
		SEAM_LESS_ON,
		msg_204,
		eTAG_GDS_DATANO_S,
		eMUENU_DATA_DUMMY,
		eINPUT_EXECUTED,
		BR_EX_DATA_NONE,
		BR_EX_DATA_NONE,
		NULL,
	},
	///< もどる
	{
		TRUE,
		SEAM_LESS_ON,
		msg_05,
		eTAG_BACK,
		eMENU_DATA_SEQ_BACK,
		eINPUT_TAG_BACK,
		BR_EX_DATA_NONE,
		BR_EX_DATA_NONE,
		NULL,
	},
	{	FALSE,FALSE,0,0x0000,0,0,0,0,NULL },	
};

static const BR_MENU_DATA br_gds_menu_2_2[ 5 ] = {
	///< しせつでさがす
	{
		TRUE,
		SEAM_LESS_ON,
		msg_205,
		eTAG_GDS_FACILITIES_S,
		eMUENU_DATA_DUMMY,
		eINPUT_EXECUTED,
		BR_EX_DATA_NONE,
		BR_EX_DATA_NONE,
		NULL,
	},
	///< ポケモンでさがす
	{
		TRUE,
		SEAM_LESS_ON,
		msg_206,
		eTAG_GDS_DETAIL_S,
		eMUENU_DATA_DUMMY,
		eINPUT_EXECUTED,
		BR_EX_DATA_NONE,
		BR_EX_DATA_NONE,
		NULL,
	},
	///< プロフィールでさがす
	{
		TRUE,
		SEAM_LESS_ON,
		msg_207,
		eTAG_GDS_DETAIL_S,
		eMUENU_DATA_DUMMY,
		eINPUT_EXECUTED,
		BR_EX_DATA_NONE,
		BR_EX_DATA_NONE,
		NULL,
	},
	///< もどる
	{
		TRUE,
		SEAM_LESS_ON,
		msg_05,
		eTAG_BACK,
		eMENU_DATA_SEQ_BACK,
		eINPUT_TAG_BACK,
		BR_EX_DATA_NONE,
		BR_EX_DATA_NONE,
		NULL,
	},
	{	FALSE,FALSE,0,0x0000,0,0,0,0,NULL },	
};

/// ドレスアップ
static const BR_MENU_DATA br_photo_menu_top[ 5 ] = {
	///< ドレスをみる
	{
		TRUE,
		SEAM_LESS_ON,
		msg_301,
		eTAG_GDS_PHOTO,
		eMENU_DATA_PHOTO_RECV,
		eINPUT_TAG_SELECT,
		BR_EX_DATA_NONE,
		BR_EX_DATA_NONE,		
		NULL,
	},
	///< ドレスをおくる
	{
		TRUE,
		SEAM_LESS_ON,
		msg_300,
		eTAG_GDS_PHOTO,
		eMENU_DATA_PHOTO_SEND,
		eINPUT_TAG_SELECT,
		IMC_SAVEDATA_TELEVISION_MYDATA,	
		BR_EX_DATA_NONE,	
		NULL,
	},
	///< やめる
	{
		TRUE,
		SEAM_LESS_ON,
		msg_04,
		eTAG_GDS_BOX_DRESS_END,
		eMENU_DATA_SEQ_END,
		eINPUT_TAG_SELECT,
		BR_EX_WIFI_END,		
		BR_EX_DATA_NONE,
		br_gds_box_dress_end_top,
	},
	{	FALSE,FALSE,0,0x0000,0,0,0,0,NULL },
	{	FALSE,FALSE,0,0x0000,0,0,0,0,NULL },
};

/// ボックスショット
static const BR_MENU_DATA br_box_menu_top[ 5 ] = {
	///< ボックスみる
	{
		TRUE,
		SEAM_LESS_ON,
		msg_401,
		eTAG_GDS_BOX_SHOT,
		eMENU_DATA_BOX_RECV,
		eINPUT_TAG_SELECT,
		BR_EX_DATA_NONE,
		BR_EX_DATA_NONE,		
		NULL,
	},
	///< ボックスおくる
	{
		TRUE,
		SEAM_LESS_ON,
		msg_400,
		eTAG_GDS_BOX_SHOT,
		eMENU_DATA_BOX_SEND,
		eINPUT_TAG_SELECT,
		BR_EX_DATA_BOX_SHOT_SEND,	
		BR_EX_DATA_NONE,	
		NULL,
	},
	///< やめる
	{
		TRUE,
		SEAM_LESS_ON,
		msg_04,
		eTAG_GDS_BOX_DRESS_END,
		eMENU_DATA_SEQ_END,
		eINPUT_TAG_SELECT,
		BR_EX_WIFI_END,		
		BR_EX_DATA_NONE,
		br_gds_box_dress_end_top,
	},
	{	FALSE,FALSE,0,0x0000,0,0,0,0,NULL },
	{	FALSE,FALSE,0,0x0000,0,0,0,0,NULL },
};


/// 週間ランキング
static const BR_MENU_DATA br_weekly_rank_menu_top[ 5 ] = {
	///< 今週
	{
		TRUE,
		SEAM_LESS_ON,
		msg_500,
		eTAG_GDS_THIS_WEEK,
		eMENU_DATA_THIS_WEEK,
		eINPUT_TAG_SELECT,
		BR_EX_DATA_THIS_WEEK_RANK,	
		BR_EX_DATA_NONE,	
		NULL,
	},
	///< 先週
	{
		TRUE,
		SEAM_LESS_ON,
		msg_501,
		eTAG_GDS_LAST_WEEK,
		eMENU_DATA_LAST_WEEK,
		eINPUT_TAG_SELECT,
		BR_EX_DATA_LAST_WEEK_RANK,	
		BR_EX_DATA_NONE,	
		NULL,
	},
	///< やめる
	{
		TRUE,
		SEAM_LESS_ON,
		msg_04,
		eTAG_GDS_WEEKLY_RANK_END,
		eMENU_DATA_SEQ_END,
		eINPUT_TAG_SELECT,
		BR_EX_WIFI_END,		
		BR_EX_DATA_NONE,
		br_gds_wrank_end_top,
	},
	{	FALSE,FALSE,0,0x0000,0,0,0,0,NULL },
	{	FALSE,FALSE,0,0x0000,0,0,0,0,NULL },
};


/// ビデオランキング
static const BR_MENU_DATA br_video_rank_1_menu_top[ 5 ] = {
	///< 通信対戦
	{
		TRUE,
		SEAM_LESS_ON,
		msg_600,
		eTAG_GDS_VR_TUUSHIN,
		eMENU_DATA_COMM_BATTLE_RANKING,
		eINPUT_TAG_SELECT,
		BR_EX_DATA_BATTLE_RANK,
		BR_EX_DATA_NONE,
		NULL,
	},
	///< やめる
	{
		TRUE,
		SEAM_LESS_ON,
		msg_04,
		eTAG_GDS_VR_END,
		eMENU_DATA_SEQ_END,
		eINPUT_TAG_SELECT,
		BR_EX_WIFI_END,		
		BR_EX_DATA_NONE,
		br_gds_vrank_end_top,
	},
	{	FALSE,FALSE,0,0x0000,0,0,0,0,NULL },
	{	FALSE,FALSE,0,0x0000,0,0,0,0,NULL },
	{	FALSE,FALSE,0,0x0000,0,0,0,0,NULL },
};

/// ビデオランキング
static const BR_MENU_DATA br_video_rank_2_menu_top[ 5 ] = {
	///< 通信対戦
	{
		TRUE,
		SEAM_LESS_ON,
		msg_600,
		eTAG_GDS_VR_TUUSHIN,
		eMENU_DATA_COMM_BATTLE_RANKING,
		eINPUT_TAG_SELECT,
		BR_EX_DATA_BATTLE_RANK,
		BR_EX_DATA_NONE,
		NULL,
	},
	///< バトルフロンティア
	{
		TRUE,
		SEAM_LESS_ON,
		msg_601,
		eTAG_GDS_VR_FRONTIER,
		eMENU_DATA_BATTLE_FRONTIER_RANKING,
		eINPUT_TAG_SELECT,
		BR_EX_DATA_FRONTIER_RANK,
		BR_EX_DATA_NONE,		
		NULL,
	},
	///< やめる
	{
		TRUE,
		SEAM_LESS_ON,
		msg_04,
		eTAG_GDS_VR_END,
		eMENU_DATA_SEQ_END,
		eINPUT_TAG_SELECT,
		BR_EX_WIFI_END,		
		BR_EX_DATA_NONE,
		br_gds_vrank_end_top,
	},
	{	FALSE,FALSE,0,0x0000,0,0,0,0,NULL },
	{	FALSE,FALSE,0,0x0000,0,0,0,0,NULL },
};

static const BR_MENU_DATA br_video_send_check[ 5 ] = {	
	///< 送る
	{
		TRUE,
		SEAM_LESS_ON,
		msg_1000,
		eTAG_GDS_VIDEO_OK,
		eMENU_DATA_VIDE_SEND,
		eINPUT_TAG_SELECT,
		BR_EX_DATA_VIDEO_SEND,
		BR_EX_DATA_NONE,
		NULL,
	},
	///< もどる
	{
		TRUE,
		SEAM_LESS_ON,
		msg_05,
		eTAG_GDS_VIDEO_BACK,
		eMENU_DATA_SEQ_BACK,
		eINPUT_TAG_BACK,
		BR_EX_DATA_NONE,
		BR_EX_DATA_NONE,
		NULL,
	},
	{	FALSE,FALSE,0,0x0000,0,0,0,0,NULL },
	{	FALSE,FALSE,0,0x0000,0,0,0,0,NULL },
};

static const BR_MENU_DATA br_video_menu_top[ 5 ] = {
	///< ビデオをみる
	{
		TRUE,
		SEAM_LESS_ON,
		msg_700,
		eTAG_GDS_VIDEO_PLAY,
		eMENU_DATA_SEQ_SELECT,
		eINPUT_TAG_SELECT,
		BR_EX_DATA_NONE,
		BR_EX_DATA_NONE,
		br_video_menu_2_top,
	},
	///< ビデオをおくる
	{
		TRUE,
		SEAM_LESS_ON,
		msg_701,
		eTAG_GDS_VIDEO_SEND,
		eMENU_DATA_SEQ_SELECT,
		eINPUT_TAG_SELECT,
	//	BR_EX_DATA_VIDEO_SEND,	
		BR_EX_SEND_CHECK,
		BR_EX_DATA_NONE,
		br_video_send_check,
	},
	///< やめる
	{
		TRUE,
		SEAM_LESS_ON,
		msg_04,
		eTAG_GDS_VIDEO_END,
		eMENU_DATA_SEQ_END,
		eINPUT_TAG_SELECT,
		BR_EX_WIFI_END,		
		BR_EX_DATA_NONE,
		br_gds_video_end_top,
	},
	{	FALSE,FALSE,0,0x0000,0,0,0,0,NULL },
	{	FALSE,FALSE,0,0x0000,0,0,0,0,NULL },
};

static const BR_MENU_DATA br_video_menu_2_top[ 5 ] = {
	///< 最新３０個
	{
		TRUE,
		SEAM_LESS_ON,
		msg_702,
		eTAG_GDS_VIDEO_NEW30,
		eMENU_DATA_VIDE_NEW30,
		eINPUT_TAG_SELECT,
		BR_EX_DATA_NEW30,
		BR_EX_DATA_NONE,
		NULL,
	},
	///< くわしく
	{
		TRUE,
		SEAM_LESS_ON,
		msg_703,
		eTAG_GDS_VIDEO_KUWASIKU,
		eMENU_DATA_VIDE_KUWASIKU,
		eINPUT_TAG_SELECT,
		BR_EX_DATA_KUWASIKU,
		BR_EX_DATA_NONE,		
		NULL,
	},
	///< データナンバーで探す
	{
		TRUE,
		SEAM_LESS_ON,
		msg_704,
		eTAG_GDS_VIDEO_NO_SEARCH,
		eMENU_DATA_VIDE_NO_SEARCH,
		eINPUT_TAG_SELECT,
		BR_EX_DATA_NO_SEARCH,	
		BR_EX_DATA_NONE,	
		NULL,
	},
	///< もどる
	{
		TRUE,
		SEAM_LESS_ON,
		msg_05,
		eTAG_GDS_VIDEO_BACK,
		eMENU_DATA_SEQ_BACK,
		eINPUT_TAG_BACK,
		BR_EX_DATA_NONE,
		BR_EX_DATA_NONE,
		NULL,
	},
	{	FALSE,FALSE,0,0x0000,0,0,0,0,NULL },
};


// =============================================================================
//
//
//	■GDS　からの終了処理
//
//
// =============================================================================
static const BR_MENU_DATA br_gds_video_end_top[ 5 ] = 
{
	///< はい
	{
		TRUE,
		SEAM_LESS_ON,
		msg_1000,
		eTAG_GDS_VIDEO_OK,
		eMENU_DATA_SEQ_END,
		eINPUT_END,
		BR_EX_DATA_NONE,
		BR_EX_DATA_NONE,
		NULL,
	},
	///< もどる
	{
		TRUE,
		SEAM_LESS_ON,
		msg_05,
		eTAG_GDS_VIDEO_BACK,
		eMENU_DATA_SEQ_END,
		eINPUT_TAG_BACK,
		BR_EX_DATA_NONE,
		BR_EX_DATA_NONE,		
		NULL,
	},
	{	FALSE,FALSE,0,0x0000,0,0,0,0,NULL },
	{	FALSE,FALSE,0,0x0000,0,0,0,0,NULL },
	{	FALSE,FALSE,0,0x0000,0,0,0,0,NULL },
};

static const BR_MENU_DATA br_gds_vrank_end_top[ 5 ] = 
{
	///< はい
	{
		TRUE,
		SEAM_LESS_ON,
		msg_1000,
		eTAG_GDS_VR_3,
		eMENU_DATA_SEQ_END,
		eINPUT_END,
		BR_EX_DATA_NONE,
		BR_EX_DATA_NONE,
		NULL,
	},
	///< もどる
	{
		TRUE,
		SEAM_LESS_ON,
		msg_05,
		eTAG_GDS_VR_2,
		eMENU_DATA_SEQ_END,
		eINPUT_TAG_BACK,
		BR_EX_DATA_NONE,
		BR_EX_DATA_NONE,		
		NULL,
	},
	{	FALSE,FALSE,0,0x0000,0,0,0,0,NULL },
	{	FALSE,FALSE,0,0x0000,0,0,0,0,NULL },
	{	FALSE,FALSE,0,0x0000,0,0,0,0,NULL },
};

static const BR_MENU_DATA br_gds_wrank_end_top[ 5 ] = 
{
	///< はい
	{
		TRUE,
		SEAM_LESS_ON,
		msg_1000,
		eTAG_GDS_RANK_SAVE_OK,
		eMENU_DATA_SEQ_END,
		eINPUT_END,
		BR_EX_DATA_NONE,
		BR_EX_DATA_NONE,
		NULL,
	},
	///< もどる
	{
		TRUE,
		SEAM_LESS_ON,
		msg_05,
		eTAG_GDS_RANK_BACK,
		eMENU_DATA_SEQ_END,
		eINPUT_TAG_BACK,
		BR_EX_DATA_NONE,	
		BR_EX_DATA_NONE,	
		NULL,
	},
	{	FALSE,FALSE,0,0x0000,0,0,0,0,NULL },
	{	FALSE,FALSE,0,0x0000,0,0,0,0,NULL },
	{	FALSE,FALSE,0,0x0000,0,0,0,0,NULL },
};

static const BR_MENU_DATA br_gds_box_dress_end_top[ 5 ] = 
{
	///< はい
	{
		TRUE,
		SEAM_LESS_ON,
		msg_1000,
		eTAG_GDS_BOX_DRESS_OK,
		eMENU_DATA_SEQ_END,
		eINPUT_END,
		BR_EX_DATA_NONE,
		BR_EX_DATA_NONE,
		NULL,
	},
	///< もどる
	{
		TRUE,
		SEAM_LESS_ON,
		msg_05,
		eTAG_GDS_BOX_DRESS_BACK,
		eMENU_DATA_SEQ_END,
		eINPUT_TAG_BACK,
		BR_EX_DATA_NONE,
		BR_EX_DATA_NONE,		
		NULL,
	},
	{	FALSE,FALSE,0,0x0000,0,0,0,0,NULL },
	{	FALSE,FALSE,0,0x0000,0,0,0,0,NULL },
	{	FALSE,FALSE,0,0x0000,0,0,0,0,NULL },
};

//--------------------------------------------------------------
/**
 * @brief	メニューデータの取得 ( 以後 Link 構造の内部を参照していく)
 *
 * @param	mode	
 *
 * @retval	BR_MENU_DATA*	
 *
 */
//--------------------------------------------------------------
const BR_MENU_DATA* BattleRecorder_MenuDataGet( int mode )
{
	///< 指定よりでかい場合は、強制的に BR_MODE_BROWSE 
	if ( mode >= BR_MODE_MAX ){
		
	//	OS_Printf( "mode 指定がおかしい。 BR_MODE_BROWSE　のデータ取得\n");
		GF_ASSERT( mode < BR_MODE_MAX );
		
		return br_brows_menu_base[ BR_MODE_BROWSE ].data;
	}
	
	return br_brows_menu_base[ mode ].data;
}

//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 * @param	mode	
 *
 * @retval	const BR_MENU_DATA*	
 *
 */
//--------------------------------------------------------------
const BR_MENU_DATA* BattleRecorder_MenuDataGetEx( BR_WORK* wk, int mode )
{	
	///< ブラウズモード＆未クリア時
	if ( mode == BR_MODE_BROWSE ){
		if ( BR_IsGameClear( wk ) == FALSE ){
			
			return &br_brows_menu_top2[ 0 ];
		}
	}
	
	///< ビデオランキング＆未クリア
	if ( mode == BR_MODE_GDS_BV_RANK ){
		if ( BR_IsGameClear( wk ) == FALSE ){
			
			return &br_video_rank_1_menu_top[ 0 ];
		}
	}
	
	return BattleRecorder_MenuDataGet( mode );
}


//--------------------------------------------------------------
/**
 * @brief	メニュー画面のセットをする
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BattleRecorder_MenuSetting( BR_WORK* wk )
{
	int i;
	
	const BR_MENU_DATA* menu;
	
	menu = wk->menu_data;
	
	wk->tag_man.item_max = 0;
	
	for ( i = 0; i < TAG_MAX; i++ ){
		
		if ( menu[ i ].active == FALSE ){ continue; }
		
		wk->tag_man.item_max++;
	}
}

