//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		lobby_news_data.h
 *	@brief		ロビーニュース	データ格納部分
 *	@author		tomoya takahashi
 *	@data		2007.10.09
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __LOBBY_NEWS_DATA_H__
#define __LOBBY_NEWS_DATA_H__

#include "gflib/strbuf.h"

#include "savedata/mystatus.h"

#include "wflby_def.h"

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	ROOM表示　特殊状態
//=====================================
typedef enum {
	NEWS_ROOMSP_NONE	= 0,
	NEWS_ROOMSP_MYDATA	= 1 << 0,
	NEWS_ROOMSP_OLD		= 1 << 1,
} NEWS_ROOMSP;

//-------------------------------------
///	ROOMイベント
//=====================================
typedef enum {
	NEWS_ROOMEV_NONE,
	NEWS_ROOMEV_IN,		// 誰か入ってきた
	NEWS_ROOMEV_OUT,	// 誰か出て行った
} NEWS_ROOMEV;



//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	WiFiロビーニュースデータベース
//=====================================
typedef struct _NEWS_DATA NEWS_DATA;





//-------------------------------------
///	ロビートピック設定構造体郡
//=====================================
// 会話データ
typedef struct {
	const MYSTATUS* cp_p1;
	const MYSTATUS* cp_p2;
	u16				idx_p1;
	u16				idx_p2;
} NEWS_DATA_SET_CONNECT;

// ガジェット交換データ
typedef struct {
	const MYSTATUS* cp_p1;
	const MYSTATUS* cp_p2;
	u16				idx_p1;
	u16				idx_p2;
	WFLBY_ITEMTYPE	item;
} NEWS_DATA_SET_ITEM;

// ミニゲームデータ
typedef struct {
	WFLBY_GAMETYPE minigame;
	u32 num;
	const MYSTATUS* cp_p1;
	const MYSTATUS* cp_p2;
	const MYSTATUS* cp_p3;
	const MYSTATUS* cp_p4;
	u16				idx_p1;
	u16				idx_p2;
	u16				idx_p3;
	u16				idx_p4;
	BOOL play;
} NEWS_DATA_SET_MINIGAME;

// 足跡ボード
typedef struct {
	WFLBY_GAMETYPE board;
	u32 num;
	const MYSTATUS* cp_p1;
	u16				idx_p1;
} NEWS_DATA_SET_FOOTBOARD;

// 世界時計
typedef struct {
	u32 num;
	const MYSTATUS* cp_p1;
	const MYSTATUS* cp_p2;
	const MYSTATUS* cp_p3;
	const MYSTATUS* cp_p4;
	u16				idx_p1;
	u16				idx_p2;
	u16				idx_p3;
	u16				idx_p4;
} NEWS_DATA_SET_WORLDTIMER;

// ロビーニュース
typedef struct {
	u32 num;
	const MYSTATUS* cp_p1;
	const MYSTATUS* cp_p2;
	const MYSTATUS* cp_p3;
	const MYSTATUS* cp_p4;
	u16				idx_p1;
	u16				idx_p2;
	u16				idx_p3;
	u16				idx_p4;
} NEWS_DATA_SET_LOBBYNEWS;

// イベント
typedef struct {
	const WFLBY_TIME*	cp_time;
	WFLBY_EVENTTYPE		event_no;
} NEWS_DATA_SET_TIMEEVENT;

// VIP入場
typedef struct {
	u32	vip_plidx;
} NEWS_DATA_SET_VIPIN;

// ミニゲーム結果
typedef struct {
	WFLBY_GAMETYPE minigame;
	u32 num;
	const MYSTATUS* cp_p1;
	const MYSTATUS* cp_p2;
	const MYSTATUS* cp_p3;
	const MYSTATUS* cp_p4;
	u16				idx_p1;
	u16				idx_p2;
	u16				idx_p3;
	u16				idx_p4;
} NEWS_DATA_SET_MGRESULT;



//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
extern NEWS_DATA* NEWS_DSET_Init( u32 heapID );
extern void NEWS_DSET_Exit( NEWS_DATA* p_data );
extern void NEWS_DSET_Main( NEWS_DATA* p_data );	// トピックの破棄処理などを行う
	// NEWS_DSET_Mainと各データ設定のタイミングに注意してください。
	// Main関数内で、入室イベントフラグの破棄などを行う為
	// 描画に反映されなくなる可能性があります。
	//
	// データ設定後１シンクMainが呼ばれないようにしておく必要があります。
	//
	// Main
	// ｜
	// データ設定
	// ｜
	// 描画処理
	//
	// この流れが理想です。

// 時間データ設定
extern void NEWS_DSET_SetLockTime( NEWS_DATA* p_data, const WFLBY_TIME* cp_time );

// 入退室設定
extern void NEWS_DSET_SetRoomIn( NEWS_DATA* p_data, u32 player,  u32 sex, NEWS_ROOMSP special_msk );
extern BOOL NEWS_DSET_SetRoomOut( NEWS_DATA* p_data, u32 player );
extern void NEWS_DSET_SetRoomOld( NEWS_DATA* p_data, u32 player );

// トピックス設定
extern void NEWS_DSET_SetConnect( NEWS_DATA* p_data,  const NEWS_DATA_SET_CONNECT* cp_data );
extern void NEWS_DSET_SetItem( NEWS_DATA* p_data, const NEWS_DATA_SET_ITEM* cp_data );
extern void NEWS_DSET_SetMiniGame( NEWS_DATA* p_data, const NEWS_DATA_SET_MINIGAME* cp_data );
extern void NEWS_DSET_SetFootBoard( NEWS_DATA* p_data, const NEWS_DATA_SET_FOOTBOARD* cp_data );
extern void NEWS_DSET_SetWorldTimer( NEWS_DATA* p_data, const NEWS_DATA_SET_WORLDTIMER* cp_data );
extern void NEWS_DSET_SetLobbyNews( NEWS_DATA* p_data, const NEWS_DATA_SET_LOBBYNEWS* cp_data );
extern void NEWS_DSET_SetTimeEvent( NEWS_DATA* p_data, const NEWS_DATA_SET_TIMEEVENT* cp_data );
extern void NEWS_DSET_SetVipIn( NEWS_DATA* p_data, const NEWS_DATA_SET_VIPIN* cp_data, const WFLBY_VIPFLAG* cp_vip );
extern void NEWS_DSET_SetMgResult( NEWS_DATA* p_data, const NEWS_DATA_SET_MGRESULT* cp_data );

// トピックスの破棄
extern void NEWS_DSET_RemoveTopTopic( NEWS_DATA* p_data );

// LOCK時間取得
extern BOOL NEWS_DSET_GetLockTime( const NEWS_DATA* cp_data, WFLBY_TIME* p_time );

// プレイヤーデータ取得
extern BOOL NEWS_DSET_CheckPlayerInside( const NEWS_DATA* cp_wk, u32 player );
extern u32 NEWS_DSET_GetPlayerSex( const NEWS_DATA* cp_wk, u32 player );
extern BOOL NEWS_DSET_GetPlayerSpecialMy( const NEWS_DATA* cp_wk, u32 player );
extern BOOL NEWS_DSET_GetPlayerSpecialOld( const NEWS_DATA* cp_wk, u32 player );
extern BOOL NEWS_DSET_GetPlayerNewPlayer( const NEWS_DATA* cp_wk, u32 player );
extern NEWS_ROOMEV NEWS_DSET_GetPlayerEvent( const NEWS_DATA* cp_wk );

// トピックデータ取得
// トピックデータは文字列の形になって取得される
extern BOOL NEWS_DSET_CheckToppic( const NEWS_DATA* cp_wk );
extern NEWS_TOPICTYPE NEWS_DSET_GetTopicType( const NEWS_DATA* cp_wk ); 
extern BOOL NEWS_DSET_GetTopicData( const NEWS_DATA* cp_wk, const WFLBY_VIPFLAG* cp_vip, NEWS_TOPICTYPE type, STRBUF* p_str, u32 heapID );
extern u32 NEWS_DSET_GetTopicUserIdx( const NEWS_DATA* cp_wk, NEWS_TOPICTYPE type, u32 count );

#endif		// __LOBBY_NEWS_DATA_H__

