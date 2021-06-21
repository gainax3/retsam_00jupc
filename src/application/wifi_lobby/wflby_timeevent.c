//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wflby_timeevent.c
 *	@brief		タイムイベント
 *	@author		tomoya takahshi
 *	@data		2008.01.26
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "common.h"

#include "wflby_timeevent.h"
#include "wflby_light.h"
#include "wflby_float.h"
#include "wflby_fire.h"
#include "wflby_snd.h"

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
//#define WFLBY_DEBUG_LIGHT_CONT
#endif

#ifdef WFLBY_DEBUG_LIGHT_CONT
static s32 s_WFLBY_DEBUG_LIGHT_CONT_ROOM_COUNT;
static s32 s_WFLBY_DEBUG_LIGHT_CONT_FLOOR_COUNT;
#endif


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
///	タイムイベントワーク
//=====================================
typedef struct _WFLBY_TIMEEVENT{
	WFLBY_ROOMWK*		p_rmwk;
	WFLBY_SYSTEM*		p_system;
	WFLBY_3DMAPOBJ_CONT*p_mapobjcont;
	WFLBY_3DOBJCONT*	p_3dobjcont;
	WFLBY_LIGHTWK*		p_light;
	WFLBY_FLOAT_CONT*	p_floatcont;
	WFLBY_FIRE_CONT*	p_firecont;
}WFLBY_TIMEEVENT;

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
static u32 WFLBY_TIMEEVENT_GetNeonRoom( PPW_LOBBY_TIME_EVENT event );
static u32 WFLBY_TIMEEVENT_GetNeonFloor( PPW_LOBBY_TIME_EVENT event );
static u32 WFLBY_TIMEEVENT_GetNeonMonu( PPW_LOBBY_TIME_EVENT event );

static void WFLBY_TIMEEVENT_AplLightOff( WFLBY_TIMEEVENT* p_wk );

//----------------------------------------------------------------------------
/**
 *	@brief	タイムイベント管理システム	初期化
 *
 *	@param	heapID		ヒープ
 *	@param	p_rmwk		部屋ワーク
 */
//-----------------------------------------------------------------------------
WFLBY_TIMEEVENT* WFLBY_TIMEEVENT_Init( u32 heapID, WFLBY_ROOMWK* p_rmwk )
{
	WFLBY_TIMEEVENT* p_wk;
	u32 room, floor, monu;
	u32 hanabi, lock, parade, neon, close, minigame;
	

	p_wk = sys_AllocMemory( heapID, sizeof(WFLBY_TIMEEVENT) );
	memset( p_wk, 0, sizeof(WFLBY_TIMEEVENT) );

	p_wk->p_rmwk		= p_rmwk;
	p_wk->p_system		= WFLBY_ROOM_GetSystemData( p_rmwk );
	p_wk->p_mapobjcont	= WFLBY_ROOM_GetMapObjCont( p_rmwk );
	p_wk->p_3dobjcont	= WFLBY_ROOM_GetObjCont( p_rmwk );

	room	= WFLBY_TIMEEVENT_GetNeonRoom( WFLBY_SYSTEM_Event_GetNeonRoom( p_wk->p_system ) );
	floor	= WFLBY_TIMEEVENT_GetNeonFloor( WFLBY_SYSTEM_Event_GetNeonFloor( p_wk->p_system ) );
	monu	= WFLBY_TIMEEVENT_GetNeonMonu( WFLBY_SYSTEM_Event_GetNeonMonu( p_wk->p_system ) );

	//  ライト管理システム初期化
	p_wk->p_light		= WFLBY_LIGHT_Init( heapID, room, floor, monu );

	// フロート管理システム初期化
	p_wk->p_floatcont	= WFLBY_FLOAT_CONT_Init( p_rmwk, heapID );

	// 花火管理システム初期化
	p_wk->p_firecont	= WFLBY_FIRECONT_Init( p_wk->p_system, p_wk->p_mapobjcont, heapID );

	// 人物の影の色
	WFLBY_3DOBJCONT_InitShadowAlpha( p_wk->p_3dobjcont, room );


	// 各イベント状態取得
	hanabi	= WFLBY_SYSTEM_Event_GetHanabi( p_wk->p_system );
	lock	= WFLBY_SYSTEM_Event_GetLock( p_wk->p_system );
	parade	= WFLBY_SYSTEM_Event_GetParade( p_wk->p_system );
	neon	= WFLBY_SYSTEM_Event_GetNeon( p_wk->p_system );
	close	= WFLBY_SYSTEM_Event_GetEndCM( p_wk->p_system );
	minigame= WFLBY_SYSTEM_Event_GetMiniGameStop( p_wk->p_system );
	
	// 銅像の状態
	{
		if( hanabi == WFLBY_EVENT_HANABI_AFTER ){
			// 花火後なので、銅像の明かりを消す
			WFLBY_3DMAPOBJCONT_MAP_OffDouzouLight( p_wk->p_mapobjcont );
		}
	}

	// 床ライトの状態
	{
		// 花火が始まっているとき
		if(  hanabi != WFLBY_EVENT_HANABI_PLAY ){

			// 花火前なら最初から光を出す
			if( (hanabi == WFLBY_EVENT_HANABI_BEFORE) && (neon == TRUE) ){
				WFLBY_3DMAPOBJCONT_MAP_SetFloorLight( p_wk->p_mapobjcont, WFLBY_3DMAPOBJ_CONT_LAMP_YURE  );
			}
		}
	}

	// 策の状態
	{
		// パレードが始まっているとき
		if( parade == TRUE ){
			WFLBY_3DMAPOBJCONT_MAP_OnPoll( p_wk->p_mapobjcont );
		}
	}

	// クローズ
	{
		if( close == TRUE ){
			// アプリの明かりを消す
			WFLBY_TIMEEVENT_AplLightOff( p_wk );
		}
	}


	return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief	タイムイベント管理システム	破棄
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_TIMEEVENT_Exit( WFLBY_TIMEEVENT* p_wk )
{
	// 花火管理システム破棄
	WFLBY_FIRECONT_Exit( p_wk->p_firecont );

	// フロート管理システム破棄
	WFLBY_FLOAT_CONT_Exit( p_wk->p_floatcont );
	
	// ライト管理システム破棄
	WFLBY_LIGHT_Exit( p_wk->p_light );

	// ワーク破棄
	sys_FreeMemoryEz( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	タイムイベント管理システム	メイン
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_TIMEEVENT_Main( WFLBY_TIMEEVENT* p_wk )
{
#ifdef WFLBY_DEBUG_LIGHT_CONT
	if( sys.cont & PAD_BUTTON_A ){
		if( sys.trg & PAD_KEY_UP ){
			s_WFLBY_DEBUG_LIGHT_CONT_ROOM_COUNT = (s_WFLBY_DEBUG_LIGHT_CONT_ROOM_COUNT + 1) % WFLBY_LIGHT_NEON_ROOMNUM;
			OS_TPrintf( "room type %d\n", s_WFLBY_DEBUG_LIGHT_CONT_ROOM_COUNT );
			WFLBY_LIGHT_SetRoom( p_wk->p_light, s_WFLBY_DEBUG_LIGHT_CONT_ROOM_COUNT );
		}
		else if( sys.trg & PAD_KEY_DOWN ){
			s_WFLBY_DEBUG_LIGHT_CONT_ROOM_COUNT --;
			if( s_WFLBY_DEBUG_LIGHT_CONT_ROOM_COUNT < 0 ){
				s_WFLBY_DEBUG_LIGHT_CONT_ROOM_COUNT += WFLBY_LIGHT_NEON_ROOMNUM;
			}
			OS_TPrintf( "room type %d\n", s_WFLBY_DEBUG_LIGHT_CONT_ROOM_COUNT );
			WFLBY_LIGHT_SetRoom( p_wk->p_light, s_WFLBY_DEBUG_LIGHT_CONT_ROOM_COUNT );
		}
	}

	if( sys.cont & PAD_BUTTON_Y ){
		if( sys.trg & PAD_KEY_UP ){
			s_WFLBY_DEBUG_LIGHT_CONT_FLOOR_COUNT = (s_WFLBY_DEBUG_LIGHT_CONT_FLOOR_COUNT + 1) % WFLBY_LIGHT_NEON_FLOORNUM;
			OS_TPrintf( "floor type %d\n", s_WFLBY_DEBUG_LIGHT_CONT_FLOOR_COUNT );
			WFLBY_LIGHT_SetFloor( p_wk->p_light, s_WFLBY_DEBUG_LIGHT_CONT_FLOOR_COUNT );
		}
		else if( sys.trg & PAD_KEY_DOWN ){
			s_WFLBY_DEBUG_LIGHT_CONT_FLOOR_COUNT --;
			if( s_WFLBY_DEBUG_LIGHT_CONT_FLOOR_COUNT < 0 ){
				s_WFLBY_DEBUG_LIGHT_CONT_FLOOR_COUNT += WFLBY_LIGHT_NEON_FLOORNUM;
			}
			OS_TPrintf( "floor type %d\n", s_WFLBY_DEBUG_LIGHT_CONT_FLOOR_COUNT );
			WFLBY_LIGHT_SetFloor( p_wk->p_light, s_WFLBY_DEBUG_LIGHT_CONT_FLOOR_COUNT );
		}
	}
#endif
	
	// イベント発動チェック
	// 部屋ネオン
	if( WFLBY_SYSTEM_FLAG_GetEventStart( p_wk->p_system, WFLBY_EVENT_CHG_NEON_ROOM ) ){
		u32 room;
		room = WFLBY_TIMEEVENT_GetNeonRoom( WFLBY_SYSTEM_Event_GetNeonRoom( p_wk->p_system ) );
		WFLBY_LIGHT_SetRoom( p_wk->p_light, room );
		// 人物の影の色
		WFLBY_3DOBJCONT_SetShadowAlpha( p_wk->p_3dobjcont, room );
	}
	// 床ネオン
	if( WFLBY_SYSTEM_FLAG_GetEventStart( p_wk->p_system, WFLBY_EVENT_CHG_NEON_FLOOR ) ){
		u32 floor;
		floor = WFLBY_TIMEEVENT_GetNeonFloor( WFLBY_SYSTEM_Event_GetNeonFloor( p_wk->p_system ) );
		WFLBY_LIGHT_SetFloor( p_wk->p_light, floor );
	}
	// モニュメントネオン
	if( WFLBY_SYSTEM_FLAG_GetEventStart( p_wk->p_system, WFLBY_EVENT_CHG_NEON_MONU ) ){
		u32 monu;
		monu = WFLBY_TIMEEVENT_GetNeonMonu( WFLBY_SYSTEM_Event_GetNeonMonu( p_wk->p_system ) );
		WFLBY_LIGHT_SetMonu( p_wk->p_light, monu );
	}

	// ネオンイベントが始まっていて、花火中でないとき
	if( (WFLBY_SYSTEM_Event_GetNeon( p_wk->p_system ) == TRUE) && 
		(WFLBY_SYSTEM_Event_GetHanabi( p_wk->p_system ) == WFLBY_EVENT_HANABI_BEFORE) ){
		// ライトがついていないなら、ライトON
		if( WFLBY_3DMAPOBJCONT_MAP_CheckFloorLightYure( p_wk->p_mapobjcont ) == FALSE ){
			// ライトON
			WFLBY_3DMAPOBJCONT_MAP_SetFloorLight( p_wk->p_mapobjcont, WFLBY_3DMAPOBJ_CONT_LAMP_ON );
		}
	}

	// 花火開始
	if( WFLBY_SYSTEM_FLAG_GetEventStart( p_wk->p_system, WFLBY_EVENT_CHG_FIRE ) ){
		if( WFLBY_SYSTEM_Event_GetHanabi( p_wk->p_system ) == WFLBY_EVENT_HANABI_PLAY ){
			// 花火開始
			// 銅像のライトが同時にOFFになります
			WFLBY_3DMAPOBJCONT_MAP_OffDouzouLight( p_wk->p_mapobjcont );

			// 床ライトOFF
			WFLBY_3DMAPOBJCONT_MAP_SetFloorLight( p_wk->p_mapobjcont, WFLBY_3DMAPOBJ_CONT_LAMP_OFF );

		}else{
			// 花火停止
		}
	}
		

	// パレード開始
	if( WFLBY_SYSTEM_FLAG_GetEventStart( p_wk->p_system, WFLBY_EVENT_CHG_PARADE ) ){
		if( WFLBY_SYSTEM_Event_GetParade( p_wk->p_system ) == TRUE ){

			// 策をアニメ開始
			WFLBY_3DMAPOBJCONT_MAP_OnPoll( p_wk->p_mapobjcont );
		}
	}

	// クローズＣＭ開始
	if( WFLBY_SYSTEM_Event_GetEndCM( p_wk->p_system ) == TRUE ){
		// 枠のアニメ停止
		WFLBY_3DMAPOBJCONT_MAP_OffPoll( p_wk->p_mapobjcont );
		

		// アプリの明かりを消す
		WFLBY_TIMEEVENT_AplLightOff( p_wk );
	}

	
	// ライト管理メイン
	WFLBY_LIGHT_Main( p_wk->p_light );

	// フロート管理メイン
	WFLBY_FLOAT_CONT_Main( p_wk->p_floatcont );

	// 花火管理メイン
	WFLBY_FIRECONT_Main( p_wk->p_firecont );
}

//----------------------------------------------------------------------------
/**
 *	@brief	フロートがアニメ中かチェック
 *
 *	@param	cp_wk		システム
 *	@param	floatidx	フロートインデックス
 *	@param	anmidx		アニメインデックス		WFLBY_FLOAT_ANM_NUM
 *
 *	@retval	TRUE	アニメ中
 *	@retval	FALSE	アニメしてない
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_TIMEEVENT_FLOAT_CheckAnm( const WFLBY_TIMEEVENT* cp_wk, u32 floatidx, WFLBY_FLOAT_ANM_TYPE anmidx )
{
	return WFLBY_FLOAT_CONT_CheckAnm( cp_wk->p_floatcont, floatidx, anmidx );
}

//----------------------------------------------------------------------------
/**
 *	@brief	花火SEが流れてたら消す
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_TIMEEVENT_FIRE_StopSe( WFLBY_TIMEEVENT* p_wk )
{
	WFLBY_FIRECONT_StopFireSe( p_wk->p_firecont );
}





//-----------------------------------------------------------------------------
/**
 *			プライベート
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	部屋ネオンナンバーを取得する
 *
 *	@param	event	イベント
 */
//-----------------------------------------------------------------------------
static u32 WFLBY_TIMEEVENT_GetNeonRoom( PPW_LOBBY_TIME_EVENT event )
{
	switch( event ){
    case PPW_LOBBY_TIME_EVENT_NEON_A0:           ///< 室内ネオン諧調0。
    case PPW_LOBBY_TIME_EVENT_NEON_A1:           ///< 室内ネオン諧調1。
    case PPW_LOBBY_TIME_EVENT_NEON_A2:           ///< 室内ネオン諧調2。
    case PPW_LOBBY_TIME_EVENT_NEON_A3:           ///< 室内ネオン諧調3。
    case PPW_LOBBY_TIME_EVENT_NEON_A4:           ///< 室内ネオン諧調4。
    case PPW_LOBBY_TIME_EVENT_NEON_A5:           ///< 室内ネオン諧調5。
		
		return event - PPW_LOBBY_TIME_EVENT_NEON_A0;

	default:
		//  イベントナンバーえらー
		GF_ASSERT( 0 );
		break;
	}
	return 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief	床ネオンナンバーを取得する
 *
 *	@param	event	イベント
 */
//-----------------------------------------------------------------------------
static u32 WFLBY_TIMEEVENT_GetNeonFloor( PPW_LOBBY_TIME_EVENT event )
{
	switch( event ){
    case PPW_LOBBY_TIME_EVENT_NEON_B0:           ///< 床ネオン諧調0。
    case PPW_LOBBY_TIME_EVENT_NEON_B1:           ///< 床ネオン諧調1。
    case PPW_LOBBY_TIME_EVENT_NEON_B2:           ///< 床ネオン諧調2。
    case PPW_LOBBY_TIME_EVENT_NEON_B3:           ///< 床ネオン諧調3。
		
		return event - PPW_LOBBY_TIME_EVENT_NEON_B0;

	default:
		//  イベントナンバーえらー
		GF_ASSERT( 0 );
		break;
	}
	return 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief	モニュメントネオンナンバーを取得する
 *
 *	@param	event	イベント
 */
//-----------------------------------------------------------------------------
static u32 WFLBY_TIMEEVENT_GetNeonMonu( PPW_LOBBY_TIME_EVENT event )
{
	switch( event ){
    case PPW_LOBBY_TIME_EVENT_NEON_C0:           ///< モニュメント諧調0。
    case PPW_LOBBY_TIME_EVENT_NEON_C1:           ///< モニュメント諧調1。
    case PPW_LOBBY_TIME_EVENT_NEON_C2:           ///< モニュメント諧調2。
    case PPW_LOBBY_TIME_EVENT_NEON_C3:           ///< モニュメント諧調3。
		
		return event - PPW_LOBBY_TIME_EVENT_NEON_C0;

	default:
		//  イベントナンバーえらー
		GF_ASSERT( 0 );
		break;
	}
	return 0;
}





//----------------------------------------------------------------------------
/**
 *	@brief	アプリケーションライトをOFFする
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_TIMEEVENT_AplLightOff( WFLBY_TIMEEVENT* p_wk )
{
	// ライト２をOFFにする
	NNS_G3dGlbLightColor( 2, GX_RGB( 0,0,0 ) );
}
