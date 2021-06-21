//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		minigame_tool.h
 *	@brief		ミニゲームツール
 *	@author		tomoya takahashi	
 *	@data		2007.10.22
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __MINIGAME_TOOL_H__
#define __MINIGAME_TOOL_H__

#include "gflib/clact.h"

#include "savedata/mystatus.h"
#include "savedata/savedata_def.h"

#include "application/wifi_lobby/wflby_system.h"

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
// 内部で使用する通信同期定数	（ミニゲームとかぶらないように）
enum{
	MNGM_SYNC_START	= 128,
	MNGM_SYNC_END,			// 全体の終了をあわせます。
	MNGM_SYNC_RESULT_END,	// 結果画面の終了をあわせます。
};

#define MNGM_COUNT_PALNUM		( 3 )	//カウントダウンで使用するパレット本数


// 風千割ガジェットアップデート定数
#define MNGM_BALLOON_GADGET_UPDATE	( 10 )

// Balanceボールで何もしなかったときに入っているscoreの値
#define MNGM_BALANCEBALL_NOTOUCH_SCORE	( 0 )


// Rareゲームタイプ
typedef enum {
	// 玉いれ
	MNGM_RAREGAME_BUCKET_NORMAL	= 0,
	MNGM_RAREGAME_BUCKET_REVERSE,
	MNGM_RAREGAME_BUCKET_BIG,
	MNGM_RAREGAME_BUCKET_NUM,

	// 玉乗り
	MNGM_RAREGAME_BALANCEBALL_NORMAL = 0,
	MNGM_RAREGAME_BALANCEBALL_BIG,
	MNGM_RAREGAME_BALANCEBALL_NUM,

	// 風千割
	MNGM_RAREGAME_BALLOON_NORMAL = 0,
	MNGM_RAREGAME_BALLOON_THICK,	//太
	MNGM_RAREGAME_BALLOON_FINE,		//細
	MNGM_RAREGAME_BALLOON_NUM,
} MNGM_RAREGAME_TYPE;

// Rareゲームのパーセンテージ	MAXが100
// 玉いれ
#define MNGM_RAREPAR_BUCKET_NORMAL		(75)
#define MNGM_RAREPAR_BUCKET_REVERSE		(15)
#define MNGM_RAREPAR_BUCKET_BIG			(10)
// 玉乗り
#define MNGM_RAREPAR_BALANCEBALL_NORMAL	(85)
#define MNGM_RAREPAR_BALANCEBALL_BIG	(15)
// 風千割
#define MNGM_RAREPAR_BALLOON_NORMAL		(85)
#define MNGM_RAREPAR_BALLOON_THICK		(15)
#define MNGM_RAREPAR_BALLOON_FINE		(0)


//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------


//-------------------------------------
///	ミニゲームWiFiロビー情報
//=====================================
typedef struct {
	WFLBY_ITEMTYPE			gadget;
	WFLBY_MINIGAME_PLIDX	plidx;		// プレイヤーインデックス
	WFLBY_VIPFLAG			vipflag;	// VIPフラグ
	BOOL					error_end;	// エラー終了チェック

	// 080627 mystatus取得にも使用
	MYSTATUS*				p_mystate[WFLBY_MINIGAME_MAX];	// 全員のMYSTATUS

	// トピックを生成する必要が出てきたので、もうSYSTEM自体を入れてしまう
	WFLBY_SYSTEM*			p_wflbysystem;	// ただし、TOPICの生成にしか使用しない
} WFLBY_MINIGAME_WK;


//-------------------------------------
///	エントリー、結果画面共通ワーク
//  配列の中身はすべてプレイヤーnoで参照します
//=====================================
typedef struct {
	SAVEDATA*				p_save;		// セーブデータ
	u8	pnetid[WFLBY_MINIGAME_MAX];		// プレイヤーに対応するNETID
	u8	num;							// エントリー人数
	u8	my_playerid;					// 自分のプレイヤーID
	u8	vchat;							// vchat対応か
	u8	replay;							// もう一度遊ぶか質問するか
	WFLBY_MINIGAME_WK*		p_lobby_wk;	// ロビー用情報
} MNGM_ENRES_PARAM;


//-------------------------------------
///	エントリー画面ワーク
//=====================================
typedef struct _MNGM_ENTRYWK MNGM_ENTRYWK;

//-------------------------------------
///	結果画面ワーク
//  配列の中身はすべてプレイヤーnoで参照します
//=====================================
typedef struct _MNGM_RESULTWK MNGM_RESULTWK;
typedef struct {

	//	玉いれ、玉乗り用スコアデータ
	u32 score[ WFLBY_MINIGAME_MAX ];	// 各ゲームでのスコア
	u8	result[ WFLBY_MINIGAME_MAX ];	// プレイヤーNoをINDEXにした順位(0.1.2.3)

	// 風千割用　割った数
	u32 balloon; 

} MNGM_RESULT_PARAM;


//-------------------------------------
///	START	TIMEUP	ワーク
//=====================================
typedef struct _MNGM_COUNTWK MNGM_COUNTWK;




//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	エントリー画面、結果画面共通パラメータツール
//=====================================
extern void MNGM_ENRES_PARAM_Init( MNGM_ENRES_PARAM* p_param, BOOL lobby_flag, SAVEDATA* p_save, BOOL vchat, WFLBY_MINIGAME_WK* p_lobby_wk );
extern u32 MNGM_ENRES_PARAM_GetNetIDtoPlNO( const MNGM_ENRES_PARAM* cp_param, u32 netid );
extern u32 MNGM_ENRES_PARAM_GetPlNOtoNetID( const MNGM_ENRES_PARAM* cp_param, u32 plno );
extern BOOL MNGM_ENRES_PARAM_GetVipFlag( const MNGM_ENRES_PARAM* cp_param, u32 netid );
extern MYSTATUS* MNGM_ENRES_PARAM_GetMystatus( const MNGM_ENRES_PARAM* cp_param, u32 netid );



//-------------------------------------
///	エントリー画面ワーク
//=====================================
extern MNGM_ENTRYWK* MNGM_ENTRY_InitBallSlow( const MNGM_ENRES_PARAM* cp_commparam, u32 heapID );
extern MNGM_ENTRYWK* MNGM_ENTRY_InitBalanceBall( const MNGM_ENRES_PARAM* cp_commparam, u32 heapID );
extern MNGM_ENTRYWK* MNGM_ENTRY_InitBalloon( const MNGM_ENRES_PARAM* cp_commparam, u32 heapID );
extern void MNGM_ENTRY_Exit( MNGM_ENTRYWK* p_wk );
extern BOOL MNGM_ENTRY_Wait( const MNGM_ENTRYWK* cp_wk );
extern MNGM_RAREGAME_TYPE MNGM_ENTRY_GetRareGame( const MNGM_ENTRYWK* cp_wk );

//-------------------------------------
///	結果画面ワーク
//=====================================
extern MNGM_RESULTWK* MNGM_RESULT_InitBallSlow( const MNGM_ENRES_PARAM* cp_commparam, const MNGM_RESULT_PARAM* cp_param,  u32 heapID );
extern MNGM_RESULTWK* MNGM_RESULT_InitBalanceBall( const MNGM_ENRES_PARAM* cp_commparam, const MNGM_RESULT_PARAM* cp_param, u32 heapID );
extern MNGM_RESULTWK* MNGM_RESULT_InitBalloon( const MNGM_ENRES_PARAM* cp_commparam, const MNGM_RESULT_PARAM* cp_param, u32 heapID );
extern void MNGM_RESULT_Exit( MNGM_RESULTWK* p_wk );
extern BOOL MNGM_RESULT_Wait( const MNGM_RESULTWK* cp_wk );
extern BOOL MNGM_RESULT_GetReplay( const MNGM_RESULTWK* cp_wk );

// 結果画面パラメータ生成ツール
// 今後ガジェットパラメータの設定ツールなども追加していく

// 玉投げと玉乗りは、scoreデータをすべて設定してからこの関数を呼んでください。
extern void MNGM_RESULT_CalcRank( MNGM_RESULT_PARAM* p_param, u32 plnum );



//-------------------------------------
///	切断エラーチェック
//	Wi-Fi広場のときにしか動作しません
//=====================================
extern BOOL MNGM_ERROR_CheckDisconnect( MNGM_ENRES_PARAM* p_commparam );
extern BOOL MNGM_ERROR_DisconnectWait( const MNGM_ENRES_PARAM* cp_commparam );



//-------------------------------------
///	START	TIMEUP	ワーク
//	Hブランク使用します。
//=====================================
extern MNGM_COUNTWK* MNGM_COUNT_Init( CLACT_SET_PTR p_clset, u32 heapID );
extern void MNGM_COUNT_Exit( MNGM_COUNTWK* p_wk );
extern void MNGM_COUNT_StartStart( MNGM_COUNTWK* p_wk );
extern void MNGM_COUNT_StartTimeUp( MNGM_COUNTWK* p_wk );
extern BOOL MNGM_COUNT_Wait( const MNGM_COUNTWK* p_wk );
extern int MNGM_PalNoGet( MNGM_COUNTWK* p_wk );

#endif		// __MINIGAME_TOOL_H__

