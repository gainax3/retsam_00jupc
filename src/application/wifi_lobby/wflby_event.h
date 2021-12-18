//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wflby_event.h
 *	@brief		イベント管理システム
 *	@author		tomoya takahashi
 *	@data		2007.11.26
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WFLBY_EVENT_H__
#define __WFLBY_EVENT_H__

#include "wflby_event_def.h"
#include "wflby_room_def.h"

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	イベント型宣言
//	
//	p_wk	このイベントのワーク
//	p_rmwk	部屋の全データ
//	plno	対象となるプレイヤーNo（Globalイベントは常に０）
//
//	BOOL	TRUE:終了	FALSE:続行
//
//	TRUEを返すと、どのタイミングで呼ばれた関数であれ、イベントごと終了します。
//=====================================
typedef BOOL (*pEVENT_FUNC)( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno );


//-------------------------------------
///	イベント設定ワーク
// 使用しないタイミングの関数ポインタ変数にはNULLをいれてください
//=====================================
typedef struct {
	pEVENT_FUNC p_before;		// オブジェリクエスト設定前
	pEVENT_FUNC	p_after;		// オブジェリクエスト設定後
} WFLBY_EVENT_DATA;

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
// イベントシステム管理
//=====================================
extern WFLBY_EVENT* WFLBY_EVENT_Init( WFLBY_ROOMWK* p_rmwk, u32 heapID );
extern void WFLBY_EVENT_Exit( WFLBY_EVENT* p_sys );

//-------------------------------------
// 各実行関数
//=====================================
extern void WFLBY_EVENT_MainBefore( WFLBY_EVENT* p_sys );	// オブジェリクエスト設定前
extern void WFLBY_EVENT_MainAfter( WFLBY_EVENT* p_sys );	// オブジェリクエスト設定後

//-------------------------------------
// ワーク関数
//=====================================
extern void* WFLBY_EVENTWK_GetParam( WFLBY_EVENTWK* p_wk );
extern void* WFLBY_EVENTWK_AllocWk( WFLBY_EVENTWK* p_wk, u32 size );
extern void WFLBY_EVENTWK_DeleteWk( WFLBY_EVENTWK* p_wk );
extern void* WFLBY_EVENTWK_GetWk( WFLBY_EVENTWK* p_wk );
extern u32 WFLBY_EVENTWK_GetSeq( const WFLBY_EVENTWK* cp_wk );
extern void WFLBY_EVENTWK_SetSeq( WFLBY_EVENTWK* p_wk, u32 seq );
extern void WFLBY_EVENTWK_AddSeq( WFLBY_EVENTWK* p_wk );


//-------------------------------------
// 個別イベント管理
//=====================================
// 上書き	常にこの関数を呼ぶようになります。（常駐イベント）
extern void WFLBY_EVENT_SetPrivateEvent( WFLBY_EVENT* p_sys, u32 plno, const WFLBY_EVENT_DATA* cp_data, void* p_param );
// 割り込みイベント	Wait関数にTRUEが変えてくるまでその関数の中身を実行します。
// 実行後は、その前に設定されていた関数（常駐イベント）を呼びます
// この関数を読んだ後、シーケンスなどは設定したイベントのものになってしまいます。注意してください
extern void WFLBY_EVENT_SetPrivateIntrEvent( WFLBY_EVENT* p_sys, u32 plno, const WFLBY_EVENT_DATA* cp_data, void* p_param );
extern BOOL WFLBY_EVENT_WaitPrivateIntrEvent( const WFLBY_EVENT* cp_sys, u32 plno );

#ifdef PM_DEBUG
// デバック
extern void WFLBY_EVENT_SetPrivateEvent_DEBUG( WFLBY_EVENT* p_sys, u32 plno, const WFLBY_EVENT_DATA* cp_data, void* p_param, u32 line );
extern void WFLBY_EVENT_SetPrivateIntrEvent_DEBUG( WFLBY_EVENT* p_sys, u32 plno, const WFLBY_EVENT_DATA* cp_data, void* p_param, u32 line );

#define WFLBY_EVENT_SET_PRIVATE_EVENT( p_sys, plno, cp_data, p_param )		WFLBY_EVENT_SetPrivateEvent_DEBUG( p_sys, plno, cp_data, p_param, __LINE__ )
#define WFLBY_EVENT_SET_PRIVATE_INTR_EVENT( p_sys, plno, cp_data, p_param )		WFLBY_EVENT_SetPrivateIntrEvent_DEBUG( p_sys, plno, cp_data, p_param, __LINE__ )

#else

//　 リリース
#define WFLBY_EVENT_SET_PRIVATE_EVENT( p_sys, plno, cp_data, p_param )		WFLBY_EVENT_SetPrivateEvent( p_sys, plno, cp_data, p_param )
#define WFLBY_EVENT_SET_PRIVATE_INTR_EVENT( p_sys, plno, cp_data, p_param )		WFLBY_EVENT_SetPrivateIntrEvent( p_sys, plno, cp_data, p_param )

#endif

//  動作設定
//  イベントを停止させた人が責任を持って、再生させること！
extern void WFLBY_EVENT_SetPrivateEventMove( WFLBY_EVENT* p_sys, u32 plno, BOOL move );



//-------------------------------------
// 全体イベント管理
// 実行をしたら全体の動作を停止して、この処理を行います。
//=====================================
extern void WFLBY_EVENT_SetGlobalEvent( WFLBY_EVENT* p_sys, const WFLBY_EVENT_DATA* cp_data, void* p_param );
extern BOOL WFLBY_EVENT_WaitGlobalEvent( const WFLBY_EVENT* cp_sys );


#endif		// __WFLBY_EVENT_H__

