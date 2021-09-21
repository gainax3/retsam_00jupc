//==============================================================================
/**
 * @file	frontier_main.h
 * @brief	フロンティアメイン処理のヘッダ
 * @author	matsuda
 * @date	2007.04.05(木)
 */
//==============================================================================
#ifndef __FRONTIER_MAIN_H__
#define __FRONTIER_MAIN_H__

#include "savedata/config.h"
#include "system/procsys.h"
#include "system/mystatus.h"
#include "savedata/savedata_def.h"
#include "savedata/fnote_mem.h"
#include "itemtool/myitem.h"
#include "battle/battle_cursor_disp.h"

#include "application/wifi_2dmap/wf2dmap_obj.h"
#include "application/wifi_2dmap/wf2dmap_judge.h"
#include "application/wifi_2dmap/wf2dmap_scrdraw.h"
#include "application/wifi_2dmap/wf2dmap_objdraw.h"
#include "application/wifi_2dmap/wf2dmap_cmdq.h"

//仮！！！
#include "field/field.h"

//==============================================================================
//	型定義
//==============================================================================
typedef void (*FS_SUBPROC_END_FUNC)(void *);


//==============================================================================
//	定数定義
//==============================================================================
///マップ切り替え時、イベントID指定なし
#define FS_MAPCHANGE_EVENTID_NULL		(0xffff)


//==============================================================================
//	構造体定義
//==============================================================================
///フロンティア呼び出し時に外側から渡されるシステム関連データ
typedef struct{
	void *syswork;				///<アプリケーション側で保持して欲しいワークのポインタを渡す
	
	const CONFIG *config;		///<コンフィグデータへのポインタ
	SAVEDATA *savedata;			///<セーブデータへのポインタ
	
	FNOTE_DATA	*fnote_data;	///<冒険ノートワーク
	BAG_CURSOR	*bag_cursor;	///<バッグのカーソルデータ
	int	bg_id;					///<背景ID
	int	ground_id;				///<地形ID
	int	zone_id;				///<ゾーンID
	BATTLE_CURSOR_DISP	*battle_cursor;	///<戦闘カーソル初期表示状態
	
	u8 scene_id;				///<シーンID

	//仮！！！
	FIELDSYS_WORK* fsys;		///<WiFi受付から呼ばれる　ポケモンリストに必要
	//POKE_PARTY 
	//SAVE
	//
}FRONTIER_EX_PARAM;


//--------------------------------------------------------------
//	
//--------------------------------------------------------------
///キャラクタリソース登録用データ構造体
typedef struct{
	u16 charid;
	u8 movetype;
}FSS_CHAR_RESOURCE_DATA;

///OBJ動作用ワークが持つ汎用ワークの最大数
#define FSS_OBJMOVE_WORK_MAX		(7)

///OBJ動作用ワーク構造体
typedef struct{
	u8 move_id;		///<OBJ動作コード
	u8 seq;			///<汎用シーケンス番号
	s16 work[FSS_OBJMOVE_WORK_MAX];	///<汎用ワーク
}FSS_OBJMOVE_WORK;

///キャラクタアクター登録用データ構造体
typedef struct{
	u16 charid;		///<キャラクターID
	u16 status;		///<初期化状態(WF2DMAP_OBJST)
	u16 playid;		///<プレイヤー識別ID
	s16 x;			///<X座標
	s16 y;			///<Y座標
	u8 way;			///<方向(WF2DMAP_WAY)

	u8 visible;		///<TRUE:表示、FALSE:非表示
	u8 event_id;	///<対応するイベントID
	
	//OBJ動作コード用ワーク
	FSS_OBJMOVE_WORK objmove;	///<OBJ動作コード用ワーク

}FSS_CHAR_ACTOR_DATA;

///アクターのOBJパラメータ
///(現状FSS_CHAR_ACTOR_DATAと同じ。どちらかを変更する場合は、
/// Frontier_ActWork_to_ActData関数、Frontier_ActWorkDataSet関数の
/// 変更などにも注意すること)
typedef FSS_CHAR_ACTOR_DATA FSS_OBJPARAM;

///アクター動作用ワーク(PUSH時の情報退避先としても使用される
typedef struct{
	//param.playidから引っ張れるので持つ必要はないけれど、サーチをなくせるので持たせてます
	//objwkがNULLかどうかで使用・未使用判定にも使用しています
	WF2DMAP_OBJWK *objwk;	///<オブジェクトワーク
	WF2DMAP_OBJDRAWWK *drawwk;	///<描画オブジェクトワーク

	FSS_OBJPARAM param;			///<OBJパラメータ
	FSS_OBJMOVE_WORK move;		///<OBJ動作ワーク
	TCB_PTR anime_tcb;	///<スクリプトアニメ実行中の場合、制御TCBのポインタが入る
}FSS_ACTOR_WORK;


//==============================================================================
//	外部関数宣言
//==============================================================================
extern FRONTIER_EX_PARAM * Frontier_ExParamGet(FMAIN_PTR fmain);
extern FMAP_PTR Frontier_FMapAdrsGet(FMAIN_PTR fmain);
extern FSS_PTR Frontier_FSSAdrsGet(FMAIN_PTR fmain);
extern void Frontier_SubProcSet(FMAIN_PTR fmain, const PROC_DATA * proc_data, void * parent_work, int auto_free, FS_SUBPROC_END_FUNC end_func);
extern void Frontier_FinishReq(FMAIN_PTR fmain);
extern void * Frontier_SysWorkGet(FMAIN_PTR fmain);
extern void Frontier_SysWorkSet(FMAIN_PTR fmain, void *syswork);
extern FSS_CHAR_RESOURCE_DATA * Frontier_CharResourceBufferGet(FMAIN_PTR fmain);
extern FSS_ACTOR_WORK * Frontier_ActorBufferGet(FMAIN_PTR fmain);
extern FSS_ACTOR_WORK * Frontier_ActorWorkPtrGet(FMAIN_PTR fmain, int actor_no);
extern void Frontier_MapChangeReq(FMAIN_PTR fmain, u16 scene_id, u16 event_id);
extern PUSH_ARTICLE_WORK * Frontier_ArticlePushBufferGet(FMAIN_PTR fmain);
extern void Frontier_ArticlePushBufferInit(FMAIN_PTR fmain);

//==============================================================================
//	データ
//==============================================================================
extern const PROC_DATA FrontierMainProcData;


#endif	//__FRONTIER_MAIN_H__

