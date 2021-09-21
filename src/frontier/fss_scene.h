//==============================================================================
/**
 * @file	fss_scene.h
 * @brief	
 * @author	matsuda
 * @date	2007.04.03(火)
 */
//==============================================================================
#ifndef __FSS_SCENE_H__
#define __FSS_SCENE_H__


//==============================================================================
//	定数定義
//==============================================================================

//--------------------------------------------------------------
//	シーンID定義(※SceneDataTblと並びを同じにすること！)
//--------------------------------------------------------------
#define FSS_SCENEID_TESTSCR		(0)
#define FSS_SCENEID_TESTSCR2	(FSS_SCENEID_TESTSCR + 1)
#define FSS_SCENEID_TESTSCR3	(FSS_SCENEID_TESTSCR2 + 1)
//ファクトリー
#define FSS_SCENEID_FACTORY		(FSS_SCENEID_TESTSCR3 + 1)
#define FSS_SCENEID_FACTORY_BTL	(FSS_SCENEID_FACTORY + 1)
//タワー
#define FSS_SCENEID_TOWER_START			(FSS_SCENEID_FACTORY_BTL + 1)		//タワースタート
#define FSS_SCENEID_TOWER_SINGLE_WAY	(FSS_SCENEID_TOWER_START + 0)		//r203
#define FSS_SCENEID_TOWER_MULTI_WAY		(FSS_SCENEID_TOWER_START + 1)		//r204
#define FSS_SCENEID_TOWER_SINGLE_BTL	(FSS_SCENEID_TOWER_START + 2)		//r205
#define FSS_SCENEID_TOWER_MULTI_BTL		(FSS_SCENEID_TOWER_START + 3)		//r206
//ステージ
#define FSS_SCENEID_STAGE_START			(FSS_SCENEID_TOWER_MULTI_BTL + 1)	//ステージスタート
#define FSS_SCENEID_STAGE_WAY			(FSS_SCENEID_STAGE_START + 0)		//
#define FSS_SCENEID_STAGE_BTL			(FSS_SCENEID_STAGE_START + 1)		//
//キャッスル
#define FSS_SCENEID_CASTLE_START		(FSS_SCENEID_STAGE_BTL + 1)			//キャッスルスタート
#define FSS_SCENEID_CASTLE_ROOM			(FSS_SCENEID_CASTLE_START + 0)		//
#define FSS_SCENEID_CASTLE_BTL			(FSS_SCENEID_CASTLE_START + 1)		//
#define FSS_SCENEID_CASTLE_BYPATH		(FSS_SCENEID_CASTLE_START + 2)		//
//WiFi受付
#define FSS_SCENEID_WIFI_COUNTER		(FSS_SCENEID_CASTLE_BYPATH + 1)		//
//ルーレット
#define FSS_SCENEID_ROULETTE_START		(FSS_SCENEID_WIFI_COUNTER + 1)		//ルーレットスタート
#define FSS_SCENEID_ROULETTE_WAY		(FSS_SCENEID_ROULETTE_START + 0)	//
#define FSS_SCENEID_ROULETTE_BTL		(FSS_SCENEID_ROULETTE_START + 1)	//

#define FSS_SCENEID_END			(0xfffe)
#define FSS_SCENEID_DEFAULT		(0xffff)	//デフォルトID


//--------------------------------------------------------------
//	
//--------------------------------------------------------------
///シーンデータに多重面を使用しない場合に指定
#define MULTI_BG_NULL		(0xffff)


//==============================================================================
//
//	
//
//==============================================================================
#ifndef	__ASM_NO_DEF_	// ↓これ以降は、アセンブラでは無視

//--------------------------------------------------------------
//	シーンデータ取得コード
//--------------------------------------------------------------
enum{
	FSS_SCENE_DATA_BG_MODE,		//※check 一時的なもの。全てのグラフィックが移行出来たら消す
	FSS_SCENE_DATA_SCRID,
	FSS_SCENE_DATA_MSGID,
	FSS_SCENE_DATA_BGMID,
	FSS_SCENE_DATA_SCREEN_SIZE,
	FSS_SCENE_DATA_MAP_ARCID,
	FSS_SCENE_DATA_MAP_SCREENID,
	FSS_SCENE_DATA_MAP_CHARID,
	FSS_SCENE_DATA_MAP_PLTTID,
	FSS_SCENE_DATA_MULTI_MAP_SCREENID,
	FSS_SCENE_DATA_MULTI_MAP_CHARID,
	FSS_SCENE_DATA_MULTI_MAP_PLTTID,
	FSS_SCENE_DATA_SCROLL_MODE,
	FSS_SCENE_DATA_MULTI_SCROLL_MODE,
};

///マップのスクロール動作モード
enum{
	SCROLL_MODE_NORMAL,		///<通常スクロール処理(WF2DMAPシステム使用)
	SCROLL_MODE_EASY,		///<BG座標のスクロール処理のみ(スクリーン転送アニメを使用する場合)
};

//==============================================================================
//	外部関数宣言
//==============================================================================
extern int FSS_SceneParamGet(int scene_id, int param_code);
extern void FSS_SceneInitFuncCall(FMAP_PTR fmap, void **ptr_work, int scene_id);
extern void FSS_SceneEndFuncCall(FMAP_PTR fmap, void **ptr_work, int scene_id);

#endif	//__ASM_NO_DEF_


#endif	//__FSS_SCENE_H__

