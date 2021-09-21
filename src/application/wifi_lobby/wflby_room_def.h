//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wflby_room_def.h
 *	@brief		WiFiロビー	部屋システム	構造体定義
 *	@author		tomoya takahashi
 *	@data		2007.11.26
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WFLBY_ROOM_DEF_H__
#define __WFLBY_ROOM_DEF_H__

#include "gflib/strbuf.h"

#include "system/clact_util.h"
#include "system/render_oam.h"
#include "system/bmp_list.h"

#include "savedata/mystatus.h"

#include "wflby_3dobjcont.h"
#include "wflby_3dmapobj_cont.h"
#include "wflby_mapcont.h"
#include "wflby_camera.h"
#include "wflby_event_def.h"

#include "wflby_room.h"


//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
/// 広場専用メッセージ
//=====================================
typedef enum{
	WFLBY_DEFMSG_TYPE_INFO,
	WFLBY_DEFMSG_TYPE_HIROBA,
	WFLBY_DEFMSG_TYPE_ERR,
	WFLBY_DEFMSG_TYPE_AISATSU,
	WFLBY_DEFMSG_TYPE_NUM,
} WFLBY_DEFMSG_TYPE;

//-------------------------------------
///	広場YESNOウィンドウ戻り値
//=====================================
typedef enum {
	WFLBY_ROOM_YESNO_OK,	// はい
	WFLBY_ROOM_YESNO_NO,	// いいえ
	WFLBY_ROOM_YESNO_WAIT,	// 選び中
} WFLBY_ROOM_YESNO_RET;

//-------------------------------------
///	プレイヤー入場常駐イベント定数
//=====================================
enum {
	WFLBY_EV_DEF_PLAYERIN_NORMAL,	// 通常入場
	WFLBY_EV_DEF_PLAYERIN_GRID,		// グリッドを指定して入場(SUBCHANからの退避)
	WFLBY_EV_DEF_PLAYERIN_MINIGAME,	// グリッドを指定して入場(ミニゲームからの退避)
	WFLBY_EV_DEF_PLAYERIN_ANKETO,	// グリッドを指定して入場(アンケートからの退避)
	WFLBY_EV_DEF_PLAYERIN_NUM,
} ;



//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
// 部屋のワーク
typedef struct _WFLBY_ROOMWK WFLBY_ROOMWK;


//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
 */
//-----------------------------------------------------------------------------
extern WFLBY_SYSTEM* WFLBY_ROOM_GetSystemData( WFLBY_ROOMWK* p_wk );
extern WFLBY_ROOM_SAVE* WFLBY_ROOM_GetSaveData( WFLBY_ROOMWK* p_wk );
extern WFLBY_3DOBJCONT*	WFLBY_ROOM_GetObjCont( WFLBY_ROOMWK* p_wk );
extern WFLBY_3DMAPOBJ_CONT* WFLBY_ROOM_GetMapObjCont( WFLBY_ROOMWK* p_wk );
extern WFLBY_MAPCONT* WFLBY_ROOM_GetMapCont( WFLBY_ROOMWK* p_wk );
extern WFLBY_CAMERA* WFLBY_ROOM_GetCamera( WFLBY_ROOMWK* p_wk );
extern WFLBY_EVENT* WFLBY_ROOM_GetEvent( WFLBY_ROOMWK* p_wk );	


// 通信エラーチェックのONOFF
// 初期値FALSE
extern void WFLBY_ROOM_SetErrorCheckFlag( WFLBY_ROOMWK* p_wk, BOOL flag );
extern BOOL WFLBY_ROOM_GetErrorCheckFlag( const WFLBY_ROOMWK* cp_wk );

// フラグ関連
extern u32 WFLBY_ROOM_GetPlno( const WFLBY_ROOMWK* cp_wk );
extern BOOL WFLBY_ROOM_GetFadeFlag( const WFLBY_ROOMWK* cp_wk );
extern BOOL WFLBY_ROOM_GetEndFlag( const WFLBY_ROOMWK* cp_wk );
extern u8 WFLBY_ROOM_GetEndRet( const WFLBY_ROOMWK* cp_wk );
extern void WFLBY_ROOM_SetEndFlag( WFLBY_ROOMWK* p_wk, BOOL flag );
extern void WFLBY_ROOM_SetEndRet( WFLBY_ROOMWK* p_wk,  u8 retdata );
extern void WFLBY_ROOM_SetEvNo( WFLBY_ROOMWK* p_wk, u8 evno );
extern u8 WFLBY_ROOM_GetEvNo( const WFLBY_ROOMWK* cp_wk );
extern void WFLBY_ROOM_OffTimeOutCheck( WFLBY_ROOMWK* p_wk );

#if PL_T0867_080716_FIX
// 主人公動作
extern void WFLBY_ROOM_SetPlayerEventAfterMove( WFLBY_ROOMWK* p_wk, BOOL flag );
extern BOOL WFLBY_ROOM_CheckPlayerEventAfterMove( const WFLBY_ROOMWK* cp_wk );
#endif

// NPCローカル環境会話可能チェック
extern void WFLBY_ROOM_SetNpcTalkBusy( WFLBY_ROOMWK* p_wk, u8 plno, BOOL busy );
extern BOOL WFBLY_ROOM_CheckNpcTalkBusy( const WFLBY_ROOMWK* cp_wk, u8 plno );
// NPCローカル環境情報表示可能チェック
extern void WFLBY_ROOM_SetNpcInfoDrawBusy( WFLBY_ROOMWK* p_wk, u8 plno, BOOL busy );
extern BOOL WFBLY_ROOM_CheckNpcInfoDrawBusy( const WFLBY_ROOMWK* cp_wk, u8 plno );


extern void WFLBY_ROOM_TALKWIN_PrintStr( WFLBY_ROOMWK* p_wk, const STRBUF* cp_str );
extern void WFLBY_ROOM_TALKWIN_PrintStrAllPut( WFLBY_ROOMWK* p_wk, const STRBUF* cp_str );
extern BOOL WFLBY_ROOM_TALKWIN_WaitEnd( const WFLBY_ROOMWK* cp_wk );
extern void WFLBY_ROOM_TALKWIN_StartTimeWait( WFLBY_ROOMWK* p_wk );
extern void WFLBY_ROOM_TALKWIN_StopTimeWait( WFLBY_ROOMWK* p_wk );
extern BOOL WFLBY_ROOM_TALKWIN_CheckTimeWait( const WFLBY_ROOMWK* cp_wk );
extern void WFLBY_ROOM_TALKWIN_Off( WFLBY_ROOMWK* p_wk );


extern void WFLBY_ROOM_BOARDWIN_PrintStr( WFLBY_ROOMWK* p_wk, const STRBUF* cp_str );
extern void WFLBY_ROOM_BOARDWIN_PrintStrAllPut( WFLBY_ROOMWK* p_wk, const STRBUF* cp_str );
extern BOOL WFLBY_ROOM_BOARDWIN_WaitEnd( const WFLBY_ROOMWK* cp_wk );
extern void WFLBY_ROOM_BOARDWIN_Off( WFLBY_ROOMWK* p_wk );


extern void WFLBY_ROOM_LISTWIN_CreateBmpList( WFLBY_ROOMWK* p_wk, u32 num );
// MatchComment: new function
extern void WFLBY_ROOM_LISTWIN_CreateBmpList_WithThirdArg( WFLBY_ROOMWK* p_wk, u32 num, u32 a2);
extern void WFLBY_ROOM_LISTWIN_DeleteBmpList( WFLBY_ROOMWK* p_wk );
extern void WFLBY_ROOM_LISTWIN_SetBmpListStr( WFLBY_ROOMWK* p_wk, const STRBUF* cp_str, u32 param );
extern const BMP_MENULIST_DATA* WFLBY_ROOM_LISTWIN_GetBmpList( const WFLBY_ROOMWK* cp_wk );
extern BOOL WFLBY_ROOM_LISTWIN_CheckBmpListParam( const WFLBY_ROOMWK* cp_wk, u32 param );
extern void WFLBY_ROOM_LISTWIN_Start( WFLBY_ROOMWK* p_wk, const BMPLIST_HEADER* cp_data, u16 list_p, u16 cursor_p );
extern void WFLBY_ROOM_LISTWIN_Start_Ex( WFLBY_ROOMWK* p_wk, const BMPLIST_HEADER* cp_data, u16 list_p, u16 cursor_p, u8 cx, u8 cy, u8 szcx );
extern u32 WFLBY_ROOM_LISTWIN_Main( WFLBY_ROOMWK* p_wk );
extern void WFLBY_ROOM_LISTWIN_End( WFLBY_ROOMWK* p_wk, u16* p_list_p, u16* p_cursor_p );
extern void WFLBY_ROOM_LISTWIN_YAZIRUSHI_SetDraw( WFLBY_ROOMWK* p_wk, BOOL flag );

extern void WFLBY_ROOM_SUBWIN_Start( WFLBY_ROOMWK* p_wk, u8 cx, u8 cy, u8 szcx, u8 szcy );
extern void WFLBY_ROOM_SUBWIN_End( WFLBY_ROOMWK* p_wk );
extern void WFLBY_ROOM_SUBWIN_Print( WFLBY_ROOMWK* p_wk, const STRBUF* cp_str, u8 x, u8 y );
extern void WFLBY_ROOM_SUBWIN_Clear( WFLBY_ROOMWK* p_wk );
extern void WFLBY_ROOM_SUBWIN_ClearRect( WFLBY_ROOMWK* p_wk, u16 x, u16 y, u16 sx, u16 sy );


extern void WFLBY_ROOM_YESNOWIN_Start( WFLBY_ROOMWK* p_wk );
extern void WFLBY_ROOM_YESNOWIN_StartNo( WFLBY_ROOMWK* p_wk );
extern WFLBY_ROOM_YESNO_RET WFLBY_ROOM_YESNOWIN_Main( WFLBY_ROOMWK* p_wk );
extern void WFLBY_ROOM_YESNOWIN_End( WFLBY_ROOMWK* p_wk );

extern STRBUF* WFLBY_ROOM_MSG_Get( WFLBY_ROOMWK* p_wk, WFLBY_DEFMSG_TYPE type, u32 stridx );
extern void WFLBY_ROOM_MSG_SetNumber( WFLBY_ROOMWK* p_wk, u32  num, u32 keta, u32 bufid, NUMBER_DISPTYPE disptype );
extern void WFLBY_ROOM_MSG_SetPlayerName( WFLBY_ROOMWK* p_wk, const MYSTATUS* cp_mystatus, u32 bufid );
extern void WFLBY_ROOM_MSG_SetIdxPlayerName( WFLBY_ROOMWK* p_wk, u32 idx, u32 bufid );
extern void WFLBY_ROOM_MSG_SetMinigame( WFLBY_ROOMWK* p_wk, WFLBY_GAMETYPE type, u32 bufid );
extern void WFLBY_ROOM_MSG_SetTimeEvent( WFLBY_ROOMWK* p_wk, WFLBY_EVENTGMM_TYPE type, u32 bufid );
extern void WFLBY_ROOM_MSG_SetItem( WFLBY_ROOMWK* p_wk, WFLBY_ITEMTYPE item, u32 bufid );
extern void WFLBY_ROOM_MSG_ClearWordSet( WFLBY_ROOMWK* p_wk );
extern void WFLBY_ROOM_MSG_SetAisatsuJapan( WFLBY_ROOMWK* p_wk, u32 bufID, u32 time_zone );
extern void WFLBY_ROOM_MSG_SetAisatsuEnglish( WFLBY_ROOMWK* p_wk, u32 bufID, u32 time_zone );
extern void WFLBY_ROOM_MSG_SetAisatsuFrance( WFLBY_ROOMWK* p_wk, u32 bufID, u32 time_zone );
extern void WFLBY_ROOM_MSG_SetAisatsuItaly( WFLBY_ROOMWK* p_wk, u32 bufID, u32 time_zone );
extern void WFLBY_ROOM_MSG_SetAisatsuGerMany( WFLBY_ROOMWK* p_wk, u32 bufID, u32 time_zone );
extern void WFLBY_ROOM_MSG_SetAisatsuSpain( WFLBY_ROOMWK* p_wk, u32 bufID, u32 time_zone );
extern void WFLBY_ROOM_MSG_SetWazaType( WFLBY_ROOMWK* p_wk, u32 bufID, WFLBY_POKEWAZA_TYPE type );

// した画面コントロール
extern void WFLBY_ROOM_UNDERWIN_TrCardOn( WFLBY_ROOMWK* p_wk, u32 idx, BOOL aikotoba );
extern void WFLBY_ROOM_UNDERWIN_TrCardGadGetChange( WFLBY_ROOMWK* p_wk, WFLBY_ITEMTYPE item );
extern void WFLBY_ROOM_UNDERWIN_TrCardWazaTypeWrite( WFLBY_ROOMWK* p_wk );
extern void WFLBY_ROOM_UNDERWIN_TrCardOff( WFLBY_ROOMWK* p_wk );
extern u32 WFLBY_ROOM_UNDERWIN_TrCardGetPlIdx( const WFLBY_ROOMWK* cp_wk );
extern BOOL WFLBY_ROOM_UNDERWIN_TrCardGetDraw( const WFLBY_ROOMWK* cp_wk );
extern BOOL WFLBY_ROOM_UNDERWIN_TrCardCheckCanOff( const WFLBY_ROOMWK* cp_wk );
extern BOOL WFLBY_ROOM_UNDERWIN_CheckSeqNormal( const WFLBY_ROOMWK* cp_wk );

// ガジェットを起動するかチェック
// チェック後フラグが自動でOFFになります
extern void WFLBY_ROOM_GadGet_Get( WFLBY_ROOMWK* p_wk );
extern void WFLBY_ROOM_GadGet_SetStop( WFLBY_ROOMWK* p_wk, BOOL stop );
extern void WFLBY_ROOM_GadGet_ClearLockWait( WFLBY_ROOMWK* p_wk );
extern BOOL WFLBY_ROOM_GadGet_GetFlag( WFLBY_ROOMWK* p_wk );
extern void WFLBY_ROOM_GadGet_Play( WFLBY_ROOMWK* p_wk, u32 idx );
extern BOOL WFLBY_ROOM_GadGet_PlayWait( const WFLBY_ROOMWK* cp_wk, u32 idx );
extern void WFLBY_ROOM_UNDERWIN_FloatBttnOn( WFLBY_ROOMWK* p_wk );
extern void WFLBY_ROOM_UNDERWIN_FloatBttnOff( WFLBY_ROOMWK* p_wk );

// フロートの揺れアニメ情報を取得
extern BOOL WFLBY_ROOM_FLOAT_CheckShakeAnm( const WFLBY_ROOMWK* cp_wk, u32 floatidx );

#endif		// __WFLBY_ROOM_DEF_H__

