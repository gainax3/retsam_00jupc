//==============================================================================
/**
 * @file	fss_scene.c
 * @brief	フロンティア用スクリプト：シーンデータ
 * @author	matsuda
 * @date	2007.04.03(火)
 */
//==============================================================================
#include "common.h"
#include "system/snd_tool.h"
#include "frontier_types.h"
#include "fss_scene.h"
#include "frontier_main.h"
#include "frontier_map.h"

#include "msgdata/msg.naix"
#include "script/fr_script.naix"

#include "graphic/frontier_bg_def.h"
#include "system/arc_tool.h"

#include "stage_bganm.h"

#include "frontier_tool.h"
#include "castle_tool.h"
#include "factory_tool.h"
#include "wifi_counter_tool.h"


//==============================================================================
//	型定義
//==============================================================================
///シーンデータの初期動作関数・終了動作関数の型
typedef void (*FSS_SCENE_FUNC)(FMAP_PTR, void **);


//==============================================================================
//	プロトタイプ宣言
//==============================================================================
static void TestInitFunc(FMAP_PTR fmap, void **ptr_work);
static void TestEndFunc(FMAP_PTR fmap, void **ptr_work);
static void InitFunc_Stage(FMAP_PTR fmap, void **ptr_work);
static void EndFunc_Stage(FMAP_PTR fmap, void **ptr_work);
static void InitFunc_RouletteWay(FMAP_PTR fmap, void **ptr_work);
static void EndFunc_RouletteWay(FMAP_PTR fmap, void **ptr_work);
static void InitFunc_Roulette(FMAP_PTR fmap, void **ptr_work);
static void EndFunc_Roulette(FMAP_PTR fmap, void **ptr_work);
static void InitFunc_Factory(FMAP_PTR fmap, void **ptr_work);
static void EndFunc_Factory(FMAP_PTR fmap, void **ptr_work);
static void InitFunc_FactoryBtl(FMAP_PTR fmap, void **ptr_work);
static void EndFunc_FactoryBtl(FMAP_PTR fmap, void **ptr_work);
static void InitFunc_WifiCounter(FMAP_PTR fmap, void **ptr_work);
static void EndFunc_WifiCounter( FMAP_PTR fmap, void** ptr_work );
static void InitFunc_CastleBtl(FMAP_PTR fmap, void **ptr_work);
static void EndFunc_CastleBtl(FMAP_PTR fmap, void **ptr_work);


//==============================================================================
//	構造体定義
//==============================================================================
///シーンデータ構造体
typedef struct{
	int bg_mode;		///<BGモード(テキストBG、拡張BGの設定など)
	
	FSS_SCENE_FUNC init_func;	///<初期動作関数
	FSS_SCENE_FUNC end_func;	///<終了動作関数
	
	u16 scr_id;			///<スクリプトID
	u16 msg_id;			///<メッセージID
	u16 bgm_id;			///<BGM ID(読み込む音色は"SND_SCENE_FIELD"固定になっています)
	u16 screen_size;	///<スクリーンサイズ(GF_BGL_SCRSIZ_???)
	
	u16 map_arc_id;		///<マップのアーカイブID
	u16 arc_screen_id;	///<マップスクリーンのID
	u16 arc_char_id;	///<マップキャラクタのID
	u16 arc_pltt_id;	///<マップパレットのID

	u16 arc_multi_screen_id;	///<多重面マップスクリーンのID(パレットはarc_pltt_idと共通)
	u16 arc_multi_char_id;		///<多重面マップキャラクタのID
	u16 arc_multi_pltt_id;		///<多重面マップパレットのID

	u8 scroll_mode;		///<SCROLL_MODE_???
	u8 multiple_scroll_mode;	///<多重面のスクロールを通常面と同期させるか(TRUE:同期, FALSE:システム側では多重面スクロールはいじらない)
	
	u8 dummy[2];		///<4バイト境界ダミー
}FSS_SCENE_DATA;


//==============================================================================
//	データ(GX_BGMODE_5,SCROLL_MODE_EASY)
//==============================================================================
//シーンデータテーブル
static const FSS_SCENE_DATA SceneDataTbl[] = {
	{//FSS_SCENEID_TESTSCR
		GX_BGMODE_0,	//BGモード(一時的なもの。全てのグラフィックが移行出来たら消す)
		TestInitFunc,							//初期動作関数
		TestEndFunc,							//終了動作関数
		NARC_fr_script_testscr_bin,		//スクリプトID
		NARC_msg_cmsg_acting_dat,		//メッセージID
		SEQ_BF_TOWWER,					//BGM ID
		GF_BGL_SCRSIZ_512x256,			//スクリーンサイズ
		ARC_FRONTIER_BG,				//マップのアーカイブID
		BF_ROOM_BATTLE_NSCR_BIN,		//マップスクリーンのID
		BF_ROOM_NCGR_BIN,				//マップキャラクタのID
		BF_ROOM_NCLR,					//マップパレットのID
		BF_ROOM_RAIL_NSCR_BIN,			//多重面のスクリーンID
		BF_ROOM_NCGR_BIN,				//多重面のキャラクタID
		BF_ROOM_NCLR,					//多重面のパレットID
		SCROLL_MODE_NORMAL,				//スクロールモード
		TRUE,							//多重面のスクロール同期
	},
	{//FSS_SCENEID_TESTSCR2		TESTSCRからマップ切り替えで来るマップ
		GX_BGMODE_0,	//BGモード(一時的なもの。全てのグラフィックが移行出来たら消す)
		NULL,							//初期動作関数
		NULL,							//終了動作関数
		0,		//スクリプトID	TESTSCRと共通
		NARC_msg_cmsg_acting_dat,		//メッセージID
		SEQ_BF_TOWWER,					//BGM ID
		GF_BGL_SCRSIZ_512x256,			//スクリーンサイズ
		ARC_FRONTIER_BG,				//マップのアーカイブID
		BF_ROOM_EXCHANGE_NSCR_BIN,		//マップスクリーンのID
		BF_ROOM_NCGR_BIN,				//マップキャラクタのID
		BF_ROOM_NCLR,					//マップパレットのID
		MULTI_BG_NULL,					//多重面のスクリーンID
		MULTI_BG_NULL,					//多重面のキャラクタID
		MULTI_BG_NULL,					//多重面のパレットID
		SCROLL_MODE_NORMAL,				//スクロールモード
		TRUE,							//多重面のスクロール同期
	},
	{//FSS_SCENEID_TESTSCR3		TESTSCRからMAP_CHANGE_EXで切り替えてくるマップ
		GX_BGMODE_0,	//BGモード(一時的なもの。全てのグラフィックが移行出来たら消す)
		NULL,							//初期動作関数
		NULL,							//終了動作関数
		NARC_fr_script_testscr3_bin,	//スクリプトID(EXなのでこれが読み込まれる)
		NARC_msg_cmsg_dance_dat,		//メッセージID
		SEQ_BF_TOWWER,					//BGM ID
		GF_BGL_SCRSIZ_512x256,			//スクリーンサイズ
		ARC_FRONTIER_BG,				//マップのアーカイブID
		BF_ROOM_BATTLE_NSCR_BIN,		//マップスクリーンのID
		BF_ROOM_NCGR_BIN,				//マップキャラクタのID
		BF_ROOM_NCLR,					//マップパレットのID
		MULTI_BG_NULL,					//多重面のスクリーンID
		MULTI_BG_NULL,					//多重面のキャラクタID
		MULTI_BG_NULL,					//多重面のパレットID
		SCROLL_MODE_NORMAL,				//スクロールモード
		TRUE,							//多重面のスクロール同期
	},
	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	{//FSS_SCENEID_FACTORY
		GX_BGMODE_5,	//BGモード(一時的なもの。全てのグラフィックが移行出来たら消す)
		InitFunc_Factory,				//初期動作関数
		EndFunc_Factory,				//終了動作関数
		NARC_fr_script_factory_bin,		//スクリプトID
		NARC_msg_factory_room_dat,		//メッセージID
		SEQ_PL_BF_FACTORY,				//BGM ID
		GF_BGL_SCRSIZ_512x512,			//スクリーンサイズ
		ARC_FRONTIER_BG,				//マップのアーカイブID
		BF_ROOM_EXCHANGE_NSCR_BIN,		//マップスクリーンのID
		BF_ROOM_NCGR_BIN,				//マップキャラクタのID
		BF_ROOM_NCLR,					//マップパレットのID
		BF_ROOM_RAIL_NSCR_BIN,			//多重面のスクリーンID
		BF_ROOM_NCGR_BIN,				//多重面のキャラクタID
		BF_ROOM_NCLR,					//多重面のパレットID
		SCROLL_MODE_EASY,				//スクロールモード
		//TRUE,							//多重面のスクロール同期
		FALSE,							//多重面のスクロール同期
	},
	{//FSS_SCENEID_FACTORY_BTL	マップ切り替えで来るマップ
		GX_BGMODE_5,	//BGモード(一時的なもの。全てのグラフィックが移行出来たら消す)
		InitFunc_FactoryBtl,			//初期動作関数
		EndFunc_FactoryBtl,				//終了動作関数
		NARC_fr_script_factory_bin,		//スクリプトID
		NARC_msg_factory_room_dat,		//メッセージID
		SEQ_PL_BF_FACTORY,				//BGM ID
		GF_BGL_SCRSIZ_256x256,			//スクリーンサイズ
		ARC_FRONTIER_BG,				//マップのアーカイブID
		BF_ROOM_BATTLE_NSCR_BIN,		//マップスクリーンのID
		BF_ROOM_NCGR_BIN,				//マップキャラクタのID
		BF_ROOM_NCLR,					//マップパレットのID
		MULTI_BG_NULL,					//多重面のスクリーンID
		MULTI_BG_NULL,					//多重面のキャラクタID
		MULTI_BG_NULL,					//多重面のパレットID
		SCROLL_MODE_EASY,				//スクロールモード
		TRUE,							//多重面のスクロール同期
	},
	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	{//FSS_SCENEID_TOWER_SINGLE_WAY	マップ切り替えで来るマップ
		GX_BGMODE_5,	//BGモード(一時的なもの。全てのグラフィックが移行出来たら消す)
		NULL,							//初期動作関数
		NULL,							//終了動作関数
		NARC_fr_script_tower3_bin,		//スクリプトID
		NARC_msg_d31r0201_dat,			//メッセージID(r0203は存在しない)
		SEQ_BF_TOWWER,					//BGM ID
		GF_BGL_SCRSIZ_512x512,			//スクリーンサイズ
		ARC_FRONTIER_BG,				//マップのアーカイブID
		BT_WAY_NSCR_BIN,				//マップスクリーンのID
		BT_NCGR_BIN,					//マップキャラクタのID
		BT_NCLR,						//マップパレットのID
		BT_WAY_A_NSCR_BIN,				//多重面のスクリーンID
		BT_NCGR_BIN,					//多重面のキャラクタID
		BT_NCLR,						//多重面のパレットID
		SCROLL_MODE_EASY,				//スクロールモード
		TRUE,							//多重面のスクロール同期
	},
	{//FSS_SCENEID_TOWER_MULTI_WAY	マップ切り替えで来るマップ
		GX_BGMODE_5,	//BGモード(一時的なもの。全てのグラフィックが移行出来たら消す)
		NULL,							//初期動作関数
		NULL,							//終了動作関数
		NARC_fr_script_tower4_bin,		//スクリプトID
		NARC_msg_d31r0201_dat,			//メッセージID(r0204は存在しない)
		SEQ_BF_TOWWER,					//BGM ID
		GF_BGL_SCRSIZ_512x512,			//スクリーンサイズ
		ARC_FRONTIER_BG,				//マップのアーカイブID
		BT_MULWAY_NSCR_BIN,				//マップスクリーンのID
		BT_NCGR_BIN,					//マップキャラクタのID
		BT_NCLR,						//マップパレットのID
		BT_MULWAY_A_NSCR_BIN,			//多重面のスクリーンID
		BT_NCGR_BIN,					//多重面のキャラクタID
		BT_NCLR,						//多重面のパレットID
		SCROLL_MODE_EASY,				//スクロールモード
		TRUE,							//多重面のスクロール同期
	},
	{//FSS_SCENEID_TOWER_SINGLE_BTL	マップ切り替えで来るマップ
		GX_BGMODE_5,	//BGモード(一時的なもの。全てのグラフィックが移行出来たら消す)
		NULL,							//初期動作関数
		NULL,							//終了動作関数
		NARC_fr_script_tower5_bin,		//スクリプトID
		NARC_msg_d31r0205_dat,			//メッセージID
		SEQ_BF_TOWWER,					//BGM ID
		GF_BGL_SCRSIZ_256x256,			//スクリーンサイズ
		ARC_FRONTIER_BG,				//マップのアーカイブID
		BT_SBATTLE_NSCR_BIN,			//マップスクリーンのID
		BT_NCGR_BIN,					//マップキャラクタのID
		BT_NCLR,						//マップパレットのID
		MULTI_BG_NULL,					//多重面のスクリーンID
		MULTI_BG_NULL,					//多重面のキャラクタID
		MULTI_BG_NULL,					//多重面のパレットID
		SCROLL_MODE_EASY,				//スクロールモード
		TRUE,							//多重面のスクロール同期
	},
	{//FSS_SCENEID_TOWER_MULTI_BTL	マップ切り替えで来るマップ
		GX_BGMODE_5,	//BGモード(一時的なもの。全てのグラフィックが移行出来たら消す)
		NULL,							//初期動作関数
		NULL,							//終了動作関数
		NARC_fr_script_tower6_bin,		//スクリプトID
		NARC_msg_d31r0206_dat,			//メッセージID
		SEQ_BF_TOWWER,					//BGM ID
		GF_BGL_SCRSIZ_256x256,			//スクリーンサイズ
		ARC_FRONTIER_BG,				//マップのアーカイブID
		BT_MULBATTLE_NSCR_BIN,			//マップスクリーンのID
		BT_NCGR_BIN,					//マップキャラクタのID
		BT_NCLR,						//マップパレットのID
		MULTI_BG_NULL,					//多重面のスクリーンID
		MULTI_BG_NULL,					//多重面のキャラクタID
		MULTI_BG_NULL,					//多重面のパレットID
		SCROLL_MODE_EASY,				//スクロールモード
		TRUE,							//多重面のスクロール同期
	},
	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	{//FSS_SCENEID_STAGE_WAY
		GX_BGMODE_5,	//BGモード(一時的なもの。全てのグラフィックが移行出来たら消す)
		NULL,							//初期動作関数
		NULL,							//終了動作関数
		NARC_fr_script_stage_bin,		//スクリプトID
		NARC_msg_stage_room_dat,		//メッセージID
		SEQ_PL_BF_STAGE,				//BGM ID
		GF_BGL_SCRSIZ_256x256,			//スクリーンサイズ
		ARC_FRONTIER_BG,				//マップのアーカイブID
		BS_STAGE_ROUKA_NSCR_BIN,		//マップスクリーンのID
		BS_STAGE_ROUKA_NCGR_BIN,		//マップキャラクタのID
		BS_STAGE_ROUKA_NCLR,			//マップパレットのID
		MULTI_BG_NULL,					//多重面のスクリーンID
		MULTI_BG_NULL,					//多重面のキャラクタID
		MULTI_BG_NULL,					//多重面のパレットID
		SCROLL_MODE_EASY,				//スクロールモード
		TRUE,							//多重面のスクロール同期
	},
	{//FSS_SCENEID_STAGE_BTL
		GX_BGMODE_5,	//BGモード(一時的なもの。全てのグラフィックが移行出来たら消す)
		InitFunc_Stage,							//初期動作関数
		EndFunc_Stage,							//終了動作関数
		NARC_fr_script_stage_bin,		//スクリプトID
		NARC_msg_stage_room_dat,		//メッセージID
		SEQ_PL_BF_STAGE,				//BGM ID
		GF_BGL_SCRSIZ_512x512,			//スクリーンサイズ
		ARC_FRONTIER_BG,				//マップのアーカイブID
		BS_STAGE_01_A_NSCR_BIN,			//マップスクリーンのID
		BS_STAGE_NCGR_BIN,				//マップキャラクタのID
		BS_STAGE_NCLR,					//マップパレットのID
		BS_STAGE_ALPHA_A_NSCR_BIN,			//多重面のスクリーンID
		BS_STAGE_ALPHA_NCGR_BIN,				//多重面のキャラクタID
		BS_STAGE_ALPHA_NCLR,			//多重面のパレットID
		SCROLL_MODE_EASY,				//スクロールモード
		TRUE,							//多重面のスクロール同期
	},
	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	{//FSS_SCENEID_CASTLE
		GX_BGMODE_5,	//BGモード(一時的なもの。全てのグラフィックが移行出来たら消す)
		NULL,							//初期動作関数
		NULL,							//終了動作関数
		NARC_fr_script_castle_bin,		//スクリプトID
		NARC_msg_castle_room_dat,		//メッセージID
		SEQ_PL_BF_CASTLE,				//BGM ID
		GF_BGL_SCRSIZ_512x512,			//スクリーンサイズ
		ARC_FRONTIER_BG,				//マップのアーカイブID
		BC_OHMA_NSCR_BIN,				//マップスクリーンのID
		BC_OHMA_NCGR_BIN,				//マップキャラクタのID
		BC_OHMA_NCLR,					//マップパレットのID
		MULTI_BG_NULL,					//多重面のスクリーンID
		MULTI_BG_NULL,					//多重面のキャラクタID
		MULTI_BG_NULL,					//多重面のパレットID
		SCROLL_MODE_EASY,				//スクロールモード
		TRUE,							//多重面のスクロール同期
	},
	{//FSS_SCENEID_CASTLE_BTL	マップ切り替えで来るマップ
		GX_BGMODE_5,	//BGモード(一時的なもの。全てのグラフィックが移行出来たら消す)
		InitFunc_CastleBtl,				//初期動作関数
		EndFunc_CastleBtl,				//終了動作関数
		NARC_fr_script_castle_bin,		//スクリプトID(共通)
		NARC_msg_castle_room_dat,		//メッセージID(共通)
		SEQ_PL_BF_CASTLE,				//BGM ID
		GF_BGL_SCRSIZ_256x256,			//スクリーンサイズ
		ARC_FRONTIER_BG,				//マップのアーカイブID
		BC_ROOM_NSCR_BIN,				//マップスクリーンのID
		BC_ROOM_NCGR_BIN,				//マップキャラクタのID
		BC_ROOM_NCLR,					//マップパレットのID
		BC_ROOM_A_NSCR_BIN,				//多重面のスクリーンID
		BC_ROOM_NCGR_BIN,				//多重面のキャラクタID
		BC_ROOM_NCLR,					//多重面のパレットID
		SCROLL_MODE_EASY,				//スクロールモード
		TRUE,							//多重面のスクロール同期
	},
	{//FSS_SCENEID_CASTLE_BYPATH	マップ切り替えで来るマップ
		GX_BGMODE_5,	//BGモード(一時的なもの。全てのグラフィックが移行出来たら消す)
		NULL,							//初期動作関数
		NULL,							//終了動作関数
		NARC_fr_script_castle_bin,		//スクリプトID(共通)
		NARC_msg_castle_room_dat,		//メッセージID(共通)
		SEQ_PL_BF_CASTLE,				//BGM ID
		GF_BGL_SCRSIZ_256x256,			//スクリーンサイズ
		ARC_FRONTIER_BG,				//マップのアーカイブID
		BC_ROUKA_NSCR_BIN,				//マップスクリーンのID
		BC_ROUKA_NCGR_BIN,				//マップキャラクタのID
		BC_ROUKA_NCLR,					//マップパレットのID
		MULTI_BG_NULL,					//多重面のスクリーンID
		MULTI_BG_NULL,					//多重面のキャラクタID
		MULTI_BG_NULL,					//多重面のパレットID
		SCROLL_MODE_EASY,				//スクロールモード
		TRUE,							//多重面のスクロール同期
	},
	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	{//FSS_SCENEID_WIFI_COUNTER
		GX_BGMODE_5,	//BGモード(一時的なもの。全てのグラフィックが移行出来たら消す)
		InitFunc_WifiCounter,			//初期動作関数
		EndFunc_WifiCounter,			//終了動作関数
		NARC_fr_script_wifi_counter_bin,//スクリプトID
		NARC_msg_bf_info_dat,			//メッセージID
		SEQ_BF_TOWWER,					//BGM ID
		GF_BGL_SCRSIZ_256x256,			//スクリーンサイズ
		ARC_FRONTIER_BG,				//マップのアーカイブID
		BF_CONNECT_NSCR_BIN,			//マップスクリーンのID
		BF_CONNECT_NCGR_BIN,			//マップキャラクタのID
		BF_CONNECT_NCLR,				//マップパレットのID
		MULTI_BG_NULL,					//多重面のスクリーンID
		MULTI_BG_NULL,					//多重面のキャラクタID
		MULTI_BG_NULL,					//多重面のパレットID
		SCROLL_MODE_EASY,				//スクロールモード
		TRUE,							//多重面のスクロール同期
	},
	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	{//FSS_SCENEID_ROULETTE_WAY
		GX_BGMODE_5,	//BGモード(一時的なもの。全てのグラフィックが移行出来たら消す)
		InitFunc_RouletteWay,			//初期動作関数
		EndFunc_RouletteWay,			//終了動作関数
		NARC_fr_script_roulette_bin,	//スクリプトID
		NARC_msg_roulette_room_dat,		//メッセージID
		SEQ_PL_BF_ROULETTE,				//BGM ID
		GF_BGL_SCRSIZ_256x256,			//スクリーンサイズ
		ARC_FRONTIER_BG,				//マップのアーカイブID
		BR_ROUKA_NSCR_BIN,				//マップスクリーンのID
		BR_ROUKA_NCGR_BIN,				//マップキャラクタのID
		BR_ROUKA_NCLR,					//マップパレットのID
		MULTI_BG_NULL,					//多重面のスクリーンID
		MULTI_BG_NULL,					//多重面のキャラクタID
		MULTI_BG_NULL,					//多重面のパレットID
		SCROLL_MODE_EASY,				//スクロールモード
		TRUE,							//多重面のスクロール同期
	},
	{//FSS_SCENEID_ROULETTE_BTL
		GX_BGMODE_5,	//BGモード(一時的なもの。全てのグラフィックが移行出来たら消す)
		InitFunc_Roulette,				//初期動作関数
		EndFunc_Roulette,				//終了動作関数
		NARC_fr_script_roulette_bin,	//スクリプトID
		NARC_msg_roulette_room_dat,		//メッセージID
		SEQ_PL_BF_ROULETTE,				//BGM ID
		GF_BGL_SCRSIZ_512x512,			//スクリーンサイズ
		ARC_FRONTIER_BG,				//マップのアーカイブID
		BR_ROOM_NSCR_BIN,				//マップスクリーンのID
		BR_ROOM_NCGR_BIN,				//マップキャラクタのID
		BR_ROOM_NCLR,					//マップパレットのID
		BR_KEKKA01_HP_A_NSCR_BIN,		//多重面のスクリーンID
		BR_KEKKA01_HP_NCGR_BIN,			//多重面のキャラクタID
		BR_ROOM_KEKKA_NCLR,				//多重面のパレットID
		SCROLL_MODE_EASY,				//スクロールモード
		TRUE,							//多重面のスクロール同期
	},
};


//==============================================================================
//
//	
//
//==============================================================================

//--------------------------------------------------------------
/**
 * @brief   シーンIDからシーンデータの各種パラメータを取得する
 *
 * @param   scene_id		シーンID
 * @param   param_code		取得したいデータ(FSS_SCENE_DATA_???)
 *
 * @retval  取得データ
 */
//--------------------------------------------------------------
int FSS_SceneParamGet(int scene_id, int param_code)
{
	switch(param_code){
	case FSS_SCENE_DATA_BG_MODE:
		return SceneDataTbl[scene_id].bg_mode;
	case FSS_SCENE_DATA_SCRID:
		return SceneDataTbl[scene_id].scr_id;
	case FSS_SCENE_DATA_MSGID:
		return SceneDataTbl[scene_id].msg_id;
	case FSS_SCENE_DATA_BGMID:
		return SceneDataTbl[scene_id].bgm_id;
	case FSS_SCENE_DATA_SCREEN_SIZE:
		return SceneDataTbl[scene_id].screen_size;
	case FSS_SCENE_DATA_MAP_ARCID:
		return SceneDataTbl[scene_id].map_arc_id;
	case FSS_SCENE_DATA_MAP_SCREENID:
		return SceneDataTbl[scene_id].arc_screen_id;
	case FSS_SCENE_DATA_MAP_CHARID:
		return SceneDataTbl[scene_id].arc_char_id;
	case FSS_SCENE_DATA_MAP_PLTTID:
		return SceneDataTbl[scene_id].arc_pltt_id;
	case FSS_SCENE_DATA_MULTI_MAP_SCREENID:
		return SceneDataTbl[scene_id].arc_multi_screen_id;
	case FSS_SCENE_DATA_MULTI_MAP_CHARID:
		return SceneDataTbl[scene_id].arc_multi_char_id;
	case FSS_SCENE_DATA_MULTI_MAP_PLTTID:
		return SceneDataTbl[scene_id].arc_multi_pltt_id;
	case FSS_SCENE_DATA_SCROLL_MODE:
		return SceneDataTbl[scene_id].scroll_mode;
	case FSS_SCENE_DATA_MULTI_SCROLL_MODE:
		return SceneDataTbl[scene_id].multiple_scroll_mode;
	}
	
	GF_ASSERT(0);
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief   シーンデータに設定されている初期動作関数呼び出し
 *
 * @param   fmap			
 * @param   ptr_work		初期動作関数内でAllocした場合のポインタ保存場所
 * @param   scene_id		シーンID
 */
//--------------------------------------------------------------
void FSS_SceneInitFuncCall(FMAP_PTR fmap, void **ptr_work, int scene_id)
{
	if(SceneDataTbl[scene_id].init_func != NULL){
		SceneDataTbl[scene_id].init_func(fmap, ptr_work);
	}
}

//--------------------------------------------------------------
/**
 * @brief   シーンデータに設定されている終了動作関数呼び出し
 *
 * @param   fmap			
 * @param   ptr_work		初期動作関数内でポインタなどをセットした場合、このワークに入っている
 * @param   scene_id		シーンID
 */
//--------------------------------------------------------------
void FSS_SceneEndFuncCall(FMAP_PTR fmap, void **ptr_work, int scene_id)
{
	if(SceneDataTbl[scene_id].end_func != NULL){
		SceneDataTbl[scene_id].end_func(fmap, ptr_work);
	}
}



//==============================================================================
//	初期動作関数・終了動作関数
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   初期動作関数の実行テスト
 *
 * @param   fmap			
 * @param   ptr_work		初期動作関数内でメモリ確保した場合のポインタ保持領域
 */
//--------------------------------------------------------------
static void TestInitFunc(FMAP_PTR fmap, void **ptr_work)
{
	OS_TPrintf("初期動作関数が呼ばれた\n");
	
	//メモリ確保が必要な場合はここで確保し、ポインタをシステム側が用意している領域に保存させます
	//ヒープIDは基本的にはHEAPID_FRONTIERMAPを使用。
	*ptr_work = sys_AllocMemory(HEAPID_FRONTIERMAP, 32);
}

//--------------------------------------------------------------
/**
 * @brief   終了動作関数の実行テスト
 *
 * @param   fmap			
 * @param   ptr_work		初期動作関数内でメモリ確保した場合のポインタ保持領域
 */
//--------------------------------------------------------------
static void TestEndFunc(FMAP_PTR fmap, void **ptr_work)
{
	OS_TPrintf("終了動作関数が呼ばれた\n");

	//InitFuncで確保して使用していたメモリをこのタイミングで解放させることが出来ます
	sys_FreeMemoryEz(*ptr_work);
}

//==============================================================================
//	ステージ：初期動作関数
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   バトルステージ：初期動作関数
 *
 * @param   fmap		
 * @param   ptr_work		
 */
//--------------------------------------------------------------
static void InitFunc_Stage(FMAP_PTR fmap, void **ptr_work)
{
	//BGアニメ制御システム作成
	*ptr_work = StageAnimation_Init(fmap->bgl, fmap->pfd);

	GF_BGL_PrioritySet( GF_BGL_FRAME0_M, 1 );
	GF_BGL_PrioritySet( FRMAP_FRAME_EFF, 2 );
	GF_BGL_PrioritySet( FRMAP_FRAME_MAP, 3 );

	//GF_Disp_GX_VisibleControl( FRMAP_FRAME_EFF, VISIBLE_OFF );	//BG非表示
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );		//BG非表示
	return;
}

//--------------------------------------------------------------
/**
 * @brief   バトルステージ：終了動作関数
 *
 * @param   fmap		
 * @param   ptr_work		
 */
//--------------------------------------------------------------
static void EndFunc_Stage(FMAP_PTR fmap, void **ptr_work)
{
	//BGアニメ制御システム解放
	StageAnimation_End(*ptr_work);
	return;
}

//==============================================================================
//	ルーレット通路：初期動作関数
//==============================================================================

//--------------------------------------------------------------
/**
 * @brief   バトルルーレット通路：初期動作関数
 *
 * @param   fmap		
 * @param   ptr_work		
 *
 * ROULETTE_SCRWORKはまだ確保されていないのでアクセスしてはダメ！
 */
//--------------------------------------------------------------
static void InitFunc_RouletteWay(FMAP_PTR fmap, void **ptr_work)
{
	OS_Printf( "\n***********************\nルーレット通路　初期動作　開始\n" );

#if 0
	PaletteFadeReq( fmap->pfd, 0xffff, 0xffff, 0, 
	//PaletteFadeReq( fmap->pfd, PF_BIT_MAIN_BG, 0xffff, 0, 
					0,									//開始濃度
					8,									//最終濃度 
					0x0000 );							//変更後の色
#endif
	return;
}

//--------------------------------------------------------------
/**
 * @brief   バトルルーレット通路：終了動作関数
 *
 * @param   fmap		
 * @param   ptr_work		
 *
 * ROULETTE_SCRWORKはまだ確保されていないのでアクセスしてはダメ！
 */
//--------------------------------------------------------------
static void EndFunc_RouletteWay(FMAP_PTR fmap, void **ptr_work)
{
	OS_Printf( "\n***********************\nルーレット通路　初期動作　終了\n" );
	return;
}

//==============================================================================
//	ルーレット：初期動作関数
//==============================================================================

//--------------------------------------------------------------
/**
 * @brief   バトルルーレット：初期動作関数
 *
 * @param   fmap		
 * @param   ptr_work		
 *
 * ROULETTE_SCRWORKはまだ確保されていないのでアクセスしてはダメ！
 */
//--------------------------------------------------------------
static void InitFunc_Roulette(FMAP_PTR fmap, void **ptr_work)
{
	OS_Printf( "\n***********************\nルーレット　初期動作　開始\n" );

#if 0
	PaletteFadeReq( fmap->pfd, 0xffff, 0xffff, 0, 
	//PaletteFadeReq( fmap->pfd, PF_BIT_MAIN_BG, 0xffff, 0, 
					0,									//開始濃度
					8,									//最終濃度 
					0x0000 );							//変更後の色
#endif

	GF_BGL_PrioritySet( GF_BGL_FRAME0_M, 1 );
	//GF_BGL_PrioritySet( FRMAP_FRAME_WIN, 0 );				//メッセージを最前面
	GF_BGL_PrioritySet( FRMAP_FRAME_EFF, 2 );
	GF_BGL_PrioritySet( FRMAP_FRAME_MAP, 3 );

#if 1
	G2_SetBlendAlpha(	GX_BLEND_PLANEMASK_BG0, 
						GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 | 
						GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD, 
						20, 20 );	//ここの値はパーティクルデータ側の値が反映するので意味なし
#else
	G2_SetBlendAlpha(	GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_OBJ,
						GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 | 
						GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD, 
						20, 20 );	//ここの値はパーティクルデータ側の値が反映するので意味なし
#endif

	//GF_Disp_GX_VisibleControl( FRMAP_FRAME_EFF, VISIBLE_OFF );	//BG非表示
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );		//BG非表示
	return;
}

//--------------------------------------------------------------
/**
 * @brief   バトルルーレット：終了動作関数
 *
 * @param   fmap		
 * @param   ptr_work		
 *
 * ROULETTE_SCRWORKはまだ確保されていないのでアクセスしてはダメ！
 */
//--------------------------------------------------------------
static void EndFunc_Roulette(FMAP_PTR fmap, void **ptr_work)
{
	OS_Printf( "\n***********************\nルーレット　初期動作　終了\n" );
	return;
}


//==============================================================================
//	ファクトリー：初期動作関数
//==============================================================================

//--------------------------------------------------------------
/**
 * @brief   バトルファクトリー：初期動作関数
 *
 * @param   fmap		
 * @param   ptr_work		
 *
 * FACTORY_SCRWORKはまだ確保されていないのでアクセスしてはダメ！
 */
//--------------------------------------------------------------
static void InitFunc_Factory(FMAP_PTR fmap, void **ptr_work)
{
	OS_Printf( "\n***********************\nファクトリー　初期動作　開始\n" );
	GF_BGL_PrioritySet( FRMAP_FRAME_EFF, 3 );
	GF_BGL_PrioritySet( FRMAP_FRAME_MAP, 2 );
	return;
}

//--------------------------------------------------------------
/**
 * @brief   バトルファクトリー：終了動作関数
 *
 * @param   fmap		
 * @param   ptr_work		
 *
 * FACTORY_SCRWORKはまだ確保されていないのでアクセスしてはダメ！
 */
//--------------------------------------------------------------
static void EndFunc_Factory(FMAP_PTR fmap, void **ptr_work)
{
	OS_Printf( "\n***********************\nファクトリー　初期動作　終了\n" );
	return;
}

//--------------------------------------------------------------
/**
 * @brief   バトルファクトリー対戦部屋：初期動作関数
 *
 * @param   fmap		
 * @param   ptr_work		
 *
 * FACTORY_SCRWORKはまだ確保されていないのでアクセスしてはダメ！
 */
//--------------------------------------------------------------
static void InitFunc_FactoryBtl(FMAP_PTR fmap, void **ptr_work)
{
	OS_Printf( "\n***********************\nファクトリー　初期動作　開始\n" );
#if 0
	PaletteFadeWorkAllocSet( fmap->pfd, FADE_MAIN_BG_EX3, 2*6, HEAPID_FRONTIERMAP );
	SoftFadePfd( fmap->pfd, FADE_MAIN_BG_EX3, 0, 1, 16, 0x0000 );
	SoftFadePfd( fmap->pfd, FADE_MAIN_BG_EX3, 1, 1, 16, 0x3d6b );
	SoftFadePfd( fmap->pfd, FADE_MAIN_BG_EX3, 2, 1, 16, 0x3d8c );
	SoftFadePfd( fmap->pfd, FADE_MAIN_BG_EX3, 3, 1, 16, 0x418c );
	SoftFadePfd( fmap->pfd, FADE_MAIN_BG_EX3, 4, 1, 16, 0x3d6b );
	SoftFadePfd( fmap->pfd, FADE_MAIN_BG_EX3, 5, 1, 16, 0x6f37 );	//ここまで必要
#endif

	G2_SetBlendAlpha(	GX_BLEND_PLANEMASK_BG0, 
						GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 | 
						GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD, 
						20, 20 );	//ここの値はパーティクルデータ側の値が反映するので意味なし

	return;
}

//--------------------------------------------------------------
/**
 * @brief   バトルファクトリー対戦部屋：終了動作関数
 *
 * @param   fmap		
 * @param   ptr_work		
 *
 * FACTORY_SCRWORKはまだ確保されていないのでアクセスしてはダメ！
 */
//--------------------------------------------------------------
static void EndFunc_FactoryBtl(FMAP_PTR fmap, void **ptr_work)
{
	OS_Printf( "\n***********************\nファクトリー　初期動作　終了\n" );
#if 0
	PaletteFadeWorkAllocFree( fmap->pfd, FADE_MAIN_BG_EX3 );
#endif
	return;
}


//==============================================================================
//	wifiカウンター：初期動作関数
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   wifiカウンター：初期動作関数
 *
 * @param   fmap		
 * @param   ptr_work		
 */
//--------------------------------------------------------------
static void InitFunc_WifiCounter(FMAP_PTR fmap, void **ptr_work)
{
	*ptr_work = WifiCounterMonitor_Init( fmap->bgl );
	return;
}

//--------------------------------------------------------------
/**
 * @brief   wifiカウンター：終了動作関数
 *
 * @param   fmap		
 * @param   ptr_work		
 */
//--------------------------------------------------------------
static void EndFunc_WifiCounter( FMAP_PTR fmap, void** ptr_work )
{
	WifiCounterMonitor_End( *ptr_work );
	return;
}


//==============================================================================
//	キャッスルBTL：初期動作関数
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   キャッスルBTL：初期動作関数
 *
 * @param   fmap		
 * @param   ptr_work		
 */
//--------------------------------------------------------------
static void InitFunc_CastleBtl(FMAP_PTR fmap, void **ptr_work)
{
	//GF_BGL_PrioritySet( FRMAP_FRAME_EFF, 0 );
	
	GF_BGL_PrioritySet( FRMAP_FRAME_WIN, 0 );				//メッセージを最前面
#if 0
	GF_BGL_PrioritySet( GF_BGL_FRAME0_M, 1 );				//パーティクル
	GF_BGL_PrioritySet( FRMAP_FRAME_EFF, 2 );
	GF_BGL_PrioritySet( FRMAP_FRAME_MAP, 3 );
#else
	GF_BGL_PrioritySet( GF_BGL_FRAME0_M, 2 );				//パーティクル
	GF_BGL_PrioritySet( FRMAP_FRAME_EFF, 2 );
	GF_BGL_PrioritySet( FRMAP_FRAME_MAP, 3 );
#endif

	G2_SetBlendAlpha(	GX_BLEND_PLANEMASK_BG0, 
						GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 | 
						GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD, 
						20, 20 );	//ここの値はパーティクルデータ側の値が反映するので意味なし
	return;
}

//--------------------------------------------------------------
/**
 * @brief   キャッスルBTL：終了動作関数
 *
 * @param   fmap		
 * @param   ptr_work		
 */
//--------------------------------------------------------------
static void EndFunc_CastleBtl( FMAP_PTR fmap, void** ptr_work )
{
	return;
}


