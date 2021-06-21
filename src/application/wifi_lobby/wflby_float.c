//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wflby_float.c
 *	@brief		フロート管理システム	(SYSTEMのフロートデータを描画に反映させるシステム)
 *	@author		tomoya takahashi
 *	@data		2008.01.29
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "common.h"

#include  "poketool/monsno.h"

#include "wflby_snd.h"

#include "wflby_float.h"
#include "wflby_3dmatrix.h"

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
//#define WFLBY_DEBUG_FLOAT_ANM_SELECT	// Lボタンでアニメを選択する
#endif

// Lボタンでアニメを選択する
#ifdef WFLBY_DEBUG_FLOAT_ANM_SELECT
static u32 s_WFLBY_DEBUG_FLOAT_ANM_SELECT_COUNT = WFLBY_FLOAT_ANM_SOUND; 
#endif



//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	マップの移動距離
//=====================================
#define WFLBY_FLOAT_CONT_START_X_OFS	( 6 )	// マップの大きさ＋この位置からフロートが始まる
#define WFLBY_FLOAT_CONT_END_X			( -8 )	// 終了する座標
#define WFLBY_FLOAT_CONT_LOCK_X_OFS		( WF2DMAP_GRID_SIZ )	// 予約ロックをかける駅位置からのオフセット位置



//-------------------------------------
///	フロートゆれアニメ種類
//=====================================
enum {
	WFLBY_FLOAT_SHAKE_ANM_NORMAL,
	WFLBY_FLOAT_SHAKE_ANM_NORMAL_BIG,
	WFLBY_FLOAT_SHAKE_ANM_NUM,
} ;

//-------------------------------------
///	WFLBY_FLOAT_SHAKE_ANM_NORMAL定数
//=====================================
#define WFLBY_FLOAT_SHAKE_NRM_COUNT_MAX		( 8 )
#define WFLBY_FLOAT_SHAKE_NRM_COUNT_MOVEY	( 2 )
#define WFLBY_FLOAT_SHAKE_NRM_COUNT_ROT		( RotKeyR( 5 ) )
#define WFLBY_FLOAT_SHAKE_NRM_COUNT_ROOP	( 4 )

//-------------------------------------
///	WFLBY_FLOAT_SHAKE_ANM_NORMALBIG定数
//=====================================
#define WFLBY_FLOAT_SHAKE_NRMBIG_COUNT_MAX		( 16 )
#define WFLBY_FLOAT_SHAKE_NRMBIG_COUNT_MOVEY	( 4 )
#define WFLBY_FLOAT_SHAKE_NRMBIG_COUNT_ROT		( RotKeyR( 10 ) )
#define WFLBY_FLOAT_SHAKE_NRMBIG_COUNT_ROOP		( 2 )



//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	ゆれアニメデータ
//=====================================
typedef struct {
	u16 anm;		// BOOL型
	u16	anm_type;	// WFLBY_FLOAT_SHAKE_ANM～
	union{
		// X軸ゆれ
		struct{
			s16 count;
			s16 count_max;
			u16 rot_num;
			s16 move_y;
			s16 roop;
		} normal;
	} data;
} WFLBY_FLOAT_SHAKE;


//-------------------------------------
///	フロート管理ワーク
//=====================================
typedef struct _WFLBY_FLOAT_CONT{
	WFLBY_ROOMWK* p_rmwk;
	WFLBY_SYSTEM* p_system;
	WFLBY_3DMAPOBJ_CONT* p_mapobjcont;
	WFLBY_MAPCONT* p_mapcont;

	// アニメ再生情報
	u8	anm_play[WFLBY_FLOAT_MAX][WFLBY_FLOAT_ANM_NUM];		// 3*9	27
	u8	pad;												// 28

	// フロート揺れアニメ
	WFLBY_FLOAT_SHAKE	shake[ WFLBY_FLOAT_MAX ];

	// 駅情報
	WF2DMAP_POS		station_pos[ WFLBY_FLOAT_STATION_NUM ];

	// 自分の再生したアニメデータ
	u16	float_idx;
	u16 anm_no;

	// フロート動作距離
	fx32	float_move_start;
	fx32	float_move_dis;
} WFLBY_FLOAT_CONT;


//-------------------------------------
///	フロートdata
//	へや数分のフロートデータ
//=====================================
typedef struct {
	u16	monsno_sml;
	u16	monsno_big;
	u16	monsno_sml_snd_pv;
	u16	monsno_sml_snd_ex;
	u16	monsno_big_snd_pv;
	u16	monsno_big_snd_ex;
} WFLBY_FLOAT_DATA;



//-----------------------------------------------------------------------------
/**
 *		フロートデータ
 */
//-----------------------------------------------------------------------------
static const WFLBY_FLOAT_DATA sc_WFLBY_FLOAT_DATA[WFLBY_ROOM_NUM] = {
	// 火
	{ 
		MONSNO_ATYAMO, MONSNO_RIZAADON, 
		WFLBY_SND_RIDE_BUTTON_ATYAMO_PV, 
		WFLBY_SND_RIDE_BUTTON_ATYAMO_EX,
		WFLBY_SND_RIDE_BUTTON_RIZAADON_PV, 
		WFLBY_SND_RIDE_BUTTON_RIZAADON_EX,
	},

	// 水
	{ 
		MONSNO_POTTYAMA, MONSNO_GYARADOSU,
		WFLBY_SND_RIDE_BUTTON_POTTYAMA_PV,
		WFLBY_SND_RIDE_BUTTON_POTTYAMA_EX,
		WFLBY_SND_RIDE_BUTTON_GYARADOSU_PV,
		WFLBY_SND_RIDE_BUTTON_GYARADOSU_EX,
	},

	// 電気
	{ 
		MONSNO_PIKATYUU, MONSNO_DEKAKOIRU,
		WFLBY_SND_RIDE_BUTTON_PIKATYUU_PV,
		WFLBY_SND_RIDE_BUTTON_PIKATYUU_EX,
		WFLBY_SND_RIDE_BUTTON_DEKAKOIRU_PV,
		WFLBY_SND_RIDE_BUTTON_DEKAKOIRU_EX,
	},

	// 草
	{ 
		MONSNO_HUSIGIDANE, MONSNO_HAEZIGOKU,
		WFLBY_SND_RIDE_BUTTON_HUSIGIDANE_PV,
		WFLBY_SND_RIDE_BUTTON_HUSIGIDANE_EX,
		WFLBY_SND_RIDE_BUTTON_HAEZIGOKU_PV,
		WFLBY_SND_RIDE_BUTTON_HAEZIGOKU_EX,
	},

	// 特殊
	{ 
		MONSNO_MYUU, MONSNO_MYUU,
		WFLBY_SND_RIDE_BUTTON_MYUU_PV,
		WFLBY_SND_RIDE_BUTTON_MYUU_EX,
		WFLBY_SND_RIDE_BUTTON_MYUU_PV_2,
		WFLBY_SND_RIDE_BUTTON_MYUU_EX,
	},
};

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
static void WFLBY_FLOAT_CONT_SetMatrix( WFLBY_FLOAT_CONT* p_sys, u32 idx );
static void WFLBY_FLOAT_CONT_SetReserveLock( WFLBY_FLOAT_CONT* p_sys, u32 idx );
static void WFLBY_FLOAT_CONT_SetAnm( WFLBY_FLOAT_CONT* p_sys, u32 idx, BOOL my_float_on, BOOL my_float, u32 my_float_offs );

static void WFLBY_FLOAT_CONT_PlaySe( BOOL my_float_on, BOOL my_float, BOOL my_offs, u32 se_idx );
static void WFLBY_FLOAT_CONT_PlayMonsVoice( BOOL my_float_on, BOOL my_float, BOOL my_offs, u32 monsno );

static BOOL WFLBY_FLOAT_CONT_InitShake( WFLBY_FLOAT_SHAKE* p_wk, u32 anm_type, WFLBY_3DMAPOBJ_CONT* p_mapobjcont, u32 idx );
static BOOL WFLBY_FLOAT_CONT_MainShake( WFLBY_FLOAT_SHAKE* p_wk, WFLBY_3DMAPOBJ_CONT* p_mapobjcont, u32 idx );
static void WFLBY_FLOAT_CONT_InitShakeNormal( WFLBY_FLOAT_SHAKE* p_wk, WFLBY_3DMAPOBJ_CONT* p_mapobjcont, u32 idx );
static void WFLBY_FLOAT_CONT_InitShakeNormalBig( WFLBY_FLOAT_SHAKE* p_wk, WFLBY_3DMAPOBJ_CONT* p_mapobjcont, u32 idx );
static BOOL WFLBY_FLOAT_CONT_MainShakeNormal( WFLBY_FLOAT_SHAKE* p_wk, WFLBY_3DMAPOBJ_CONT* p_mapobjcont, u32 idx );

//----------------------------------------------------------------------------
/**
 *	@brief	フロート管理システム	初期化
 *
 *	@param	p_rmwk		部屋ワーク
 *	@param	heapID		ヒープＩＤ
 *
 *	@return	ワーク
 */
//-----------------------------------------------------------------------------
WFLBY_FLOAT_CONT* WFLBY_FLOAT_CONT_Init( WFLBY_ROOMWK* p_rmwk, u32 heapID )
{
	WFLBY_FLOAT_CONT* p_sys;
	int i;
	u16 gridx, gridy;
	static const u8 sc_STATION_OBJID[ WFLBY_FLOAT_STATION_NUM ] = {
		WFLBY_MAPOBJID_FLOAT02_IN,
		WFLBY_MAPOBJID_FLOAT01_IN,
		WFLBY_MAPOBJID_FLOAT00_IN,
	};

	p_sys = sys_AllocMemory( heapID, sizeof(WFLBY_FLOAT_CONT) );
	memset( p_sys, 0, sizeof(WFLBY_FLOAT_CONT) );

	p_sys->p_rmwk		= p_rmwk;
	p_sys->p_system		= WFLBY_ROOM_GetSystemData( p_rmwk );
	p_sys->p_mapobjcont	= WFLBY_ROOM_GetMapObjCont( p_rmwk );
	p_sys->p_mapcont	= WFLBY_ROOM_GetMapCont( p_rmwk );

	// フロート動作情報を取得
	p_sys->float_move_start	= WFLBY_MAPCONT_GridXGet( p_sys->p_mapcont ) + WFLBY_FLOAT_CONT_START_X_OFS;
	p_sys->float_move_dis	= WFLBY_FLOAT_CONT_END_X - p_sys->float_move_start;
	// グリッド座標なので、３Ｄ座標にする
	p_sys->float_move_start = WF2DMAP_GRID2POS( p_sys->float_move_start ) * WFLBY_3DMATRIX_GRID_SIZ;
	p_sys->float_move_dis	= WF2DMAP_GRID2POS( p_sys->float_move_dis ) * WFLBY_3DMATRIX_GRID_SIZ;


	// 駅の位置を取得する
	for( i=0; i<WFLBY_FLOAT_STATION_NUM; i++ ){
		WFLBY_MAPCONT_SarchObjID( p_sys->p_mapcont, sc_STATION_OBJID[i], 
				&gridx, &gridy, 0 );
		p_sys->station_pos[i].x	= WF2DMAP_GRID2POS(gridx);
		p_sys->station_pos[i].y	= WF2DMAP_GRID2POS(gridy);
	}

	// フロートを端に移動させる
	for( i=0; i<WFLBY_FLOAT_MAX; i++ ){
		WFLBY_3DMAPOBJCONT_FLOAT_SetPos( p_sys->p_mapobjcont, i, p_sys->float_move_start );

		// 現在座標をフロートに設定し、予約ロック状況を設定する
		if( WFLBY_SYSTEM_GetFloatMove( p_sys->p_system, i ) == WFLBY_FLOAT_STATE_MOVE ){
			// 座標
			WFLBY_FLOAT_CONT_SetMatrix( p_sys, i );

			// 予約ロック設定
			WFLBY_FLOAT_CONT_SetReserveLock( p_sys, i );
		}
	}
	

	return p_sys;
}

//----------------------------------------------------------------------------
/**
 *	@brief	フロート管理システム	破棄
 *
 *	@param	p_sys	ワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_FLOAT_CONT_Exit( WFLBY_FLOAT_CONT* p_sys )
{
	sys_FreeMemoryEz( p_sys );
}

//----------------------------------------------------------------------------
/**
 *	@brief	フロート管理システム	メイン
 *
 *	@param	p_sys	ワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_FLOAT_CONT_Main( WFLBY_FLOAT_CONT* p_sys )
{
	int i;
	BOOL result;
	u32 my_float_idx;
	u32 my_float_offs;
	u32 myidx;
	BOOL my_float_on;
	BOOL my_float;

#ifdef WFLBY_DEBUG_FLOAT_ANM_SELECT
	if( sys.trg & PAD_BUTTON_L ){
		s_WFLBY_DEBUG_FLOAT_ANM_SELECT_COUNT = (s_WFLBY_DEBUG_FLOAT_ANM_SELECT_COUNT+1) % WFLBY_FLOAT_ANM_NUM;
		OS_TPrintf( "FLOAT_ANM change %d\n", s_WFLBY_DEBUG_FLOAT_ANM_SELECT_COUNT );
	}
#endif
	
	// 自分の座席情報取得
	myidx			= WFLBY_SYSTEM_GetMyIdx( p_sys->p_system );
	my_float_idx	= WFLBY_SYSTEM_GetFloatPlIdxReserve( p_sys->p_system, myidx );

	if( my_float_idx == DWC_LOBBY_USERIDTBL_IDX_NONE ){
		my_float_on = FALSE;		// フロートには乗ってない
	}else{
		// 自分が乗っているフロートかチェック
		WFLBY_SYSTEM_GetFloatIdxOfs( my_float_idx, &my_float_idx, &my_float_offs );
		my_float_on = TRUE;		// 自分が乗っているフロートがある
	}

	
	for( i=0; i<WFLBY_FLOAT_MAX; i++ ){
		if( WFLBY_SYSTEM_GetFloatMove( p_sys->p_system, i ) == WFLBY_FLOAT_STATE_MOVE ){

			// 表示のＯＮ　ＯＦＦ
			WFLBY_3DMAPOBJCONT_FLOAT_SetDraw( p_sys->p_mapobjcont, i, TRUE );

			// 座標
			WFLBY_FLOAT_CONT_SetMatrix( p_sys, i );

			// 予約ロック設定
			WFLBY_FLOAT_CONT_SetReserveLock( p_sys, i );

			// 自分が乗っているフロートか？
			if( my_float_on ){
				if( my_float_idx == i ){
					my_float = TRUE;
				}else{
					my_float = FALSE;
				}
			}
			
			// アニメ
			WFLBY_FLOAT_CONT_SetAnm( p_sys, i, my_float_on, my_float, my_float_offs );
		}else{

			// 表示のＯＮ　ＯＦＦ
			WFLBY_3DMAPOBJCONT_FLOAT_SetDraw( p_sys->p_mapobjcont, i, FALSE );
		}
	}

	// アニメ制御
	for( i=0; i<WFLBY_FLOAT_MAX; i++ ){


		// 頭
		result = WFLBY_3DMAPOBJCONT_FLOAT_CheckAnmSound( p_sys->p_mapobjcont, i );
		p_sys->anm_play[i][WFLBY_FLOAT_ANM_SOUND] = result;

		// 胴体揺らし
		result = WFLBY_FLOAT_CONT_MainShake( &p_sys->shake[i], p_sys->p_mapobjcont, i );
		p_sys->anm_play[i][WFLBY_FLOAT_ANM_SHAKE] = result;

		// 胴体
		result = WFLBY_3DMAPOBJCONT_FLOAT_CheckAnmBody( p_sys->p_mapobjcont, i );
		p_sys->anm_play[i][WFLBY_FLOAT_ANM_BODY] = result;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	アニメ中かチェックする
 *
 *	@param	cp_sys		システム
 *	@param	floatidx	フロートインデックス
 *	@param	anmidx		アニメインデックス
 *
 *	@retval	TRUE		アニメ中	
 *	@retval	FALSE		アニメ停止
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_FLOAT_CONT_CheckAnm( const WFLBY_FLOAT_CONT* cp_sys, u32 floatidx, WFLBY_FLOAT_ANM_TYPE anmidx )
{
	GF_ASSERT( floatidx < WFLBY_FLOAT_MAX );
	GF_ASSERT( anmidx < WFLBY_FLOAT_ANM_NUM );
	return cp_sys->anm_play[floatidx][anmidx];
}





//-----------------------------------------------------------------------------
/**
 *				プライベート関数
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	座標の設定
 *
 *	@param	p_sys		システム
 *	@param	idx			インデックス
 */
//-----------------------------------------------------------------------------
static void WFLBY_FLOAT_CONT_SetMatrix( WFLBY_FLOAT_CONT* p_sys, u32 idx )
{
	s32 count;
	s32 count_max;
	fx32 x;
	fx32 dis;

	// カウント値と最大数から今の表示位置を取得
	count		= WFLBY_SYSTEM_GetFloatCount( p_sys->p_system, idx );
	count_max	= WFLBY_SYSTEM_GetFloatCountMax( p_sys->p_system );

	// 計算するには値が大きすぎるので、移動距離を調整する
	dis = FX_Div( p_sys->float_move_dis, FX32_CONST(2) );
	x	= FX_Div( FX_Mul(FX32_CONST( count ), dis), FX32_CONST(count_max) );
	x	= FX_Mul( x, FX32_CONST(2) );
	x	+= p_sys->float_move_start;

//	OS_TPrintf( "idx=%d  x=%d\n", idx, x >> FX32_SHIFT );

	// 座標を設定
	WFLBY_3DMAPOBJCONT_FLOAT_SetPos( p_sys->p_mapobjcont, idx, x );
}


//----------------------------------------------------------------------------
/**
 *	@brief	フロートのアニメを設定する
 *
 *	@param	p_sys		システム
 *	@param	idx			インデックス
 *	@param	my_float_on		自分はフトートに乗っているか？
 *	@param	my_float		自分が乗っているフロートなのか
 *	@param	my_float_offs	自分が乗っている座席の位置
 */
//-----------------------------------------------------------------------------
static void WFLBY_FLOAT_CONT_SetAnm( WFLBY_FLOAT_CONT* p_sys, u32 idx, BOOL my_float_on, BOOL my_float, u32 my_float_offs )
{
	int i;
	BOOL anm_on;
	BOOL anm_result = FALSE;
	u32 season;
	u32 room;
	BOOL my_float_offs_chk;

	room	= WFLBY_SYSTEM_GetRoomType( p_sys->p_system );
	season	= WFLBY_SYSTEM_GetSeason( p_sys->p_system );


	
	for( i=0; i<WFLBY_FLOAT_ON_NUM; i++ ){
		// アニメの再生チェック
		anm_on = WFLBY_SYSTEM_GetFloatAnm( p_sys->p_system, (idx*WFLBY_FLOAT_ON_NUM)+i );

		// 自分のフロートのときは自分の座席かチェックする
		my_float_offs_chk = FALSE;
		if( my_float == TRUE ){
			if( i == my_float_offs ){
				my_float_offs_chk = TRUE;
			}
		}
		
		if( anm_on == TRUE ){
#ifdef WFLBY_DEBUG_FLOAT_ANM_SELECT
			switch( s_WFLBY_DEBUG_FLOAT_ANM_SELECT_COUNT ){
#else
			switch( i ){
#endif
			// 泣き声
			case WFLBY_FLOAT_ANM_SOUND:
				// アニメ設定
				anm_result = WFLBY_3DMAPOBJCONT_FLOAT_SetAnmSound( p_sys->p_mapobjcont, idx );
				// おと出す
				if( anm_result == TRUE ){
					// 泣き声とSEを鳴らす
					if( (idx % WFLBY_FLOAT_ON_NUM) == 2 ){
						// でかいフロート
						WFLBY_FLOAT_CONT_PlayMonsVoice( my_float_on, my_float, my_float_offs_chk, sc_WFLBY_FLOAT_DATA[ room ].monsno_big );

						if( sc_WFLBY_FLOAT_DATA[ room ].monsno_big_snd_pv != WFLBY_SND_RIDE_INVALIDITY ){
							WFLBY_FLOAT_CONT_PlaySe( my_float_on, my_float, my_float_offs_chk, sc_WFLBY_FLOAT_DATA[ room ].monsno_big_snd_pv );
						}
					}else{
						// ちっこいフロート
						WFLBY_FLOAT_CONT_PlayMonsVoice( my_float_on, my_float, my_float_offs_chk, sc_WFLBY_FLOAT_DATA[ room ].monsno_sml );

						if( sc_WFLBY_FLOAT_DATA[ room ].monsno_sml_snd_pv != WFLBY_SND_RIDE_INVALIDITY ){
							WFLBY_FLOAT_CONT_PlaySe( my_float_on, my_float, my_float_offs_chk, sc_WFLBY_FLOAT_DATA[ room ].monsno_sml_snd_pv );
						}
					}
					
					WFLBY_SYSTEM_CleanFloatAnm( p_sys->p_system, (idx*WFLBY_FLOAT_ON_NUM)+i );	// アニメ再生したのでフラグを落とす
				}
				break;
			// フロート揺らしアニメ
			case WFLBY_FLOAT_ANM_SHAKE:
				{
					u32 anm;

					if( (idx % WFLBY_FLOAT_ON_NUM) == 2 ){
						anm = WFLBY_FLOAT_SHAKE_ANM_NORMAL_BIG;
					}else{
						anm = WFLBY_FLOAT_SHAKE_ANM_NORMAL;
					}
					anm_result = WFLBY_FLOAT_CONT_InitShake( &p_sys->shake[ idx ], anm, p_sys->p_mapobjcont, idx );
					if( anm_result == TRUE ){
						WFLBY_SYSTEM_CleanFloatAnm( p_sys->p_system, (idx*WFLBY_FLOAT_ON_NUM)+i );	// アニメ再生したのでフラグを落とす

						WFLBY_FLOAT_CONT_PlaySe( my_float_on, my_float, my_float_offs_chk, WFLBY_SND_RIDE_BUTTON01 );
						
					}
				}
				break;

			// ボディアニメ
			case WFLBY_FLOAT_ANM_BODY:
				anm_result = WFLBY_3DMAPOBJCONT_FLOAT_SetAnmBody( p_sys->p_mapobjcont, idx );
				if( anm_result == TRUE ){
					WFLBY_SYSTEM_CleanFloatAnm( p_sys->p_system, (idx*WFLBY_FLOAT_ON_NUM)+i );	// アニメ再生したのでフラグを落とす
					
					// 泣き声とSEを鳴らす
					if( (idx % WFLBY_FLOAT_ON_NUM) == 2 ){
						// でかいフロート
						WFLBY_FLOAT_CONT_PlaySe( my_float_on, my_float, my_float_offs_chk, sc_WFLBY_FLOAT_DATA[ room ].monsno_big_snd_ex );
					}else{
						// ちっこいフロート
						WFLBY_FLOAT_CONT_PlaySe( my_float_on, my_float, my_float_offs_chk, sc_WFLBY_FLOAT_DATA[ room ].monsno_sml_snd_ex );
					}
				}
				break;
			}
		}
	}
}


//----------------------------------------------------------------------------
/**
 *	@brief	ＳＥの再生
 *
 *	@param	my_float_on		自分はフロートに乗ってるか？
 *	@param	my_float		自分が乗っているフロートか？
 *	@param	my_offs			自分が乗っている座席か？
 *	@param	se_idx			ＳＥＮｏ
 */
//-----------------------------------------------------------------------------
static void WFLBY_FLOAT_CONT_PlaySe( BOOL my_float_on, BOOL my_float, BOOL my_offs, u32 se_idx )
{
	if( my_float_on ){
		// 乗っているときは、自分が乗っているフローとだけＳＥを鳴らす
		if( my_float ){

			// さらに自分の座席のＳＥなら自分専用プレイヤーで鳴らす
			if( my_offs ){
				Snd_SePlayEx( se_idx, SND_PLAYER_NO_WIFI_HIROBA );
			}else{
				Snd_SePlay( se_idx );
			}
		}
	}else{

		// 乗っていないときは全部Snd_SePlayで上書き
		Snd_SePlay( se_idx );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	ポケモンボイス用
 */
//-----------------------------------------------------------------------------
static void WFLBY_FLOAT_CONT_PlayMonsVoice( BOOL my_float_on, BOOL my_float, BOOL my_offs, u32 monsno )
{
	if( my_float_on ){
		// 乗っているときは、自分が乗っているフローとだけＳＥを鳴らす
		if( my_float ){

			// さらに自分の座席のＳＥなら自分専用プレイヤーで鳴らす
			Snd_PMVoicePlay( monsno, 0 );
		}
	}else{

		// 乗っていないときは全部上書き
		Snd_PMVoicePlay( monsno, 0 );
	}
}




//----------------------------------------------------------------------------
/**
 *	@brief	ゆれアニメ	初期化
 *
 *	@param	p_wk			ワーク
 *	@param	anm_type		アニメタイプ
 *	@param	p_mapobjcont	マップオブジェ管理システム
 *	@param	idx				インデックス
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_FLOAT_CONT_InitShake( WFLBY_FLOAT_SHAKE* p_wk, u32 anm_type, WFLBY_3DMAPOBJ_CONT* p_mapobjcont, u32 idx )
{
	static void (* const pFunc[])( WFLBY_FLOAT_SHAKE* p_wk, WFLBY_3DMAPOBJ_CONT* p_mapobjcont, u32 idx ) = {
		WFLBY_FLOAT_CONT_InitShakeNormal,
		WFLBY_FLOAT_CONT_InitShakeNormalBig,
	};
	GF_ASSERT( anm_type < WFLBY_FLOAT_SHAKE_ANM_NUM );

	// 動作中は動かさない
	if( p_wk->anm == TRUE ){
		return FALSE;
	}

	p_wk->anm_type	= anm_type;
	p_wk->anm		= TRUE;
	pFunc[ anm_type ]( p_wk, p_mapobjcont, idx );

	return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	胴体揺らしアニメメイン
 *
 *	@param	p_wk			ワーク
 *	@param	p_mapobjcont	マップオブジェ管理システム
 *	@param	idx				インデックス
 *
 *	@retval	TRUE	揺らし中
 *	@retval	FALSE	揺らし終了
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_FLOAT_CONT_MainShake( WFLBY_FLOAT_SHAKE* p_wk, WFLBY_3DMAPOBJ_CONT* p_mapobjcont, u32 idx )
{
	BOOL result;
	static BOOL (* const pFunc[])( WFLBY_FLOAT_SHAKE* p_wk, WFLBY_3DMAPOBJ_CONT* p_mapobjcont, u32 idx ) = {
		WFLBY_FLOAT_CONT_MainShakeNormal,
		WFLBY_FLOAT_CONT_MainShakeNormal,
	};

	if( p_wk->anm ){
		result = pFunc[ p_wk->anm_type ]( p_wk, p_mapobjcont, idx );
		if( result == TRUE ){
			// 終了
			memset( p_wk, 0, sizeof(WFLBY_FLOAT_SHAKE) );
		}
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	通常ゆれアニメ	開始
 *
 *	@param	p_wk			ワーク
 *	@param	p_mapobjcont	マップオブジェ管理システム
 *	@param	idx				インデックス
 */
//-----------------------------------------------------------------------------
static void WFLBY_FLOAT_CONT_InitShakeNormal( WFLBY_FLOAT_SHAKE* p_wk, WFLBY_3DMAPOBJ_CONT* p_mapobjcont, u32 idx )
{
	p_wk->data.normal.count			= 0;
	p_wk->data.normal.count_max		= WFLBY_FLOAT_SHAKE_NRM_COUNT_MAX;
	p_wk->data.normal.rot_num		= WFLBY_FLOAT_SHAKE_NRM_COUNT_ROT;
	p_wk->data.normal.move_y		= WFLBY_FLOAT_SHAKE_NRM_COUNT_MOVEY;
	p_wk->data.normal.roop			= WFLBY_FLOAT_SHAKE_NRM_COUNT_ROOP;
}

//----------------------------------------------------------------------------
/**
 *	@brief	通常ゆれ大きいアニメ	開始
 *
 *	@param	p_wk			ワーク
 *	@param	p_mapobjcont	マップオブジェ管理システム
 *	@param	idx				インデックス
 */
//-----------------------------------------------------------------------------
static void WFLBY_FLOAT_CONT_InitShakeNormalBig( WFLBY_FLOAT_SHAKE* p_wk, WFLBY_3DMAPOBJ_CONT* p_mapobjcont, u32 idx )
{
	p_wk->data.normal.count			= 0;
	p_wk->data.normal.count_max		= WFLBY_FLOAT_SHAKE_NRMBIG_COUNT_MAX;
	p_wk->data.normal.rot_num		= WFLBY_FLOAT_SHAKE_NRMBIG_COUNT_ROT;
	p_wk->data.normal.move_y		= WFLBY_FLOAT_SHAKE_NRMBIG_COUNT_MOVEY;
	p_wk->data.normal.roop			= WFLBY_FLOAT_SHAKE_NRMBIG_COUNT_ROOP;
}

//----------------------------------------------------------------------------
/**
 *	@brief	通常ゆれアニメ
 *
 *	@param	p_wk			ワーク
 *	@param	p_mapobjcont	マップオブジェ管理システム
 *	@param	idx				インデックス
 *
 *	@retva	TRUE	アニメ完了
 *	@retva	FALSE	アニメ中
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_FLOAT_CONT_MainShakeNormal( WFLBY_FLOAT_SHAKE* p_wk, WFLBY_3DMAPOBJ_CONT* p_mapobjcont, u32 idx )
{
	VecFx32 ofs_pos = {0};	// 座標
	u16	rotx;
	u16 eff_rot;
	BOOL ret = FALSE;

	// カウント処理
	if( (p_wk->data.normal.count+1) < p_wk->data.normal.count_max ){
		p_wk->data.normal.count ++;
	}else{
		p_wk->data.normal.count = 0;
		if( (p_wk->data.normal.roop - 1) > 0 ){
			p_wk->data.normal.roop --;
		}else{
			ret = TRUE;		// 終了
		}
	}

	// カウント値から回転度数を取得する
	eff_rot = (p_wk->data.normal.count * 0xffff) / p_wk->data.normal.count_max;

	// その度数からｘ軸回転角度を計算
	rotx = FX_Mul( FX_SinIdx( eff_rot ), FX32_CONST( p_wk->data.normal.rot_num ) ) >> FX32_SHIFT;
	
	// Ｙ軸移動位置計算
	ofs_pos.y =  FX_Mul( FX_SinIdx( eff_rot ), FX32_CONST( p_wk->data.normal.move_y ) );

	// そのデータを設定
	WFLBY_3DMAPOBJCONT_FLOAT_SetRot( p_mapobjcont, idx, rotx );
	WFLBY_3DMAPOBJCONT_FLOAT_SetOfsPos( p_mapobjcont, idx, &ofs_pos );


	return ret;		// 途中
}


//----------------------------------------------------------------------------
/**
 *	@brief	予約ロック設定
 *
 *	@param	p_sys		システム
 *	@param	idx			インデックス
 */
//-----------------------------------------------------------------------------
static void WFLBY_FLOAT_CONT_SetReserveLock( WFLBY_FLOAT_CONT* p_sys, u32 idx )
{
	int i, j;
	u32 posidx;
	u32 now_station;
	VecFx32 matrix;
	fx32 lock_x;

	//　座席の座標が駅のX座標がより小さくなったら予約ロックをかける
	for( i=0; i<WFLBY_FLOAT_ON_NUM; i++ ){
		posidx = (idx * WFLBY_FLOAT_ON_NUM) + i;
		now_station = WFLBY_SYSTEM_GetFloatReserveLock( p_sys->p_system, posidx );

		// 座標を取得
		WFLBY_3DMAPOBJCONT_FLOAT_GetSheetPos( p_sys->p_mapobjcont, idx, i, &matrix );

		// ロックする駅を設定
		for( j=now_station; j<WFLBY_FLOAT_STATION_NUM; j++ ){
		
			// 駅ロックチェック
			lock_x = ((p_sys->station_pos[j].x+WFLBY_FLOAT_CONT_LOCK_X_OFS)*WFLBY_3DMATRIX_GRID_SIZ);
			if( lock_x > matrix.x ){
				// ロックかける
				WFLBY_SYSTEM_SetFloatReserveLock( p_sys->p_system, posidx, j );
			}
		}
	}
}
