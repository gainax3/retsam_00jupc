//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wifi_matchroom.c
 *	@brief		wifi	マッチングルーム
 *	@author		tomoya takahashi
 *	@data		2007.01.31
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "common.h"

#include "system/procsys.h"
#include "system/arc_util.h"
#include "system/clact_tool.h"
#include "system/wipe.h"
#include "system/lib_pack.h"
#include "system/snd_tool.h"

#include "wifi/dwc_rap.h"
#include "wifi_p2pmatch_se.h"
#include "wifi/dwc_rapfriend.h"

#include "communication/communication.h"
#include "communication/comm_state.h"
#include "system/snd_tool.h"  //sndTOOL

#include "wifip2pmatch.naix"			// グラフィックアーカイブ定義

#include "include/system/pm_debug_wifi.h"

#include "application/wifi_p2pmatch_def.h"

#define __WIFI_P2PMATCHROOM_H_GLOBAL
#include "wifi_p2pmatchroom.h"


//-----------------------------------------------------------------------------
/**
 *					コーディング規約
 *		●関数名
 *				１文字目は大文字それ以降は小文字にする
 *		●変数名
 *				・変数共通
 *						constには c_ を付ける
 *						staticには s_ を付ける
 *						ポインタには p_ を付ける
 *						全て合わさると csp_ となる
 *				・グローバル変数
 *						１文字目は大文字
 *				・関数内変数
 *						小文字と”＿”と数字を使用する 関数の引数もこれと同じ
*/
//-----------------------------------------------------------------------------
#ifdef PM_DEBUG
//#define MCR_DEBUG_2CCHAR_CHECK	// 2DMAPSYSTEMの表示チェック用
#endif

#ifdef MCR_DEBUG_2CCHAR_CHECK
static u32 s_MCR_DEBUG_2CCHAR_VIEW_ID;
static u32 s_MCR_DEBUG_2CCHAR_VIEW_COUNT = 0;
#endif



//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
// 登録位置データ
#define MCR_MOVEOBJ_ADD_POSNUM	(52)

// リクエストコマンドキュー
#define MCR_REQCMDQ_NUM	( 128 )


#define MCR_MAP_GRID_SIZE	(WF2DMAP_GRID_SIZ)	// マップ１グリッドサイズ


// 動作状態
enum{
	MCR_MOVEOBJ_ST_PL_KEYWAIT,		// プレイヤー通常動作
	MCR_MOVEOBJ_ST_KURUKURU,		// 落ちる処理
	MCR_MOVEOBJ_ST_KURUKURU_DEL,	// 上る処理
	MCR_MOVEOBJ_ST_NPC,				// NPC動作
	MCR_MOVEOBJ_ST_NPC_JUMP,		// NPC募集中
	MCR_MOVEOBJ_ST_NPC_PAUSE,		// NPC動作停止
	MCR_MOVEOBJ_ST_NUM,
};


enum{	// 方向
	MCR_MOVEOBJ_WAY_TOP = WF2DMAP_WAY_UP,
	MCR_MOVEOBJ_WAY_BOTTOM = WF2DMAP_WAY_DOWN,
	MCR_MOVEOBJ_WAY_LEFT = WF2DMAP_WAY_LEFT,
	MCR_MOVEOBJ_WAY_RIGHT = WF2DMAP_WAY_RIGHT,
	MCR_MOVEOBJ_WAY_NUM
};
#define MCR_MOVEOBJ_BG_PRI	( 3 )
#define MCR_MOVEOBJ_PLAYER_FRIENDNO	(0)

#define MCR_MOVEOBJ_WALK_COUNT		(8)	// 歩きシンク数
#define MCR_MOVEOBJ_KURU_COUNT		(45)// くるくるシンク数
#define MCR_MOVEOBJ_NPC_RAND_S		(256)// NPC方向変えるタイミング最小
#define MCR_MOVEOBJ_NPC_RAND_M		(1024)// NPC方向変えるタイミング最大

#define MCR_MOVEOBJ_WALK_DIST		( MCR_MAP_GRID_SIZE/MCR_MOVEOBJ_WALK_COUNT )	// 1シンクに動く座標
#define MCR_MOVEOBJ_KURU_DIST		( -192 )
#define MCR_MOVEOBJ_NPC_JUMP_EFFCOUNT	( 6 )// ジャンプしている時間
#define MCR_MOVEOBJ_NPC_JUMP_WAITCOUNT	( 16 )// 次のジャンプまでのウエイト
#define MCR_MOVEOBJ_NPC_JUMP_COUNT		( (MCR_MOVEOBJ_NPC_JUMP_EFFCOUNT*2)+MCR_MOVEOBJ_NPC_JUMP_WAITCOUNT )// １ジャンプカウント
#define MCR_MOVEOBJ_NPC_JUMP_DIS	( 4 )// ジャンプの高さ
#define MCR_MOVEOBJ_NPC_JUMP_RMAX	(180)// 回転弧MAX


// エフェクトリソース
#define MCR_EFFECTRES_BGPRI		(2)	// BG優先順位
#define MCR_EFFECTRES_SOFTPRI	(0)// BG優先順位
#define MCR_EFFECTRES_OFS_Y		(32)// Y座標補正地
#define MCR_EFFECTRES_OFS_X		(8)// Y座標補正地
#define MCR_EFFECTWAKURES_OFS_Y		(0)// Y座標補正地
#define MCR_EFFECTWAKURES_OFS_X		(8)// Y座標補正地

// リソース管理ID
#define MCR_EFFECTRES_CONTID	( 50 )	// エフェクトリソース管理ID


// PCアニメーション
enum{
	// 待機中のアニメ
	MCR_PCANM_ALL_SEQ_OFF,
	MCR_PCANM_ALL_SEQ_ON,
	MCR_PCANM_ALL_SEQ_NUM,

	// 実行中のアニメ
	MCR_PCANM_USE_SEQ_NONE = 0,
	MCR_PCANM_USE_SEQ_ON00,
	MCR_PCANM_USE_SEQ_OFF01,
	MCR_PCANM_USE_SEQ_ON01,
	MCR_PCANM_USE_SEQ_WAIT,
	MCR_PCANM_USE_SEQ_OFF02,
	MCR_PCANM_USE_SEQ_ON02,
	MCR_PCANM_USE_SEQ_OFF03,
	MCR_PCANM_USE_SEQ_NUM,

	// カラータイプ
	MCR_PCANM_COL_ON = 0,
	MCR_PCANM_COL_OFF,
	MCR_PCANM_COL_WAIT,
};
#define MCR_PCANM_BGPLTT	( 7 )	// アニメさせるパレット
#define MCR_PCANM_BGPLTTOFS	(1)		// アニメさせるパレットの開始オフセット
#define MCR_PCANM_PCNUM			(4)	// PCの数
#define MCR_PCANM_DESTPL(x)	( (MCR_PCANM_BGPLTT*32) + (((x)+MCR_PCANM_BGPLTTOFS)*2) )	// パレット転送先アドレス取得

//-------------------------------------
///	アニメデータ
//=====================================
typedef struct {
	u8 flag;		// カウント有無
	u8 count_num;	// カウント値
	u8 trans;		// 転送有無
	u8 trans_flag;	// 転送フラグ
} MCR_PCANM_DATA;
static const MCR_PCANM_DATA AnmData[ MCR_PCANM_USE_SEQ_NUM ] = {
	{ FALSE,	0,	FALSE,	MCR_PCANM_COL_OFF },
	{ TRUE,		2, TRUE,	MCR_PCANM_COL_ON },
	{ TRUE,		4, TRUE,	MCR_PCANM_COL_OFF },
	{ TRUE,		2, TRUE,	MCR_PCANM_COL_ON },
	{ FALSE,	0,	FALSE,	MCR_PCANM_COL_OFF },
	{ TRUE,		2, TRUE,	MCR_PCANM_COL_OFF },
	{ TRUE,		4, TRUE,	MCR_PCANM_COL_ON },
	{ TRUE,		3, TRUE,	MCR_PCANM_COL_OFF },
};

static const u16 AllAnmData[ MCR_PCANM_ALL_SEQ_NUM ] = {
	3,
	2,
};



//-----------------------------------------------------------------------------
/**
 *		マクロ
 */
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	動作オブジェクト登録位置
//=====================================
typedef struct {
	s16	x;
	s16	y;
} MCR_MOVEOBJ_ONPOS;


//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------

static void WcrMoveObjAllDel( WIFI_MATCHROOM* p_mcr );

static BOOL WcrMoveObjKeyInputCheck( const WIFI_MATCHROOM* cp_mcr );
static BOOL WcrMoveObjPCSelectCheck( const WIFI_MATCHROOM* cp_mcr );
static void WcrMoveObjCmdJudgeAndCmdMove( WIFI_MATCHROOM* p_mcr );
static void WcrMoveObjCmdReq( WIFI_MATCHROOM* p_mcr );
static void WcrMoveObjDraw( WIFI_MATCHROOM* p_mcr );
static const MCR_MOVEOBJ* WcrMoveObjGetHitCheck( const WIFI_MATCHROOM* cp_mcr, const MCR_MOVEOBJ* cp_obj, WF2DMAP_WAY way );


static void WcrObjDrawInit( WIFI_MATCHROOM* p_mcr, u32 hero_view, u32 heapID );
static void WcrObjDrawExit( WIFI_MATCHROOM* p_mcr );

static void WcrScrnDrawInit( WIFI_MATCHROOM* p_mcr, u32 heapID, ARCHANDLE* p_handle, u32 map_no );
static void WcrScrnDrawExit( WIFI_MATCHROOM* p_mcr );

static void WcrClactInit( MCR_CLACT* p_clact, u32 heapID, ARCHANDLE* p_handle );
static void WcrClactDest( MCR_CLACT* p_clact );
static void WcrClactResLoad( MCR_CLACT* p_clact, u32 heapID, ARCHANDLE* p_handle );
static void WcrClactResRelease( MCR_CLACT* p_clact );
static void WcrClactAdd( MCR_CLACT* p_clact, u32 heapID );
static void WcrClactDel( MCR_CLACT* p_clact );
static void WcrBgContInit( GF_BGL_INI* p_bgl, u32 heapID );
static void WcrBgContDest( GF_BGL_INI* p_bgl );
static void WcrBgSet( GF_BGL_INI* p_bgl, u32 heapID, ARCHANDLE* p_handle );

static BOOL WcrExitCheck( WIFI_MATCHROOM* p_mcr );

static void WcrClactResEffectLoad( MCR_CLACT* p_clact, u32 heapID, ARCHANDLE* p_handle );
static void WcrClactResEffectRelease( MCR_CLACT* p_clact );
static void WcrClactResEffectAdd( MCR_CLACT* p_clact, u32 heapID );
static void WcrClactResEffectDel( MCR_CLACT* p_clact );
static void WcrClactResEffectExitDrawOn( WIFI_MATCHROOM* p_mcr );
static void WcrClactResEffectExitDrawOff( WIFI_MATCHROOM* p_mcr );
static void WcrClactResEffectCursorDrawOn( WIFI_MATCHROOM* p_mcr, WF2DMAP_POS pos, u32 pri );
static void WcrClactResEffectCursorDrawOff( WIFI_MATCHROOM* p_mcr );


static void WcrMapGetNpcSetPos( WIFI_MATCHROOM* p_mcr, MCR_MOVEOBJ_ONPOS* p_pos, u32 friendNo );
static void WcrMapGetPlayerSetPos( WIFI_MATCHROOM* p_mcr, MCR_MOVEOBJ_ONPOS* p_pos );

static MCR_MOVEOBJ* WcrMoveObj_GetClean( WIFI_MATCHROOM* p_mcr );
static void WcrMoveObj_SetUpGraphic( WIFI_MATCHROOM* p_mcr, MCR_MOVEOBJ* p_obj, BOOL hero );
static void WcrMoveObj_SetUpMove( WIFI_MATCHROOM* p_mcr, MCR_MOVEOBJ* p_obj, const MCR_MOVEOBJ_ONPOS* cp_pos, u32 friendNo, u16 charaid, u8 way, WF2DMAP_OBJST status );
static BOOL WcrMoveObj_CheckAccess( const MCR_MOVEOBJ* cp_obj );

static void WcrMoveObj_SetMoveFuncPlayer( WIFI_MATCHROOM* p_mcr, MCR_MOVEOBJ* p_obj );
static void WcrMoveObj_SetMoveFuncKuruKuruInit( WIFI_MATCHROOM* p_mcr, MCR_MOVEOBJ* p_obj );
static void WcrMoveObj_SetMoveFuncNpc( WIFI_MATCHROOM* p_mcr, MCR_MOVEOBJ* p_obj );
static void WcrMoveObj_SetMoveFuncNpcJump( WIFI_MATCHROOM* p_mcr, MCR_MOVEOBJ* p_obj );
static u32 WcrMoveObj_GetRetWay( u32 way );

static void WcrMoveObj_DrawFuncDefault( WIFI_MATCHROOM* p_mcr, MCR_MOVEOBJ* p_obj );
static void WcrMoveObj_DrawFuncKuruKuru( WIFI_MATCHROOM* p_mcr, MCR_MOVEOBJ* p_obj );
static void WcrMoveObj_DrawFuncNpcJump( WIFI_MATCHROOM* p_mcr, MCR_MOVEOBJ* p_obj );

static BOOL WcrMoveObj_MoveFuncPlayerKeyWait( WIFI_MATCHROOM* p_mcr, MCR_MOVEOBJ* p_obj );
static BOOL WcrMoveObj_MoveFuncKuruKuru( WIFI_MATCHROOM* p_mcr, MCR_MOVEOBJ* p_obj );
static BOOL WcrMoveObj_MoveFuncKuruKuruDel( WIFI_MATCHROOM* p_mcr, MCR_MOVEOBJ* p_obj );
static BOOL WcrMoveObj_MoveFuncNpc( WIFI_MATCHROOM* p_mcr, MCR_MOVEOBJ* p_obj );
static BOOL WcrMoveObj_MoveFuncNpcJump( WIFI_MATCHROOM* p_mcr, MCR_MOVEOBJ* p_obj );
static BOOL WcrMoveObj_MoveFuncNpcPause( WIFI_MATCHROOM* p_mcr, MCR_MOVEOBJ* p_obj );

static void WcrMoveObj_ReqCmdSet( WIFI_MATCHROOM* p_mcr, s32 cmd, s32 way, s32 playid );
static void WcrMoveObj_ReqCmdSetEasy( WIFI_MATCHROOM* p_mcr, s32 cmd, const MCR_MOVEOBJ* cp_obj );

static BOOL WcrMoveObj_MoveSetOkCheck( const MCR_MOVEOBJ* cp_obj );
static void WcrMoveObj_MoveSet( WIFI_MATCHROOM* p_mcr, MCR_MOVEOBJ* p_obj, MCR_NPC_MOVETYPE moveID );


static void WcrPCANM_Init( WIFI_MATCHROOM* p_sys, MCR_PCANM* p_wk, ARCHANDLE* p_handle );
static void WcrPCANM_Delete( WIFI_MATCHROOM* p_sys, MCR_PCANM* p_wk );
static void WcrPCANM_Main( WIFI_MATCHROOM* p_sys, MCR_PCANM* p_wk );
static void WcrPCANM_AllMain( MCR_PCANM* p_wk );
static void WcrPCANM_UseMain( MCR_PCANM* p_wk );
static void* WcrPCANM_GetAnmSrc( MCR_PCANM* p_wk, u32 on_off );
static void WcrPCANM_UseAnmEnd( MCR_PCANM* p_wk );
static void WcrPCANM_UseAnmNext( MCR_PCANM* p_wk );
static void WcrPCANM_UseStart( MCR_PCANM* p_wk, u8 pc_no );
static void WcrPCANM_UseEndReq( MCR_PCANM* p_wk );

//----------------------------------------------------------------------------
/**
 *	@brief	WIFI　マッチングルーム　初期化	ワーク＆リソース読み込みも行う
 *
 *	@param	p_mcr		ワーク
 *	@param	heapID		ヒープID
 *	@param	p_handle	アーカイブハンドル
 *	@param	p_bgl		BGL
 *	@param	hero_view	主人公の性別
 *	@param	friendNum	友達の総数
 */
//-----------------------------------------------------------------------------
void WIFI_MCR_Init( WIFI_MATCHROOM* p_mcr, u32 heapID, ARCHANDLE* p_handle, GF_BGL_INI* p_bgl, u32 hero_view, u32 friendNum )
{
	WF2DMAP_POS map_siz;
	u32 map_no;
	
	memset( p_mcr, 0, sizeof(WIFI_MATCHROOM) );

	map_no = (friendNum-1) / WCR_MAPDATA_1BLOCKOBJNUM;

	p_mcr->use_heap = heapID;
	p_mcr->p_bgl	= p_bgl;
	
	// CLACT INIT
	WcrClactInit( &p_mcr->clact, p_mcr->use_heap, p_handle );

	// BGL　初期化
	WcrBgContInit( p_mcr->p_bgl, heapID );

	// マップデータ初期化
	map_siz = WcrMapGridSizGet( map_no );
	p_mcr->p_mapsys = WF2DMAP_MAPSysInit( map_siz.x, map_siz.y, heapID );
	WF2DMAP_MAPSysDataSet( p_mcr->p_mapsys, WcrMapDataGet( map_no ) );

	// オブジェクト管理システム作成
	p_mcr->p_objsys = WF2DMAP_OBJSysInit( MCR_MOVEOBJNUM, heapID );

	// オブジェクト表示システム生成
	WcrObjDrawInit( p_mcr, hero_view, heapID );

	// スクロールデータ初期化
	WF2DMAP_SCROLLSysDataInit( &p_mcr->scroll );

	// スクロール描画システム初期化
	WcrScrnDrawInit( p_mcr, heapID, p_handle, map_no );
	WcrBgSet( p_mcr->p_bgl, heapID, p_handle );

	// PCあにめ初期化
	WcrPCANM_Init( p_mcr, &p_mcr->pc_anm, p_handle );

	// リクエストコマンドキュー生成
	p_mcr->p_reqcmdQ = WF2DMAP_REQCMDQSysInit( MCR_REQCMDQ_NUM, heapID );
	
	// 初期化完了
	p_mcr->init = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	WIFI	マッチングルーム　破棄　ワーク＆リソース破棄
 *
 *	@param	p_mcr	ワーク
 */
//-----------------------------------------------------------------------------
void WIFI_MCR_Dest( WIFI_MATCHROOM* p_mcr )
{
	// リクエストコマンドキュー破棄
	WF2DMAP_REQCMDQSysExit( p_mcr->p_reqcmdQ );
	
	// スクロール描画システムはき
	WcrScrnDrawExit( p_mcr );

	// PCあにめ破棄
	WcrPCANM_Delete( p_mcr, &p_mcr->pc_anm );

	// 全動作オブジェクト破棄
	WcrMoveObjAllDel( p_mcr );

	// オブジェクト表示システム破棄
	WcrObjDrawExit( p_mcr );

	// オブジェクトシステムはき
	WF2DMAP_OBJSysExit( p_mcr->p_objsys );

	// マップデータはき
	WF2DMAP_MAPSysExit( p_mcr->p_mapsys );

	// セルアクター破棄
	WcrClactDest( &p_mcr->clact );

	// BGL破棄
	WcrBgContDest( p_mcr->p_bgl );

	memset( p_mcr, 0, sizeof(WIFI_MATCHROOM) );
}

//----------------------------------------------------------------------------
/**
 *	@brief
 *
 *	@param	const WIFI_MATCHROOM* cp_mcr 
 *
 *	@return
 */
//-----------------------------------------------------------------------------
BOOL WIFI_MCR_GetInitFlag( const WIFI_MATCHROOM* cp_mcr )
{
	return cp_mcr->init;
}


//----------------------------------------------------------------------------
/**
 *	@brief	メイン処理
 *
 *	@param	p_mcr	ワーク
 *
 *	@retval	MCR_RET_NONE,		// なし
 *	@retval	MCR_RET_CANCEL,		// キャンセル
 *	@retval	MCR_RET_SELECT,		// 選択
 *	@retval MCR_RET_MYSELECT,	// 自分選択
 */
//-----------------------------------------------------------------------------
u32 WIFI_MCR_Main( WIFI_MATCHROOM* p_mcr )
{
	BOOL result;

	// オブジェクト動作
	WF2DMAP_OBJSysMain( p_mcr->p_objsys );

	// コマンド発行処理
	WcrMoveObjCmdReq( p_mcr );

	// コマンド判断処理＆コマンド実行処理
	WcrMoveObjCmdJudgeAndCmdMove( p_mcr );
	
	// スクロール処理
	WF2DMAP_SCRContSysMain( &p_mcr->scroll, p_mcr->p_player->p_obj );	
	
	// スクロール表示処理
	WF2DMAP_SCRDrawSysMain( p_mcr->p_scrdraw, &p_mcr->scroll );	

	// オブジェクト表示データ更新処理
	WF2DMAP_OBJDrawSysUpdata( p_mcr->p_objdraw );

	// イベントオブジェクト表示データ更新処理
	WcrMoveObjDraw( p_mcr );

	// チェック前に出口を消す
	WcrClactResEffectExitDrawOff( p_mcr );

	// キー入力処理
	if( WcrMoveObjKeyInputCheck( p_mcr ) == TRUE ){

		// 出口マットの上に立っているかチェック
		result = WcrExitCheck( p_mcr );

		if( result == TRUE ){

			// マットの上で下を向いているので、矢印を出す
			WcrClactResEffectExitDrawOn( p_mcr );

			// さらに下を押したら、部屋を出る
			if( sys.cont & PAD_KEY_DOWN ){
				return MCR_RET_CANCEL;
			}
		}
		if( sys.trg & PAD_BUTTON_DECIDE ){
			// 他人を選択したかチェック
			if( WIFI_MCR_PlayerSelect( p_mcr ) > 0 ){
				return MCR_RET_SELECT;

			}else if( WcrMoveObjPCSelectCheck( p_mcr ) == TRUE ){

				// 目の前ならPCならリストを出す
				return MCR_RET_MYSELECT;
			}
		}
	}
	
	return MCR_RET_NONE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	表示
 *
 *	@param	p_mcr	ワーク
 */
//-----------------------------------------------------------------------------
void WIFI_MCR_Draw( WIFI_MATCHROOM* p_mcr )
{
	if( p_mcr->init ){
		CLACT_Draw( p_mcr->clact.clactSet );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	選択中の友達コードを取得する
 *
 *	@param	cp_mcr	ワーク
 */
//-----------------------------------------------------------------------------
u8 WIFI_MCR_PlayerSelect( const WIFI_MATCHROOM* cp_mcr )
{
	GF_ASSERT( cp_mcr->p_player != NULL );	// 主人公がいるのか？
	return cp_mcr->p_player->accesFriend;
}

//----------------------------------------------------------------------------
/**
 *	@brief	主人公の乗っているマップデータ取得
 *
 *	@param	cp_mcr	ワーク
 *
 *	@return	マップパラメータ
 */
//-----------------------------------------------------------------------------
u32 WIFI_MCR_GetPlayerOnMapParam( const WIFI_MATCHROOM* cp_mcr )
{
	WF2DMAP_POS pos;
	
	GF_ASSERT( cp_mcr->p_player != NULL );	// 主人公がいるのか？

	pos = WF2DMAP_OBJWkMatrixGet( cp_mcr->p_player->p_obj );
	return WF2DMAP_MAPSysParamGet( cp_mcr->p_mapsys, WF2DMAP_POS2GRID(pos.x), WF2DMAP_POS2GRID(pos.y) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	主人公の乗っている下のマップデータ取得
 *
 *	@param	cp_mcr	ワーク
 *
 *	@return	マップパラメータ
 */
//-----------------------------------------------------------------------------
u32 WIFI_MCR_GetPlayerOnUnderMapParam( const WIFI_MATCHROOM* cp_mcr )
{
	WF2DMAP_POS pos;
	
	GF_ASSERT( cp_mcr->p_player != NULL );	// 主人公がいるのか？

	pos = WF2DMAP_OBJWkMatrixGet( cp_mcr->p_player->p_obj );
	return WF2DMAP_MAPSysParamGet( cp_mcr->p_mapsys, WF2DMAP_POS2GRID(pos.x), WF2DMAP_POS2GRID(pos.y)+1 );
}

//----------------------------------------------------------------------------
/**
 *	@brief	カーソル表示
 *
 *	@param	p_mcr			システムワーク
 *	@param	cp_obj			オブジェクトワーク
 */
//-----------------------------------------------------------------------------
void WIFI_MCR_CursorOn( WIFI_MATCHROOM* p_mcr, const MCR_MOVEOBJ* cp_obj )
{
	WF2DMAP_POS pos;
	u32 pri;
	pos = WF2DMAP_OBJWkMatrixGet( cp_obj->p_obj );
	pri = WF2DMAP_OBJDrawWkDrawPriGet( cp_obj->p_draw );
	pri --;
	WcrClactResEffectCursorDrawOn( p_mcr, pos, pri-1 );
}

//----------------------------------------------------------------------------
/**
 *	@brief	カーソル非表示
 *
 *	@param	p_mcr 
 */
//-----------------------------------------------------------------------------
void WIFI_MCR_CursorOff( WIFI_MATCHROOM* p_mcr )
{
	WcrClactResEffectCursorDrawOff( p_mcr );
}

//----------------------------------------------------------------------------
/**
 *	@brief	プレイヤーの動作を停止設定する
 *
 *	@param	p_mcr		マッチングルームデータ
 *	@param	flag 
 */
//-----------------------------------------------------------------------------
void WIFI_MCR_PlayerMovePause( WIFI_MATCHROOM* p_mcr, BOOL flag )
{
	p_mcr->player_pause = flag;
}

//----------------------------------------------------------------------------
/**
 *	@brief	プレイヤーの動作停止フラグを取得
 *
 *	@param	cp_mcr		マッチングルームデータ
 *
 *	@retval	TRUE	停止中
 *	@retval	FALSE	実行中
 */
//-----------------------------------------------------------------------------
BOOL WIFI_MCR_PlayerMovePauseGet( const WIFI_MATCHROOM* cp_mcr )
{
	return cp_mcr->player_pause;
}


//----------------------------------------------------------------------------
/**
 *	@brief	主人公登録
 *
 *	@param	p_mcr		ワーク
 *	@param	view		姿
 *
 *	@retval	登録したオブジェクトポインタ
 *	@retval	NULL もう登録できない
 */
//-----------------------------------------------------------------------------
MCR_MOVEOBJ* WIFI_MCR_SetPlayer( WIFI_MATCHROOM* p_mcr, u32 view )
{
	MCR_MOVEOBJ* p_obj;
	MCR_MOVEOBJ_ONPOS pos;

	// 空いているオブジェ取得
	p_obj = WcrMoveObj_GetClean( p_mcr );
	
	// 主人公は取っておく
	p_mcr->p_player = p_obj;

	WcrMapGetPlayerSetPos( p_mcr, &pos );

#ifdef MCR_DEBUG_2CCHAR_CHECK
	// 動作設定
	WcrMoveObj_SetUpMove( p_mcr, p_obj, &pos, 
			MCR_MOVEOBJ_PLAYER_FRIENDNO, AMBRELLA, MCR_MOVEOBJ_WAY_TOP, WF2DMAP_OBJST_NONE );
	s_MCR_DEBUG_2CCHAR_VIEW_ID = AMBRELLA;
#else
	// 動作設定
	WcrMoveObj_SetUpMove( p_mcr, p_obj, &pos, 
			MCR_MOVEOBJ_PLAYER_FRIENDNO, view, MCR_MOVEOBJ_WAY_TOP, WF2DMAP_OBJST_NONE );
#endif

	// プレイヤー動作設定
	WcrMoveObj_SetMoveFuncPlayer( p_mcr, p_obj );

	// グラフィック設定
	WcrMoveObj_SetUpGraphic( p_mcr, p_obj, TRUE );

	// プレイヤーが登録されたら一度スクロールをあわせる
	// スクロール処理
	WF2DMAP_SCRContSysMain( &p_mcr->scroll, p_mcr->p_player->p_obj );	
	// スクロール表示処理
	WF2DMAP_SCRDrawSysMain( p_mcr->p_scrdraw, &p_mcr->scroll );	
	
	return p_obj;
}

//----------------------------------------------------------------------------
/**
 *	@brief	NPCオブジェクト　登録
 *
 *	@param	p_mcr		ワーク
 *	@param	view		姿
 *	@param	friendNo	友達番号
 *
 *	@retval	オブジェクトワーク
 *	@retval	NULL				登録失敗
 */
//-----------------------------------------------------------------------------
MCR_MOVEOBJ* WIFI_MCR_SetNpc( WIFI_MATCHROOM* p_mcr, u32 view, u8 friendNo )
{
	MCR_MOVEOBJ* p_obj;
	MCR_MOVEOBJ_ONPOS pos;
	WF2DMAP_POS hero_pos;

	// 空いているオブジェ取得
	p_obj = WcrMoveObj_GetClean( p_mcr );
	
	// 登録場所を探す
	// -1しているのは主人公を０番とした友達番号構成になっているため
	WcrMapGetNpcSetPos( p_mcr, &pos, friendNo - 1 );

	// その場所に主人公がいないかチェック
	if( p_mcr->p_player ){
		hero_pos = WF2DMAP_OBJWkMatrixGet( p_mcr->p_player->p_obj );
		if( (hero_pos.x == pos.x) &&
			(hero_pos.y == pos.y) ){
			return NULL;
		}
	}

	// 動作設定
	WcrMoveObj_SetUpMove( p_mcr, p_obj, &pos, 
			friendNo, view, MCR_MOVEOBJ_WAY_BOTTOM, WF2DMAP_OBJST_BUSY );

	// 動作関数
	WcrMoveObj_SetMoveFuncKuruKuruInit( p_mcr, p_obj );


	// グラフィック設定
	WcrMoveObj_SetUpGraphic( p_mcr, p_obj, FALSE );
	
	return p_obj;
}

//----------------------------------------------------------------------------
/**
 *	@brief	破棄リクエスト
 *
 *	@param	p_obj	オブジェクトワーク
 */
//-----------------------------------------------------------------------------
void WIFI_MCR_DelPeopleReq( WIFI_MATCHROOM* p_mcr, MCR_MOVEOBJ* p_obj )
{
	s32 way;

	way = WF2DMAP_OBJWkDataGet( p_obj->p_obj, WF2DMAP_OBJPM_WAY );
	
	p_obj->pMove = WcrMoveObj_MoveFuncKuruKuruDel;
	p_obj->pDraw = WcrMoveObj_DrawFuncKuruKuru;
	p_obj->move_st	= MCR_MOVEOBJ_ST_KURUKURU_DEL;
	p_obj->move_count = 0;

	// コマンド実行OFF
	WF2DMAP_OBJDrawWkUpdataFlagSet( p_obj->p_draw, FALSE );
	  
	// くるくるアニメスタート
	WF2DMAP_OBJDrawWkKuruAnimeStart( p_obj->p_draw );

	// 忙しい状態に変更する
	WcrMoveObj_ReqCmdSetEasy( p_mcr, WF2DMAP_OBJST_BUSY, p_obj );
}

//----------------------------------------------------------------------------
/**
 *	@brief	強制的に破棄
 *
 *	@param	p_obj	ワーク
 */
//-----------------------------------------------------------------------------
void WIFI_MCR_DelPeople( MCR_MOVEOBJ* p_obj )
{
	// 2Dキャラクタ破棄
	WF2DMAP_OBJDrawWkDel( p_obj->p_draw );

	// オブジェクト管理破棄
	WF2DMAP_OBJWkDel( p_obj->p_obj );

	memset( p_obj, 0, sizeof(MCR_MOVEOBJ) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	友達番号を取得する
 *
 *	@param	p_obj	オブジェクトワーク
 *
 *	@return	友達番号
 */
//-----------------------------------------------------------------------------
u8	WIFI_MCR_GetFriendNo( const MCR_MOVEOBJ* cp_obj )
{
	return WF2DMAP_OBJWkDataGet( cp_obj->p_obj, WF2DMAP_OBJPM_PLID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	見た目を取得
 *
 *	@param	cp_obj	ワーク
 *
 *	@return	見た目
 */
//-----------------------------------------------------------------------------
u32	WIFI_MCR_GetView( const MCR_MOVEOBJ* cp_obj )
{
	return WF2DMAP_OBJWkDataGet( cp_obj->p_obj, WF2DMAP_OBJPM_CHARA );
}

//----------------------------------------------------------------------------
/**
 *	@brief	向いている逆方向を取得
 *
 *	@param	cp_obj	オブジェクトワーク
 *
 *	@return	逆方向
 */
//-----------------------------------------------------------------------------
WF2DMAP_WAY	WIFI_MCR_GetRetWay( const MCR_MOVEOBJ* cp_obj )
{
	WF2DMAP_WAY way;

	way = WF2DMAP_OBJWkDataGet( cp_obj->p_obj, WF2DMAP_OBJPM_WAY );
	return WF2DMPA_OBJToolRetWayGet( way );
}

//----------------------------------------------------------------------------
/**
 *	@brief	NPCの動作停止　	方向固定
 *
 *	@param	p_mcr	システムワーク
 *	@param	p_obj	オブジェクトワーク
 *	@param	way			方向
 */
//-----------------------------------------------------------------------------
void WIFI_MCR_NpcPauseOn( WIFI_MATCHROOM* p_mcr, MCR_MOVEOBJ* p_obj, WF2DMAP_WAY way )
{
	u32 playid;
	WF2DMAP_POS mat;

	// 今の動作関数を保存しておく
	p_obj->pMoveTmp = p_obj->pMove;
	p_obj->pMove = WcrMoveObj_MoveFuncNpcPause;

	p_obj->move_st_tmp	= p_obj->move_st;
	p_obj->move_st		= MCR_MOVEOBJ_ST_NPC_PAUSE;

	playid = WF2DMAP_OBJWkDataGet( p_obj->p_obj, WF2DMAP_OBJPM_PLID );
	WcrMoveObj_ReqCmdSet( p_mcr, WF2DMAP_CMD_NONE, way, playid );

	// 表示も強制定期に変更
	// まず更新停止
	WF2DMAP_OBJDrawWkUpdataFlagSet( p_obj->p_draw, FALSE );
	WF2DMAP_OBJDrawWkWaySet( p_obj->p_draw, way );

	// なんかしているときもあるので
	// 座標を元に戻す
	mat.x = WF2DMAP_OBJWkDataGet( p_obj->p_obj, WF2DMAP_OBJPM_X );
	mat.y = WF2DMAP_OBJWkDataGet( p_obj->p_obj, WF2DMAP_OBJPM_Y );
	WF2DMAP_OBJDrawWkMatrixSet( p_obj->p_draw, mat );
}

//----------------------------------------------------------------------------
/**
 *	@brief	NPCの動作開始
 *
 *	@param	p_mcr	システムワーク
 *	@param	p_obj	オブジェクトワーク
 */
//-----------------------------------------------------------------------------
void WIFI_MCR_NpcPauseOff( WIFI_MATCHROOM* p_mcr, MCR_MOVEOBJ* p_obj )
{
	// 080708	tomoya
	// NULLならなんもせん
	if( p_obj == NULL ){
		return ;
	}
	if( p_obj->pMoveTmp == NULL ){
		return ;	// もうポーズ状態を回避している
	}
	p_obj->pMove = p_obj->pMoveTmp;
	p_obj->pMoveTmp = NULL;
	p_obj->move_st	= p_obj->move_st_tmp;
	WF2DMAP_OBJDrawWkUpdataFlagSet( p_obj->p_draw, TRUE );
}

//----------------------------------------------------------------------------
/**
 *	@brief	NPCの動作タイプ変更
 *
 *	@param	p_obj		オブジェクトワーク
 *	@param	moveID		動作タイプ
 */
//-----------------------------------------------------------------------------
void WIFI_MCR_NpcMoveSet( WIFI_MATCHROOM* p_mcr, MCR_MOVEOBJ* p_obj, MCR_NPC_MOVETYPE moveID )
{
	BOOL result;
	
	// くるくる動作完了しているかチェック
	result = WcrMoveObj_MoveSetOkCheck( p_obj );
	if( result == FALSE ){
		// フラグだけ設定しておく
		p_obj->moveID = moveID;
	}else{
		// 今設定しても大丈夫
		WcrMoveObj_MoveSet( p_mcr, p_obj, moveID );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	使用中PCアニメ開始
 *
 *	@param	p_mcr	システムワーク
 */
//-----------------------------------------------------------------------------
void WIFI_MCR_PCAnmStart( WIFI_MATCHROOM* p_mcr )
{
	s8 map_param;
	// PCの前にいるか？
	if( WcrMoveObjPCSelectCheck( p_mcr ) == TRUE ){
		// 何番PCか？
		map_param = WIFI_MCR_GetPlayerOnUnderMapParam( p_mcr );
		map_param -= MCR_MAPPM_MAP00;
		GF_ASSERT( (map_param >= 0) && (map_param < 4) );

		// 開始！
		WcrPCANM_UseStart( &p_mcr->pc_anm, map_param );
		return;
	}

	GF_ASSERT( 0 );
}

//----------------------------------------------------------------------------
/**
 *	@brief	アニメを終了させる
 *
 *	@param	p_mcr	ワーク
 */
//-----------------------------------------------------------------------------
void WIFI_MCR_PCAnmOff( WIFI_MATCHROOM* p_mcr )
{
	WcrPCANM_UseEndReq( &p_mcr->pc_anm );
}

//----------------------------------------------------------------------------
/**
 *	@brief	PCアニメメイン動作
 *
 *	@param	p_mcr	ワーク
 */
//-----------------------------------------------------------------------------
void WIFI_MCR_PCAnmMain( WIFI_MATCHROOM* p_mcr )
{
	// パソコンアニメ
	WcrPCANM_Main( p_mcr, &p_mcr->pc_anm );
}


//-----------------------------------------------------------------------------
/**
 *			プライベート関数
 */
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief	全部破棄
 *
 *	@param	p_mcr	ワーク
 */
//-----------------------------------------------------------------------------
static void WcrMoveObjAllDel( WIFI_MATCHROOM* p_mcr )
{
	int i;

	for( i=0; i<MCR_MOVEOBJNUM; i++ ){
		if( p_mcr->moveObj[i].p_obj != NULL ){
			WIFI_MCR_DelPeople( &p_mcr->moveObj[i] );
		}
	}
}	

//----------------------------------------------------------------------------
/**
 *	@brief	キー入力してよいかチェック
 *
 *	@param	cp_mcr	システムワーク
 *
 *	@retval	TRUE	Key入力してもよい
 *	@retval	FALSE	Key入力してはいけない
 */
//-----------------------------------------------------------------------------
static BOOL WcrMoveObjKeyInputCheck( const WIFI_MATCHROOM* cp_mcr )
{
	s32 status;

	status = WF2DMAP_OBJWkDataGet( cp_mcr->p_player->p_obj, WF2DMAP_OBJPM_ST );
	if( status == WF2DMAP_OBJST_NONE ){
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	主人公がPCの前にいるのかチェック
 *
 *	@param	cp_mcr		システムワーク
 *
 *	@retval	TRUE	PC選択
 *	@retval	FALSE	PC非選択
 */
//-----------------------------------------------------------------------------
static BOOL WcrMoveObjPCSelectCheck( const WIFI_MATCHROOM* cp_mcr )
{
	u32 param;
	WF2DMAP_POS pos;
	WF2DMAP_WAY way;

	pos = WF2DMAP_OBJWkMatrixGet( cp_mcr->p_player->p_obj );
	way = WF2DMAP_OBJWkDataGet( cp_mcr->p_player->p_obj, WF2DMAP_OBJPM_WAY );
	pos = WF2DMAP_OBJToolWayPosGet( pos, way );
	param = WF2DMAP_MAPSysParamGet( cp_mcr->p_mapsys, WF2DMAP_POS2GRID(pos.x), WF2DMAP_POS2GRID(pos.y) );

	// 上を向いている必要がある
	if( way == WF2DMAP_WAY_UP ){
		if( param == MCR_MAPPM_PC ){
			return TRUE;
		}
	}

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	コマンド判断＆コマンド実行処理
 *
 *	@param	p_mcr	システムワーク
 */
//-----------------------------------------------------------------------------
static void WcrMoveObjCmdJudgeAndCmdMove( WIFI_MATCHROOM* p_mcr )
{
	WF2DMAP_REQCMD req;
	WF2DMAP_ACTCMD act;
	BOOL result;

	// コマンドが間コマンドを実行
	while( WF2DMAP_REQCMDQSysCmdPop( p_mcr->p_reqcmdQ, &req ) == TRUE ){
		result = WF2DMAP_JUDGESysCmdJudge( p_mcr->p_mapsys, p_mcr->p_objsys, &req, &act );
		if( result == TRUE ){
			WF2DMAP_OBJSysCmdSet( p_mcr->p_objsys, &act );
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	コマンドリクエスト処理
 *
 *	@param	p_mcr	システムワーク
 */
//-----------------------------------------------------------------------------
static void WcrMoveObjCmdReq( WIFI_MATCHROOM* p_mcr )
{
	// 動作関数をまわす
	int i;
	BOOL ret;

	for( i=0; i<MCR_MOVEOBJNUM; i++ ){
		if( p_mcr->moveObj[i].p_obj != NULL ){
			
			ret = p_mcr->moveObj[i].pMove( p_mcr, &p_mcr->moveObj[i] );
			if( ret == TRUE ){
				// 破棄
				WIFI_MCR_DelPeople( &p_mcr->moveObj[i] );
			}
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	オブジェクトイベント表示処理
 *
 *	@param	p_mcr	システムワーク
 */
//-----------------------------------------------------------------------------
static void WcrMoveObjDraw( WIFI_MATCHROOM* p_mcr )
{
	int i;

	for( i=0; i<MCR_MOVEOBJNUM; i++ ){
		if( p_mcr->moveObj[i].p_obj != NULL ){
			p_mcr->moveObj[i].pDraw( p_mcr, &p_mcr->moveObj[i] );
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	オブジェ通しの当たり判定
 *
 *	@param	cp_mcr		システムワーク
 *	@param	cp_obj		オブジェワーク
 *	@param	way			そいつが進む方向
 */
//-----------------------------------------------------------------------------
static const MCR_MOVEOBJ* WcrMoveObjGetHitCheck( const WIFI_MATCHROOM* cp_mcr, const MCR_MOVEOBJ* cp_obj, WF2DMAP_WAY way )
{
	const WF2DMAP_OBJWK* cp_wk;
	int i;
	

	cp_wk = WF2DMAP_OBJSysHitCheck( cp_obj->p_obj, cp_mcr->p_objsys, way );
	
	if( cp_wk != NULL ){
		// そのオブジェを持ってるオブジェを探す
		for( i=0; i<MCR_MOVEOBJNUM; i++ ){
			if( cp_mcr->moveObj[i].p_obj == cp_wk ){
				return &cp_mcr->moveObj[i];
			}
		}
	}

	return NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief	オブジェクトデータ表示システム生成
 *
 *	@param	p_mcr		ワーク
 *	@param	heapID		使用ヒープID
 */
//-----------------------------------------------------------------------------
static void WcrObjDrawInit( WIFI_MATCHROOM* p_mcr, u32 hero_view, u32 heapID )
{
	int i;
	
#ifdef MCR_DEBUG_2CCHAR_CHECK
	p_mcr->p_objdraw = WF2DMAP_OBJDrawSysInit( p_mcr->clact.clactSet, NULL, MCR_MOVEOBJNUM, 
			NNS_G2D_VRAM_TYPE_2DMAIN, heapID );


	WF2DMAP_OBJDrawSysResSet( p_mcr->p_objdraw, AMBRELLA, WF_2DC_MOVENORMAL, heapID );
#else
	p_mcr->p_objdraw = WF2DMAP_OBJDrawSysInit_Shadow( p_mcr->clact.clactSet, NULL, MCR_MOVEOBJNUM, 
			hero_view, WF_2DC_MOVERUN, NNS_G2D_VRAM_TYPE_2DMAIN, heapID );

	// キャラクタデータの登録
	// UNIONキャラクタ
	WF2DMAP_OBJDrawSysUniResSet( p_mcr->p_objdraw, WF_2DC_MOVETURN, heapID );	
#endif


}

//----------------------------------------------------------------------------
/**
 *	@brief	オブジェクトデータ表示システム破棄
 *
 *	@param	p_mcr		ワーク
 */
//-----------------------------------------------------------------------------
static void WcrObjDrawExit( WIFI_MATCHROOM* p_mcr )
{
	WF2DMAP_OBJDrawSysExit( p_mcr->p_objdraw );
}

//----------------------------------------------------------------------------
/**
 *	@brief	スクロール表示システム初期化
 *
 *	@param	p_mcr		ワーク
 *	@param	heapID		ヒープID
 *	@param	p_handle	ハンドル
 */
//-----------------------------------------------------------------------------
static void WcrScrnDrawInit( WIFI_MATCHROOM* p_mcr, u32 heapID, ARCHANDLE* p_handle, u32 map_no )
{
	WF2DMAP_SCRDRAWINIT init = {
		CLACT_U_EASYRENDER_SURFACE_MAIN,
		GF_BGL_FRAME0_M,
		GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0xe000,
		GX_BG_CHARBASE_0x00000,
		GX_BG_EXTPLTT_01,
		MCR_MOVEOBJ_BG_PRI,
		FALSE,
		ARC_WIFIP2PMATCH_GRA,
		NARC_wifip2pmatch_wf_match_top_room_1_NSCR,
		FALSE
	};

	// グラフィックデータを設定
	init.dataid_scrn += map_no;

	p_mcr->p_scrdraw = WF2DMAP_SCRDrawSysInit( 
			&p_mcr->clact.renddata, p_mcr->p_bgl, &init, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	スクロール表示システム破棄
 *
 *	@param	p_mcr		ワーク
 */	
//-----------------------------------------------------------------------------
static void WcrScrnDrawExit( WIFI_MATCHROOM* p_mcr )
{
	WF2DMAP_SCRDrawSysExit( p_mcr->p_scrdraw );
}

//----------------------------------------------------------------------------
/**
 *	@brief	セルアクター　初期化
 *
 *	@param	p_clact		セルアクターワーク
 *	@param	heapID		ヒープ
 *	@param	p_handle	アーカイブハンドル
 */
//-----------------------------------------------------------------------------
static void WcrClactInit( MCR_CLACT* p_clact, u32 heapID, ARCHANDLE* p_handle )
{
	int i;

	// セルアクターセット作成
	p_clact->clactSet = CLACT_U_SetEasyInit( MCR_CLACT_OBJNUM, &p_clact->renddata, heapID );
    CLACT_U_SetSubSurfaceMatrix( &p_clact->renddata, 0, MCR_CLACTSUBSURFACE_Y );

	// キャラとパレットのリソースマネージャ作成
	for( i=0; i<MCR_CLACT_RESNUM; i++ ){
		p_clact->resMan[i] = CLACT_U_ResManagerInit(MCR_CLACT_LOADRESNUM, i, heapID);
	}

	// 人物リソース読み込みとキャラクタパレットの転送
	// エフェクトリソース読み込みとキャラクタパレットの転送
	WcrClactResLoad( p_clact, heapID, p_handle );

	// アクターの登録
	WcrClactAdd( p_clact, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	セルアクターデータ破棄
 *	
 *	@param	p_clact		セルアクターワーク
 */
//-----------------------------------------------------------------------------
static void WcrClactDest( MCR_CLACT* p_clact )
{
	int i;

	// アクターの破棄
	WcrClactDel( p_clact );
	
	// リソース破棄
	WcrClactResRelease( p_clact );
	
	// リソースマネージャ破棄
	for( i=0; i<MCR_CLACT_RESNUM; i++ ){
		CLACT_U_ResManagerDelete( p_clact->resMan[i] );
	}

	// セルアクターセット破棄
	CLACT_DestSet( p_clact->clactSet );
}

//----------------------------------------------------------------------------
/**
 *	@brief	人物、エフェクトのリソース読み込み＆転送
 *
 *	@param	p_clact		セルアクターワーク
 *	@param	heapID		ヒープID
 *	@param	p_handle	アーカイブハンドル
 */
//-----------------------------------------------------------------------------
static void WcrClactResLoad( MCR_CLACT* p_clact, u32 heapID, ARCHANDLE* p_handle )
{
	//　エフェクトリソース読込み
	WcrClactResEffectLoad( p_clact, heapID, p_handle );
}

//----------------------------------------------------------------------------
/**
 *	@brief	セルアクターリソース破棄
 *
 *	@param	p_clact		セルアクターワーク
 */
//-----------------------------------------------------------------------------
static void WcrClactResRelease( MCR_CLACT* p_clact )
{
	// エフェクトリソース破棄
	WcrClactResEffectRelease( p_clact );
}

//----------------------------------------------------------------------------
/**
 *	@brief	セルアクターワーク登録
 *
 *	@param	p_clact		セルアクターデータワーク
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WcrClactAdd( MCR_CLACT* p_clact, u32 heapID )
{
	WcrClactResEffectAdd( p_clact, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	セルアクターワーク破棄
 *	
 *	@param	p_clact		セルアクターデータワーク
 */
//-----------------------------------------------------------------------------
static void WcrClactDel( MCR_CLACT* p_clact )
{
	WcrClactResEffectDel( p_clact );
}

//----------------------------------------------------------------------------
/**
 *	@brief	BGコントロール設定
 *
 *	@param	p_bgl	BGL
 *	@param	heapID	使用ヒープ
 */
//-----------------------------------------------------------------------------
static void WcrBgContInit( GF_BGL_INI* p_bgl, u32 heapID )
{
}

//----------------------------------------------------------------------------
/**
 *	@brief	使用したBGCONTを破棄する
 *
 *	@param	p_bgl	BGL
 */
//-----------------------------------------------------------------------------
static void WcrBgContDest( GF_BGL_INI* p_bgl )
{
}

//----------------------------------------------------------------------------
/**
 *	@brief	BGを設定
 *
 *	@param	p_bgl		BGL
 *	@param	heapID		ヒープ
 *	@param	p_handle	ハンドル
 */
//-----------------------------------------------------------------------------
static void WcrBgSet( GF_BGL_INI* p_bgl, u32 heapID, ARCHANDLE* p_handle )
{
	// パレット転送
	// 使用してよいパレットはMAX　8
	ArcUtil_HDL_PalSet( p_handle, NARC_wifip2pmatch_wf_match_top_room_NCLR, PALTYPE_MAIN_BG, 0, 8*32, heapID );	
	// バックグラウンドカラーは変えない
	GF_BGL_BackGroundColorSet( GF_BGL_FRAME0_M, 0 );
	
    // メイン画面BG2キャラ転送
    ArcUtil_HDL_BgCharSet( p_handle, NARC_wifip2pmatch_wf_match_top_room_NCGR, p_bgl,
                       GF_BGL_FRAME0_M, 0, 0, 0, heapID);
/*
    // メイン画面BG2スクリーン転送
    ArcUtil_HDL_ScrnSet(   p_handle, NARC_wifip2pmatch_wf_match_top_room_1_NSCR+map_no, p_bgl,
                       GF_BGL_FRAME0_M, 0, 0, 0, heapID);
//*/
}

//----------------------------------------------------------------------------
/**
 *	@brief	エフェクトのリソース読込み
 *
 *	@param	p_clact		セルアクターワーク
 *	@param	heapID		ヒープ
 *	@param	p_handle	ハンドル
 */
//-----------------------------------------------------------------------------
static void WcrClactResEffectLoad( MCR_CLACT* p_clact, u32 heapID, ARCHANDLE* p_handle )
{
	BOOL result;
	
	p_clact->effect.resobj[0] = 
		CLACT_U_ResManagerResAddArcChar_ArcHandle( p_clact->resMan[0], 
				p_handle, NARC_wifip2pmatch_wf_match_top_room_obj_NCGR,
				FALSE, MCR_EFFECTRES_CONTID, NNS_G2D_VRAM_TYPE_2DMAIN, heapID );

	p_clact->effect.resobj[1] = 
		CLACT_U_ResManagerResAddArcPltt_ArcHandle( p_clact->resMan[1], 
				p_handle, NARC_wifip2pmatch_wf_match_top_room_obj_NCLR,
				FALSE, MCR_EFFECTRES_CONTID, NNS_G2D_VRAM_TYPE_2DMAIN, 1, heapID );

	p_clact->effect.resobj[2] = 
		CLACT_U_ResManagerResAddArcKindCell_ArcHandle( p_clact->resMan[2], 
				p_handle, NARC_wifip2pmatch_wf_match_top_room_obj_NCER,
				FALSE, MCR_EFFECTRES_CONTID, CLACT_U_CELL_RES, heapID );

	p_clact->effect.resobj[3] = 
		CLACT_U_ResManagerResAddArcKindCell_ArcHandle( p_clact->resMan[3], 
				p_handle, NARC_wifip2pmatch_wf_match_top_room_obj_NANR,
				FALSE, MCR_EFFECTRES_CONTID, CLACT_U_CELLANM_RES, heapID );

	// Vram転送
	result = CLACT_U_CharManagerSetAreaCont( p_clact->effect.resobj[0] );
	GF_ASSERT( result == TRUE );
	result = CLACT_U_PlttManagerSetCleanArea( p_clact->effect.resobj[1] );
	GF_ASSERT( result == TRUE );
	
	// メモリからリソースを破棄
	CLACT_U_ResManagerResOnlyDelete( p_clact->effect.resobj[0] );
	CLACT_U_ResManagerResOnlyDelete( p_clact->effect.resobj[1] );

	// ヘッダー作成
	CLACT_U_MakeHeader( &p_clact->effect.header, 
			 MCR_EFFECTRES_CONTID, MCR_EFFECTRES_CONTID, 
			 MCR_EFFECTRES_CONTID, MCR_EFFECTRES_CONTID,
			 CLACT_U_HEADER_DATA_NONE, CLACT_U_HEADER_DATA_NONE,
			 0, MCR_EFFECTRES_BGPRI, 
			 p_clact->resMan[0], p_clact->resMan[1],
			 p_clact->resMan[2], p_clact->resMan[3],
			 NULL, NULL );
}

//----------------------------------------------------------------------------
/**
 *	@brief	エフェクトのリソース破棄
 *
 *	@param	p_clact		セルアクターワーク
 */
//-----------------------------------------------------------------------------
static void WcrClactResEffectRelease( MCR_CLACT* p_clact )
{
	int i;

	CLACT_U_CharManagerDelete( p_clact->effect.resobj[0] );
	CLACT_U_PlttManagerDelete( p_clact->effect.resobj[1] );
	
	for( i=0; i<4; i++ ){
		CLACT_U_ResManagerResDelete( p_clact->resMan[i], p_clact->effect.resobj[i] );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	エフェクトアクター登録
 *
 *	@param	p_clact		セルアクターデータワーク
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WcrClactResEffectAdd( MCR_CLACT* p_clact, u32 heapID )
{
	CLACT_ADD add;

	// ０クリア
	memset( &add, 0, sizeof(CLACT_ADD) );
	
	add.ClActSet = p_clact->clactSet;
	add.ClActHeader = &p_clact->effect.header;
	add.DrawArea = NNS_G2D_VRAM_TYPE_2DMAIN;
	add.sca.x = FX32_ONE;
	add.sca.y = FX32_ONE;
	add.pri = MCR_EFFECTRES_SOFTPRI;
	add.heap  = heapID;

	p_clact->effect.exit_cursor = CLACT_Add( &add );
	p_clact->effect.obj_waku = CLACT_Add( &add );

	// 表示OFF
	CLACT_SetDrawFlag( p_clact->effect.exit_cursor, FALSE );
	CLACT_SetDrawFlag( p_clact->effect.obj_waku, FALSE );

	// カーソルはオートアニメ
	CLACT_SetAnmFlag( p_clact->effect.exit_cursor, TRUE );

	// 枠はアニメ１
	CLACT_AnmChg( p_clact->effect.obj_waku, 1 );
}

//----------------------------------------------------------------------------
/**
 *	@brief	エフェクト破棄
 *
 *	@param	p_clact		セルアクターデータワーク
 */
//-----------------------------------------------------------------------------
static void WcrClactResEffectDel( MCR_CLACT* p_clact )
{
	CLACT_Delete( p_clact->effect.exit_cursor );
	CLACT_Delete( p_clact->effect.obj_waku );
}

//----------------------------------------------------------------------------
/**
 *	@brief	出口描画ON
 *
 *	@param	p_mcr		システムワーク
 */
//-----------------------------------------------------------------------------
static void WcrClactResEffectExitDrawOn( WIFI_MATCHROOM* p_mcr )
{
	MCR_MOVEOBJ_ONPOS pos;
	VecFx32 mat;
	
	// 出口座標を取得 補正してアクターに設定
	WcrMapGetPlayerSetPos( p_mcr, &pos );

	pos.x += MCR_EFFECTRES_OFS_X;
	pos.y += MCR_EFFECTRES_OFS_Y;
	mat.x = pos.x << FX32_SHIFT;
	mat.y = pos.y << FX32_SHIFT;
	CLACT_SetMatrix( p_mcr->clact.effect.exit_cursor, &mat );
	
	
	// その座標で表示ON
	CLACT_SetDrawFlag( p_mcr->clact.effect.exit_cursor, TRUE );
}

//----------------------------------------------------------------------------
/**
 *	@brief	出口描画OFF
 *
 *	@param	p_mcr		システムワーク
 */
//-----------------------------------------------------------------------------
static void WcrClactResEffectExitDrawOff( WIFI_MATCHROOM* p_mcr )
{
	CLACT_SetDrawFlag( p_mcr->clact.effect.exit_cursor, FALSE );
}

//----------------------------------------------------------------------------
/**
 *	@brief	カーソルON
 *
 *	@param	p_mcr		システムワーク
 *	@param	pos			カーソル座標
 *	@param	pri			表示優先順位
 */	
//-----------------------------------------------------------------------------
static void WcrClactResEffectCursorDrawOn( WIFI_MATCHROOM* p_mcr, WF2DMAP_POS pos, u32 pri )
{
	VecFx32 mat;

	pos.x += MCR_EFFECTWAKURES_OFS_X;
	pos.y += MCR_EFFECTWAKURES_OFS_Y;
	mat.x = pos.x << FX32_SHIFT;
	mat.y = pos.y << FX32_SHIFT;
	CLACT_SetMatrix( p_mcr->clact.effect.obj_waku, &mat );

	CLACT_DrawPriorityChg( p_mcr->clact.effect.obj_waku, pri );
	
	// その座標で表示ON
	CLACT_SetDrawFlag( p_mcr->clact.effect.obj_waku, TRUE );
}

//----------------------------------------------------------------------------
/**
 *	@brief	カーソルOFF
 *
 *	@param	p_mcr 
 */
//-----------------------------------------------------------------------------
static void WcrClactResEffectCursorDrawOff( WIFI_MATCHROOM* p_mcr )
{
	CLACT_SetDrawFlag( p_mcr->clact.effect.obj_waku, FALSE );
}



//----------------------------------------------------------------------------
/**
 *	@brief	出口チェックの有無
 *
 *	@param	p_mcr	ワーク
 *
 *	@retval	TRUE	出口にのった
 *	@retval	FALSE	出口にのっていない
 */
//-----------------------------------------------------------------------------
static BOOL WcrExitCheck( WIFI_MATCHROOM* p_mcr )
{
	WF2DMAP_POS pos;
	u32 map_param;
	WF2DMAP_WAY	way;
	
	// 主人公がいるかチェック
	if( p_mcr->p_player == NULL ){
		return FALSE;
	}

	// 座標を取得
	pos = WF2DMAP_OBJWkMatrixGet( p_mcr->p_player->p_obj );

	// 方向を取得
	way = WF2DMAP_OBJWkDataGet( p_mcr->p_player->p_obj, WF2DMAP_OBJPM_WAY );

	// 足元のマップデータ取得
	map_param = WF2DMAP_MAPSysParamGet( p_mcr->p_mapsys, WF2DMAP_POS2GRID(pos.x), WF2DMAP_POS2GRID(pos.y) );

	// 出口の上で下を向いていたら終了
	if( (map_param == MCR_MAPPM_EXIT) && (way == WF2DMAP_WAY_DOWN) ){
		return TRUE;
	}

	return FALSE;
}


//----------------------------------------------------------------------------
/**
 *	@brief	NPC登録座標を取得
 *
 *	@param	p_mcr		ワーク
 *	@param	p_pos		座標設定先
 *	@param	friendNo	フレンドナンバー
 */
//-----------------------------------------------------------------------------
static void WcrMapGetNpcSetPos( WIFI_MATCHROOM* p_mcr, MCR_MOVEOBJ_ONPOS* p_pos, u32 friendNo )
{
	u32 block_num;
	u32 block_idx;
	int i, j;
	u32 map_grid_x, map_grid_y;
	u32 param;
	u32 block_no;

	block_num = friendNo / WCR_MAPDATA_1BLOCKOBJNUM;
	block_idx = friendNo % WCR_MAPDATA_1BLOCKOBJNUM;

	map_grid_x = WF2DMAP_MAPSysGridXGet( p_mcr->p_mapsys );
	map_grid_y = WF2DMAP_MAPSysGridYGet( p_mcr->p_mapsys );
	
	// block_numのblock_idxのマップ位置を取得する
	for( i=0; i<map_grid_y; i++ ){
		for( j=0; j<map_grid_x; j++ ){
			param = WF2DMAP_MAPSysParamGet( p_mcr->p_mapsys, j, i );
			if( param == MCR_MAPPM_OBJ00+block_idx ){
				// その下にブロックNoが入っているのでチェック
				block_no = WF2DMAP_MAPSysParamGet( p_mcr->p_mapsys, j, i+1 );
				if( block_no == block_num+MCR_MAPPM_MAP00 ){
					// みつかった！
					p_pos->x =  WF2DMAP_GRID2POS( j );
					p_pos->y =  WF2DMAP_GRID2POS( i );
					return ;
				}
			}
		}
	}

	// なかたった
	GF_ASSERT_MSG( 0, "frinedno=%d\n", friendNo );
}

//----------------------------------------------------------------------------
/**
 *	@brief	プレイヤー登録座標を取得
 *	
 *	@param	p_mcr	システムワーク
 *	@param	p_pos	座標格納先
 */
//-----------------------------------------------------------------------------
static void WcrMapGetPlayerSetPos( WIFI_MATCHROOM* p_mcr, MCR_MOVEOBJ_ONPOS* p_pos )
{
	int i, j;
	u32 map_grid_x, map_grid_y;
	u32 param;

	map_grid_x = WF2DMAP_MAPSysGridXGet( p_mcr->p_mapsys );
	map_grid_y = WF2DMAP_MAPSysGridYGet( p_mcr->p_mapsys );
	
	// block_numのblock_idxのマップ位置を取得する
	for( i=0; i<map_grid_y; i++ ){
		for( j=0; j<map_grid_x; j++ ){
			param = WF2DMAP_MAPSysParamGet( p_mcr->p_mapsys, j, i );
#ifdef WFP2P_DEBUG_PLON_PC
			if( param == MCR_MAPPM_PC ){
				// みつかった！
				p_pos->x =  WF2DMAP_GRID2POS( j );
				p_pos->y =  WF2DMAP_GRID2POS( i+1 );
				return ;
			}
#else
			if( param == MCR_MAPPM_EXIT ){
				// みつかった！
				p_pos->x =  WF2DMAP_GRID2POS( j );
				p_pos->y =  WF2DMAP_GRID2POS( i );
				return ;
			}
#endif
		}
	}

	// なかたった
	GF_ASSERT(0);
}


//----------------------------------------------------------------------------
/**
 *	@brief	空いているオブジェクトを取得
 *
 *	@param	p_mcr	ワーク
 *
 *	@retval	空いているオブジェクト
 */
//-----------------------------------------------------------------------------
static MCR_MOVEOBJ* WcrMoveObj_GetClean( WIFI_MATCHROOM* p_mcr )
{
	int i;

	for( i=0; i<MCR_MOVEOBJNUM; i++ ){
		if( p_mcr->moveObj[i].p_obj == NULL ){
			return &p_mcr->moveObj[i];
		}
	}
	GF_ASSERT(0);
	return NULL;
}


//----------------------------------------------------------------------------
/**
 *	@brief	グラフィックセットアップ
 *
 *	@param	p_mcr		ワーク
 *	@param	p_obj		動作オブジェクト
 *	@param	hero		主人公かどうか
 */
//-----------------------------------------------------------------------------
static void WcrMoveObj_SetUpGraphic( WIFI_MATCHROOM* p_mcr, MCR_MOVEOBJ* p_obj, BOOL hero )
{
	// 表示データの登録
	p_obj->p_draw = WF2DMAP_OBJDrawWkNew( p_mcr->p_objdraw, p_obj->p_obj, hero, p_mcr->use_heap );

	if( hero == FALSE ){
		// まず更新停止
		WF2DMAP_OBJDrawWkUpdataFlagSet( p_obj->p_draw, FALSE );

		// くるくるアニメスタート
		WF2DMAP_OBJDrawWkKuruAnimeStart( p_obj->p_draw );

		// OBJ表示OFF
		WF2DMAP_OBJDrawWkDrawFlagSet( p_obj->p_draw, FALSE );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	動作オブジェの動作部分の初期化
 *
 *	@param	p_mcr		ワーク
 *	@param	p_obj		オブジェクトワーク
 *	@param	cp_pos		登録ポジション
 *	@param	friendNo	友達ナンバー
 *	@param	charid		キャラクタID
 *	@param	way			初期動作方向
 *	@param	status		初期状態
 */
//-----------------------------------------------------------------------------
static void WcrMoveObj_SetUpMove( WIFI_MATCHROOM* p_mcr, MCR_MOVEOBJ* p_obj, const MCR_MOVEOBJ_ONPOS* cp_pos, u32 friendNo, u16 charaid, u8 way, WF2DMAP_OBJST status )
{
	WF2DMAP_OBJDATA add;

	// オブジェクト初期化
	add.x = cp_pos->x;
	add.y = cp_pos->y;
	add.playid = friendNo;
	add.status = status;
	add.way = way;
	add.charaid = charaid;
	p_obj->p_obj = WF2DMAP_OBJWkNew( p_mcr->p_objsys, &add );
}

//----------------------------------------------------------------------------
/**
 *	@brief	その人に今話しかけて大丈夫かチェック
 *
 *	@param	cp_obj	ワーク
 *
 *	@retval	TRUE	大丈夫
 *	@retval	FALSE	だめ
 */
//-----------------------------------------------------------------------------
static BOOL WcrMoveObj_CheckAccess( const MCR_MOVEOBJ* cp_obj )
{
	switch( cp_obj->move_st ){
	// OK
	case MCR_MOVEOBJ_ST_PL_KEYWAIT:		// プレイヤー通常動作
	case MCR_MOVEOBJ_ST_NPC:				// NPC動作
	case MCR_MOVEOBJ_ST_NPC_JUMP:		// NPC募集中
		return TRUE;
		
	// NG
	case MCR_MOVEOBJ_ST_KURUKURU:		// 落ちる処理
	case MCR_MOVEOBJ_ST_KURUKURU_DEL:	// 上る処理
	case MCR_MOVEOBJ_ST_NPC_PAUSE:		// NPC動作停止
		return FALSE;
		
	default:
		break;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	プレイヤー動作開始
 *
 *	@param	p_obj		オブジェクトワーク
 */
//-----------------------------------------------------------------------------
static void WcrMoveObj_SetMoveFuncPlayer( WIFI_MATCHROOM* p_mcr, MCR_MOVEOBJ* p_obj )
{
	p_obj->pMove = WcrMoveObj_MoveFuncPlayerKeyWait;
	p_obj->pDraw = WcrMoveObj_DrawFuncDefault;

	p_obj->move_st	= MCR_MOVEOBJ_ST_PL_KEYWAIT;

	// 状態を通常に戻す
	WcrMoveObj_ReqCmdSetEasy( p_mcr, WF2DMAP_OBJST_NONE, p_obj );
}

//----------------------------------------------------------------------------
/**
 *	@brief	くるくる初期化設定
 *
 *	@param	p_obj		オブジェクトワーク
 */
//-----------------------------------------------------------------------------
static void WcrMoveObj_SetMoveFuncKuruKuruInit( WIFI_MATCHROOM* p_mcr, MCR_MOVEOBJ* p_obj )
{
	p_obj->pMove = WcrMoveObj_MoveFuncKuruKuru;
	p_obj->pDraw = WcrMoveObj_DrawFuncKuruKuru;
	p_obj->move_count = MCR_MOVEOBJ_KURU_COUNT;

	p_obj->move_st	= MCR_MOVEOBJ_ST_KURUKURU;
}

//----------------------------------------------------------------------------
/**
 *	@brief	NPC動作設定
 *
 *	@param	p_obj	オブジェクトワーク
 */
//-----------------------------------------------------------------------------
static void WcrMoveObj_SetMoveFuncNpc( WIFI_MATCHROOM* p_mcr, MCR_MOVEOBJ* p_obj )
{
	p_obj->pMove = WcrMoveObj_MoveFuncNpc;
	p_obj->pDraw = WcrMoveObj_DrawFuncDefault;
	p_obj->move_count = MCR_MOVEOBJ_NPC_RAND_S + (gf_mtRand() % MCR_MOVEOBJ_NPC_RAND_M);

	p_obj->move_st	= MCR_MOVEOBJ_ST_NPC;

	// 状態を通常に戻す
	WcrMoveObj_ReqCmdSetEasy( p_mcr, WF2DMAP_OBJST_NONE, p_obj );

	// 描画更新有効
	WF2DMAP_OBJDrawWkUpdataFlagSet( p_obj->p_draw, TRUE );
}

//----------------------------------------------------------------------------
/**
 *	@brief	NPCジャンプ動作設定
 *
 *	@param	p_mcr	システムワーク
 *	@param	p_obj	オブジェクトワーク
 */
//-----------------------------------------------------------------------------
static void WcrMoveObj_SetMoveFuncNpcJump( WIFI_MATCHROOM* p_mcr, MCR_MOVEOBJ* p_obj )
{
	p_obj->pMove = WcrMoveObj_MoveFuncNpcJump;
	p_obj->pDraw = WcrMoveObj_DrawFuncNpcJump;
	p_obj->move_count = MCR_MOVEOBJ_NPC_JUMP_COUNT;

	p_obj->move_st	= MCR_MOVEOBJ_ST_NPC_JUMP;


	// 状態を通常に戻す
	WcrMoveObj_ReqCmdSetEasy( p_mcr, WF2DMAP_OBJST_BUSY, p_obj );

	// 描画更新無効
	WF2DMAP_OBJDrawWkUpdataFlagSet( p_obj->p_draw, FALSE );
}

//----------------------------------------------------------------------------
/**
 *	@brief	方向の逆を取得
 *		
 *	@param	way		方向
 */
//-----------------------------------------------------------------------------
static u32 WcrMoveObj_GetRetWay( u32 way )
{
	static const u32 RetWay[] = {
		MCR_MOVEOBJ_WAY_BOTTOM,
		MCR_MOVEOBJ_WAY_TOP,
		MCR_MOVEOBJ_WAY_RIGHT,
		MCR_MOVEOBJ_WAY_LEFT,
	};
	return RetWay[ way ];
}

//----------------------------------------------------------------------------
/**
 *	@brief	描画関数デフォルト
 */
//-----------------------------------------------------------------------------
static void WcrMoveObj_DrawFuncDefault( WIFI_MATCHROOM* p_mcr, MCR_MOVEOBJ* p_obj )
{
}

//----------------------------------------------------------------------------
/**
 *	@brief	くるくる
 *
 *	@param	p_obj ワーク
 */
//-----------------------------------------------------------------------------
static void WcrMoveObj_DrawFuncKuruKuru( WIFI_MATCHROOM* p_mcr, MCR_MOVEOBJ* p_obj )
{
	WF2DMAP_POS mat, shadow_mat;

	WF2DMAP_OBJDrawWkKuruAnimeMain( p_obj->p_draw );

	// 非表示なら表示にする
	if( WF2DMAP_OBJDrawWkDrawFlagGet( p_obj->p_draw ) == FALSE ){
		WF2DMAP_OBJDrawWkDrawFlagSet( p_obj->p_draw, TRUE );
	}

	// 位置を上から落としてくる
	mat.x = WF2DMAP_OBJWkDataGet( p_obj->p_obj, WF2DMAP_OBJPM_X );
	mat.y = WF2DMAP_OBJWkDataGet( p_obj->p_obj, WF2DMAP_OBJPM_Y );
	shadow_mat = mat;
	mat.y += FX_Mul( _Sin360( p_obj->move_count ), MCR_MOVEOBJ_KURU_DIST*FX32_ONE ) >> FX32_SHIFT;
	WF2DMAP_OBJDrawWkMatrixSet( p_obj->p_draw, mat );

	// 陰は地面の位置に出しっぱなし
	WF2DMAP_OBJDrawWkShadowMatrixSet( p_obj->p_draw, shadow_mat );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ジャンプ描画
 */
//-----------------------------------------------------------------------------
static void WcrMoveObj_DrawFuncNpcJump( WIFI_MATCHROOM* p_mcr, MCR_MOVEOBJ* p_obj )
{
	u32 r;
	WF2DMAP_POS mat, shadow_mat;
	s16 count;

	mat.x = WF2DMAP_OBJWkDataGet( p_obj->p_obj, WF2DMAP_OBJPM_X );
	mat.y = WF2DMAP_OBJWkDataGet( p_obj->p_obj, WF2DMAP_OBJPM_Y );
	shadow_mat = mat;

	// カウント値取得
	count = p_obj->move_count - MCR_MOVEOBJ_NPC_JUMP_WAITCOUNT;

	// count > 0　だとジャンプ中	それ以外は次のジャンプまでのウエイト
	if( count > 0 ){

		count = count % MCR_MOVEOBJ_NPC_JUMP_EFFCOUNT;

		// SINカーブの角度を取得
		r = (MCR_MOVEOBJ_NPC_JUMP_RMAX*count) / MCR_MOVEOBJ_NPC_JUMP_EFFCOUNT;

		// ジャンプ
		mat.y -= FX_Mul( Sin360(r), MCR_MOVEOBJ_NPC_JUMP_DIS*FX32_ONE ) >> FX32_SHIFT;
	}

	WF2DMAP_OBJDrawWkMatrixSet( p_obj->p_draw, mat );

	// 陰は地面の位置に出しっぱなし
	WF2DMAP_OBJDrawWkShadowMatrixSet( p_obj->p_draw, shadow_mat );

	// 下を向かせる
	WF2DMAP_OBJDrawWkWaySet( p_obj->p_draw, WF2DMAP_WAY_DOWN );
}


//----------------------------------------------------------------------------
/**
 *	@brief	動作関数
 *
 *	@param	p_obj	オブジェクトワーク
 *
 *	@retval	TRUE	終了
 *	@retval	FALSE	通常
 */
//-----------------------------------------------------------------------------
// 主人公用
static BOOL WcrMoveObj_MoveFuncPlayerKeyWait( WIFI_MATCHROOM* p_mcr, MCR_MOVEOBJ* p_obj )
{
	s32 way;
	s32 playid;
	s32 status;
	s32 cmd;
	const MCR_MOVEOBJ* cp_wk;

	way = WF2DMAP_OBJWkDataGet( p_obj->p_obj, WF2DMAP_OBJPM_WAY );
	playid = WF2DMAP_OBJWkDataGet( p_obj->p_obj, WF2DMAP_OBJPM_PLID );

	// 待機状態でなければならない
	status = WF2DMAP_OBJWkDataGet( p_obj->p_obj, WF2DMAP_OBJPM_ST );
	if( status != WF2DMAP_OBJST_NONE ){
		return FALSE;
	}


	// キャラクタを入れ替えるデバック機能
#ifdef MCR_DEBUG_2CCHAR_CHECK
	if( sys.trg & PAD_BUTTON_START ){
		static const u16 sc_DEBUG_VIEW_TBL[] = {
			HERO,
			BOY1,
			BOY3,
			MAN3,
			BADMAN,
			EXPLORE,
			FIGHTER,
			GORGGEOUSM,
			MYSTERY, 
			HEROINE,
			GIRL1, 
			GIRL2, 
			WOMAN2,
			WOMAN3,
			IDOL,  
			LADY,  
			COWGIRL,
			GORGGEOUSW,
			BOY2,			
			GIRL3,			
			CAMPBOY,		
			PICNICGIRL,	
			BABYGIRL1,		
			MIDDLEMAN1,	
			MIDDLEWOMAN1,	
			WAITER,		
			WAITRESS,		
			MAN1,			
			WOMAN1,		
			CAMERAMAN,		
			REPORTER,		
			FARMER,		
			CYCLEM,		
			CYCLEW,		
			OLDMAN1,		
			MAN5,			
			WOMAN5,		
			BABYBOY1,		
			SPORTSMAN,		
			FISHING,		
			SEAMAN,		
			MOUNT,			
			MAN2,			
			BIGMAN,		
			ASSISTANTM,	
			GENTLEMAN,		
			WORKMAN,		
			CLOWN,			
			POLICEMAN,		
			AMBRELLA,		
			PIKACHU,		
			SEVEN1,		
			SEVEN2,		
			SEVEN3,		
			SEVEN4,		
			SEVEN5,		
			TOWERBOSS,		
			ACHAMO,		
			ARTIST,		
			ASSISTANTW,	
			BABY,			
			BAG,			
			BALLOON,		
			BEACHBOY,		
			BEACHGIRL,		
			BIGFOUR1,		
			BIGFOUR2,		
			BIGFOUR3,		
			BIGFOUR4,		
			BIRD,			
			BOY4,			
			CHAMPION,		
			DOCTOR,		
			DSBOY,			
			ENECO,			
			FREEZES,		
			GINGABOSS,		
			GINGAM,		
			GINGAW,		
			GIRL4,			
			GKANBU1,		
			GKANBU2,		
			GKANBU3,		
			KINOCOCO,		
			KODUCK,		
			KOIKING,		
			LEADER1,		
			LEADER2,		
			LEADER3,		
			LEADER4,		
			LEADER5,		
			LEADER6,		
			LEADER7,		
			LEADER8,		
			MAID,			
			MAMA,			
			MIDDLEMAN2,	
			MIDDLEWOMAN2,	
			MIKAN,			
			MIMITUTO,		
			MINUN,			
			MOSS,			
			OLDMAN2,		
			OLDWOMAN1,		
			OLDWOMAN2,		
			OOKIDO,		
			PATIRITUSU,	
			PCWOMAN1,		
			BRAINS2,		
			BRAINS3,		
			BRAINS4,		
			PRINCESS,		
			BFSM,			
			BFSW1,			
			BFSW2,			
			WIFISM,		
			WIFISW,		
			DPHERO,		
			DPHEROINE,		
		};
		
		WF2DMAP_OBJDrawWkDel( p_obj->p_draw );
		WF2DMAP_OBJDrawSysResDel( p_mcr->p_objdraw, s_MCR_DEBUG_2CCHAR_VIEW_ID );

		s_MCR_DEBUG_2CCHAR_VIEW_ID = sc_DEBUG_VIEW_TBL[s_MCR_DEBUG_2CCHAR_VIEW_COUNT];
		s_MCR_DEBUG_2CCHAR_VIEW_COUNT = (s_MCR_DEBUG_2CCHAR_VIEW_COUNT + 1) % NELEMS(sc_DEBUG_VIEW_TBL);

		WF2DMAP_OBJDrawSysResSet( p_mcr->p_objdraw, s_MCR_DEBUG_2CCHAR_VIEW_ID, WF_2DC_MOVENORMAL, p_mcr->use_heap );

		WF2DMAP_OBJWkDataSet( p_obj->p_obj, WF2DMAP_OBJPM_CHARA, s_MCR_DEBUG_2CCHAR_VIEW_ID );
		p_obj->p_draw = WF2DMAP_OBJDrawWkNew( p_mcr->p_objdraw, p_obj->p_obj, TRUE, p_mcr->use_heap );
	}
#endif

	// 選択関係
	if( sys.trg & PAD_BUTTON_DECIDE ){

		// 目の前に人がいるかチェック
		cp_wk = WcrMoveObjGetHitCheck( p_mcr, p_obj, way );
		
		if( cp_wk != NULL ){
			// その人は忙しくないかチェック
			if( WcrMoveObj_CheckAccess( cp_wk ) == TRUE ){
				// そのfriendNOを設定
				p_obj->accesFriend = WF2DMAP_OBJWkDataGet( cp_wk->p_obj, WF2DMAP_OBJPM_PLID );
			}else{
				// 通信バグ602対処	tomoya
				p_obj->accesFriend = 0;
			}
		}else{
			p_obj->accesFriend = 0;
		}
	}

	// 移動停止状態かチェック
	if( p_mcr->player_pause == TRUE ){	
		return FALSE;
	}

	// 歩くか走るか
	if( sys.cont & PAD_BUTTON_B ){
		cmd = WF2DMAP_OBJST_RUN;
	}else{
		cmd = WF2DMAP_OBJST_WALK;
	}
	
	// 移動関係
	if( sys.cont & PAD_KEY_UP ){
		if( way == MCR_MOVEOBJ_WAY_TOP ){
			WcrMoveObj_ReqCmdSet( p_mcr, cmd, way, playid );
		}else{
			WcrMoveObj_ReqCmdSet( p_mcr, WF2DMAP_OBJST_TURN, MCR_MOVEOBJ_WAY_TOP, playid );
		}
	}else if( sys.cont & PAD_KEY_DOWN ){
		if( way == MCR_MOVEOBJ_WAY_BOTTOM ){
			WcrMoveObj_ReqCmdSet( p_mcr, cmd, way, playid );
		}else{
			WcrMoveObj_ReqCmdSet( p_mcr, WF2DMAP_OBJST_TURN, MCR_MOVEOBJ_WAY_BOTTOM, playid );
		}
	}else if( sys.cont & PAD_KEY_LEFT ){
		if( way == MCR_MOVEOBJ_WAY_LEFT ){
			WcrMoveObj_ReqCmdSet( p_mcr, cmd, way, playid );
		}else{
			WcrMoveObj_ReqCmdSet( p_mcr, WF2DMAP_OBJST_TURN, MCR_MOVEOBJ_WAY_LEFT, playid );
		}
	}else if( sys.cont & PAD_KEY_RIGHT ){
		if( way == MCR_MOVEOBJ_WAY_RIGHT ){
			WcrMoveObj_ReqCmdSet( p_mcr, cmd, way, playid );
		}else{
			WcrMoveObj_ReqCmdSet( p_mcr, WF2DMAP_OBJST_TURN, MCR_MOVEOBJ_WAY_RIGHT, playid );
		}
	}
	
	return FALSE;
}

// くるくる
static BOOL WcrMoveObj_MoveFuncKuruKuru( WIFI_MATCHROOM* p_mcr, MCR_MOVEOBJ* p_obj )
{
	s32 way;
	u32 playid, myplayid;
	
	p_obj->move_count --;
	if( p_obj->move_count <= 0 ){

		// くるくるアニメ終了
		WF2DMAP_OBJDrawWkKuruAnimeEnd( p_obj->p_draw );

		// アップデート開始
		WF2DMAP_OBJDrawWkUpdataFlagSet( p_obj->p_draw, TRUE );

		playid = WF2DMAP_OBJWkDataGet( p_mcr->p_player->p_obj, WF2DMAP_OBJPM_PLID );
		myplayid = WF2DMAP_OBJWkDataGet( p_obj->p_obj, WF2DMAP_OBJPM_PLID );
		
		// 初期化完了
		if( myplayid == playid ){

			// プレイヤー
			WcrMoveObj_SetMoveFuncPlayer( p_mcr, p_obj );
		}else{
			// NPC
			WcrMoveObj_MoveSet( p_mcr, p_obj, p_obj->moveID );
		}
	}
	return FALSE;
}

static BOOL WcrMoveObj_MoveFuncKuruKuruDel( WIFI_MATCHROOM* p_mcr, MCR_MOVEOBJ* p_obj )
{
	p_obj->move_count ++;
	if( p_obj->move_count > MCR_MOVEOBJ_KURU_COUNT ){
		return TRUE;
	}
	return FALSE;
}

// NPC動作
static BOOL WcrMoveObj_MoveFuncNpc( WIFI_MATCHROOM* p_mcr, MCR_MOVEOBJ* p_obj )
{
	u32 way;
	u32 playid;
	
	p_obj->move_count --;
	if( p_obj->move_count <= 0 ){

		p_obj->move_count = MCR_MOVEOBJ_NPC_RAND_S + (gf_mtRand() % MCR_MOVEOBJ_NPC_RAND_M);
	
		// ランダムで方向を変える
		way = gf_mtRand() % MCR_MOVEOBJ_WAY_NUM;
			
		// リクエストコマンド設定
		// その方向に変更
		playid = WF2DMAP_OBJWkDataGet( p_obj->p_obj, WF2DMAP_OBJPM_PLID );
		WcrMoveObj_ReqCmdSet( p_mcr, WF2DMAP_CMD_NONE, way, playid );
	}
	return FALSE;
}

// NPC動作　ジャンプ
static BOOL WcrMoveObj_MoveFuncNpcJump( WIFI_MATCHROOM* p_mcr, MCR_MOVEOBJ* p_obj )
{
	p_obj->move_count --;
	if( p_obj->move_count < 0 ){
		p_obj->move_count = MCR_MOVEOBJ_NPC_JUMP_COUNT;
	}
	return FALSE;
}

// NPC動作停止
static BOOL WcrMoveObj_MoveFuncNpcPause( WIFI_MATCHROOM* p_mcr, MCR_MOVEOBJ* p_obj )
{
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	リクエストコマンドを設定する
 *
 *	@param	p_mcr		システムワーク
 *	@param	cmd			コマンド
 *	@param	way			方向
 *	@param	playid		プレイヤーID
 */
//-----------------------------------------------------------------------------
static void WcrMoveObj_ReqCmdSet( WIFI_MATCHROOM* p_mcr, s32 cmd, s32 way, s32 playid )
{
	WF2DMAP_REQCMD reqcmd;
	
	reqcmd.cmd = cmd;
	reqcmd.way = way;
	reqcmd.playid = playid;
	WF2DMAP_REQCMDQSysCmdPush( p_mcr->p_reqcmdQ, &reqcmd );
}

//----------------------------------------------------------------------------
/**
 *	@brief	簡易コマンド設定
 *
 *	@param	p_mcr	システムワーク
 *	@param	cmd		設定コマンド
 *	@param	cp_obj	設定オブジェクト
 */
//-----------------------------------------------------------------------------
static void WcrMoveObj_ReqCmdSetEasy( WIFI_MATCHROOM* p_mcr, s32 cmd, const MCR_MOVEOBJ* cp_obj )
{
	s32 way;
	s32 playid;

	way = WF2DMAP_OBJWkDataGet( cp_obj->p_obj, WF2DMAP_OBJPM_WAY );
	playid = WF2DMAP_OBJWkDataGet( cp_obj->p_obj, WF2DMAP_OBJPM_PLID );

	WcrMoveObj_ReqCmdSet( p_mcr, cmd, way, playid );
}

//----------------------------------------------------------------------------
/**
 *	@brief	NPCの動作タイプを変更しても大丈夫かチェック
 *
 *	@param	cp_obj	オブジェクトワーク
 *
 *	@retval	TRUE	動作設定　OK
 *	@retval	FALSE	動作変更　NG
 */
//-----------------------------------------------------------------------------
static BOOL WcrMoveObj_MoveSetOkCheck( const MCR_MOVEOBJ* cp_obj )
{
	// くるくる描画中はだめ
	if( (u32)cp_obj->pDraw == (u32)WcrMoveObj_DrawFuncKuruKuru ){
		return FALSE;
	}
	return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	動作設定
 *
 *	@param	p_obj		オブジェクトワーク
 *	@param	moveID		動作タイプ
 */
//-----------------------------------------------------------------------------
static void WcrMoveObj_MoveSet( WIFI_MATCHROOM* p_mcr, MCR_MOVEOBJ* p_obj, MCR_NPC_MOVETYPE moveID )
{	
	// 描画関数を変更
	switch( moveID ){
	case MCR_NPC_MOVE_NORMAL:
		WcrMoveObj_SetMoveFuncNpc( p_mcr, p_obj );
		break;
		
	case MCR_NPC_MOVE_JUMP:
		WcrMoveObj_SetMoveFuncNpcJump( p_mcr, p_obj );
		break;

	default:
		// そんなもんない
		GF_ASSERT( 0 );
		break;
	}
	p_obj->moveID = moveID;
}

//----------------------------------------------------------------------------
/**
 *	@brief	PCアニメシステム　初期化
 *
 *	@param	p_sys		システム
 *	@param	p_wk		ワーク
 *	@param	p_handle	アーカイブハンドル
 */
//-----------------------------------------------------------------------------
static void WcrPCANM_Init( WIFI_MATCHROOM* p_sys, MCR_PCANM* p_wk, ARCHANDLE* p_handle )
{
	// ワークのクリア
	memset( p_wk, 0, sizeof(MCR_PCANM) );
	
	// リソース読込み
	p_wk->p_plbuff = ArcUtil_HDL_PalDataGet( p_handle, 
			NARC_wifip2pmatch_wf_match_top_room_pc_NCLR, &p_wk->p_pltt, p_sys->use_heap );

	// 全PCアニメ開始
	p_wk->all_pcbitmap = (1<<0) | (1<<1) | (1<<2) | (1<<3);
	p_wk->all_seq = MCR_PCANM_ALL_SEQ_OFF;
	p_wk->all_count = AllAnmData[p_wk->all_seq];
}

//----------------------------------------------------------------------------
/**
 *	@brief	PCアニメシステム　破棄
 *
 *	@param	p_sys		システム
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void WcrPCANM_Delete( WIFI_MATCHROOM* p_sys, MCR_PCANM* p_wk )
{
	// ワークの破棄
	sys_FreeMemoryEz( p_wk->p_plbuff );
	memset( p_wk, 0, sizeof(MCR_PCANM) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	PCアニメ　メイン
 *
 *	@param	p_sys	システムワーク
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WcrPCANM_Main( WIFI_MATCHROOM* p_sys, MCR_PCANM* p_wk )
{
	// 待機アニメメイン
	WcrPCANM_AllMain( p_wk );	

	// 使用中アニメメイン
	WcrPCANM_UseMain( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	待機アニメメイン
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WcrPCANM_AllMain( MCR_PCANM* p_wk )
{
	int i;
	u32 on_off;

	// 待機アニメ
	p_wk->all_count --;
	if( p_wk->all_count <= 0 ){
		p_wk->all_seq = (p_wk->all_seq + 1) % MCR_PCANM_ALL_SEQ_NUM;
		p_wk->all_count = AllAnmData[p_wk->all_seq];

		// パレット転送
		if( p_wk->all_seq == MCR_PCANM_ALL_SEQ_ON ){
			on_off = MCR_PCANM_COL_WAIT;
		}else{
			on_off = MCR_PCANM_COL_OFF;
		}
		for( i=0; i<MCR_PCANM_PCNUM; i++ ){
			if( p_wk->all_pcbitmap & (1<<i) ){
				AddVramTransferManager( NNS_GFD_DST_2D_BG_PLTT_MAIN, 
						MCR_PCANM_DESTPL(i), WcrPCANM_GetAnmSrc(p_wk, on_off ), 2 );

//				TOMOYA_PRINT( "転送処理 %d dest[0x%x] src[0x%x] \n", on_off, MCR_PCANM_DESTPL(i), WcrPCANM_GetAnmSrc(p_wk, on_off ) );
			}
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	使用中アニメメイン
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WcrPCANM_UseMain( MCR_PCANM* p_wk )
{
	BOOL trans = FALSE;
	u32 on_off = FALSE;
	BOOL next = FALSE;


	// 基本的にはAnmDataのカウント分進むと次のアニメデータに進んでいきます。
	// カウントが無いデータの時は、use_pc_nextフラグが設定されたら次に進みます。
	// パレットの転送はAnmDataのtransメンバとtrans_flagメンバを見て決めます

	// カウントシステム
	if( AnmData[ p_wk->use_pc_seq ].flag == TRUE ){
		p_wk->use_pc_count --;
		if( p_wk->use_pc_count <= 0 ){
			next = TRUE;
		}
	}else{
		// 次に進むフラグが足っていたら次へ
		if( p_wk->use_pc_next ){
			p_wk->use_pc_next = FALSE;
			next = TRUE;
		}
	}

	// 次のシーケンスへ
	if( next ){
		p_wk->use_pc_seq ++;

		// 終了チェック
		if( p_wk->use_pc_seq >= MCR_PCANM_USE_SEQ_NUM ){
			WcrPCANM_UseAnmEnd( p_wk );
			return ;
		}

		// カウント数など設定
		p_wk->use_pc_count = AnmData[ p_wk->use_pc_seq ].count_num;
		trans = AnmData[ p_wk->use_pc_seq ].trans;
		on_off = AnmData[ p_wk->use_pc_seq ].trans_flag;
	}
	

	// 転送処理
	if( trans ){
		AddVramTransferManager( NNS_GFD_DST_2D_BG_PLTT_MAIN, 
				MCR_PCANM_DESTPL(p_wk->use_pc), WcrPCANM_GetAnmSrc(p_wk, on_off ), 2 );
//		TOMOYA_PRINT( "転送処理 %d dest[0x%x] src[0x%x] \n", on_off, MCR_PCANM_DESTPL(p_wk->use_pc), WcrPCANM_GetAnmSrc(p_wk, on_off ) );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	アニメのパレットデータを取得する
 *
 *	@param	p_wk		ワーク
 *	@param	on_off		TRUE：ON		FALSE：OFF
 */
//-----------------------------------------------------------------------------
static void* WcrPCANM_GetAnmSrc( MCR_PCANM* p_wk, u32 on_off )
{
	u16* p_data;

	// データ取得
	p_data = (u16*)p_wk->p_pltt->pRawData;

	return &p_data[ on_off+MCR_PCANM_BGPLTTOFS ];
}

//----------------------------------------------------------------------------
/**
 *	@brief	使用中アニメクリーン
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WcrPCANM_UseAnmEnd( MCR_PCANM* p_wk )
{
	// 待機アニメに合流させる
	p_wk->all_pcbitmap |= 1<<p_wk->use_pc;
	
	p_wk->use_pc = 0;
	p_wk->use_pc_seq = MCR_PCANM_USE_SEQ_NONE;
	p_wk->use_pc_count = 0;
	p_wk->use_pc_next = 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief	使用中アニメを次に進める処理
 *
 *	@param	p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void WcrPCANM_UseAnmNext( MCR_PCANM* p_wk )
{
	p_wk->use_pc_next = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	PCアニメ開始
 *
 *	@param	p_wk	ワーク
 *	@param	pc_no	パレットNO
 */
//-----------------------------------------------------------------------------
static void WcrPCANM_UseStart( MCR_PCANM* p_wk, u8 pc_no )
{
	// 今途中のものがあるか
	if( p_wk->use_pc_seq != MCR_PCANM_USE_SEQ_NONE ){
		// 今実行中のものを終わらせる
		WcrPCANM_UseAnmEnd( p_wk );
	}

	// 開始
	p_wk->all_pcbitmap ^= 1<<pc_no;
	p_wk->use_pc = pc_no;
	WcrPCANM_UseAnmNext( p_wk );

//	TOMOYA_PRINT( "use anm start\n" );
}

//----------------------------------------------------------------------------
/**
 *	@brief	PCアニメ終了
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WcrPCANM_UseEndReq( MCR_PCANM* p_wk )
{
	if( p_wk->use_pc_seq != MCR_PCANM_USE_SEQ_NONE ){
		WcrPCANM_UseAnmNext( p_wk );
//		TOMOYA_PRINT( "use anm end req\n" );
	}
}
