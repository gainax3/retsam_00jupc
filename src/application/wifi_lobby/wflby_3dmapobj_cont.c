//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wflby_3dmapobj_cont.c
 *	@brief		マップオブジェ配置管理
 *	@author		tomoya takahashi
 *	@data		2007.11.14
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "common.h"
#include "wflby_3dmapobj_cont.h"
#include "wflby_3dmatrix.h"

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
//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	配置オブジェ登録最大数
//=====================================
#define WFLBY_3DMAPOBJ_CONT_MAPOBJWK_MAX		( 128 )


//-------------------------------------
///	配置オブジェ	アニメ
//=====================================
#define WFLBY_3DMAPOBJ_CONT_ANM_MG_NUM	(3)		// ミニゲームの数
// 看板
enum{
	WFLBY_3DMAPOBJ_CONT_KANBAN_ANM_NONE,
	WFLBY_3DMAPOBJ_CONT_KANBAN_ANM_REC_0,
	WFLBY_3DMAPOBJ_CONT_KANBAN_ANM_REC_1,
	WFLBY_3DMAPOBJ_CONT_KANBAN_ANM_REC_2,
	WFLBY_3DMAPOBJ_CONT_KANBAN_ANM_REC_3,
	WFLBY_3DMAPOBJ_CONT_KANBAN_ANM_PLAY_0,
	WFLBY_3DMAPOBJ_CONT_KANBAN_ANM_PLAY_1,
	WFLBY_3DMAPOBJ_CONT_KANBAN_ANM_PLAY_2,
	WFLBY_3DMAPOBJ_CONT_KANBAN_ANM_PLAY_3,
	WFLBY_3DMAPOBJ_CONT_KANBAN_ANM_NUM,
};
#define WFLBY_3DMAPOBJ_CONT_KANBAN_ANM_ONEFRAME	( 4 )

// 銅像
#define WFLBY_3DMAPOBJ_CONT_DOUZOU_ANM_RESTART_FRAME	( 0x2000 )
//static int WFLBY_3DMAPOBJ_CONT_DOUZOU_ANM_RESTART_FRAME	= ( 0x2000 );
#define WFLBY_3DMAPOBJ_CONT_DOUZOU_BIGFIRE_POS_OFS		( 8 )
#define WFLBY_3DMAPOBJ_CONT_DOUZOU_BIGFIRE_POS_OFS_Z	( 20 )

// ランプ花火
#define WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_NUM		( 24 )	// 花火オブジェの最大数
enum{
	WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_ST_ONWAIT,
	WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_ST_ONSTART,
	WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_ST_ON,
	WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_ST_OFFSTART,
	WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_ST_OFF,
};
#define WFLBY_3DMAPOBJ_CONT_HANABI_SPEED_END	( 0 )	// 最終アニメスピード
#define WFLBY_3DMAPOBJ_CONT_HANABI_ALPHA_DEF	( 31 )	// 基本花火アルファ値
#define WFLBY_3DMAPOBJ_CONT_HANABI_COUNT_MAX	( 10 )	// 花火が消えているカウント値
#define WFLBY_3DMAPOBJ_CONT_HANABI_SCALE_DIF	( FX32_CONST( 0.180f ) )// 大きさを小さくする割合

//  看板
#define WFLBY_3DMAPOBJ_CONT_KANBAN_ANM_MAX		( 28 )
#define WFLBY_3DMAPOBJ_CONT_KANBAN_ANM_HALF		( WFLBY_3DMAPOBJ_CONT_KANBAN_ANM_MAX/2 )


//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	床花火	ワーク
//=====================================
typedef struct {
	WFLBY_3DMAPOBJ_WK* p_hanabi;
	u16	status;
	s16	count;
	fx32 start_speed;	// 開始アニメスピード
} WFLBY_3DMAPOBJ_HANABIWK;


//-------------------------------------
///	アニメの変更などで操作するもの
//=====================================
typedef struct {
	WFLBY_3DMAPOBJ_WK* p_mgmachine[WFLBY_3DMAPOBJ_CONT_ANM_MG_NUM];
	WFLBY_3DMAPOBJ_WK* p_mgkanban[WFLBY_3DMAPOBJ_CONT_ANM_MG_NUM];
	BOOL ramp_cont_yure;

	WFLBY_3DMAPOBJ_HANABIWK		hanabi[WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_NUM];
	WFLBY_3DMAPOBJ_HANABIWK		hanabibig[WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_NUM];
	WFLBY_3DMAPOBJ_WK*			p_brhanabi[WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_NUM];
	WFLBY_3DMAPOBJ_HANABIWK		brhanabibig[WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_NUM];
	u8	hanabi_count;
	u8	hanabibig_count;
	u8	brhanabi_count;
	u8	brhanabibig_count;

	u32 kanban_count;
} WFLBY_3DMAPOBJ_ANMCONT;



//-------------------------------------
///	システム構造体
//=====================================
typedef struct _WFLBY_3DMAPOBJ_CONT{
	const WFLBY_MAPCONT*	cp_mapdata;		// マップデータ
	WFLBY_3DMAPOBJ*			p_mapobjsys;	// 配置オブジェシステム
	WFLBY_3DMAPOBJ_WK*		p_mapwkobj[ WFLBY_3DMAPOBJ_CONT_MAPOBJWK_MAX ];	// 配置オブジェ配列
	WFLBY_3DMAPOBJ_FLOAT*	p_mapfloat[ WFLBY_3DMAPOBJ_CONT_MAPOBJFLOAT_MAX ];	// フロートオブジェ
	WFLBY_3DMAPOBJ_ANMCONT	anmcont;
	u8						season;			// シーズン
	u8						room;			// 部屋
	u8						objaddnum;		// 配置オブジェ登録数
	u8						floataddnum;	// フロート登録数

}WFLBY_3DMAPOBJ_CONT;


//-------------------------------------
///	配置モデル　マップIDとモデルIDの対応表
//=====================================
typedef struct {
	u16 mapid;
	u16 mdlid;
} WFLBY_3DMAPOBJ_MAPDATA;


//-------------------------------------
///	フロート構成データ
//=====================================
typedef struct {
	u16 floatnum;
	u16 col;
} WFLBY_3DMAPOBJ_FLOAT_DATA;



//-----------------------------------------------------------------------------
/**
 *			データ関連
 */
//-----------------------------------------------------------------------------
//-------------------------------------
///	配置オブジェデータ
//=====================================
static const WFLBY_3DMAPOBJ_MAPDATA sc_WFLBY_3DMAPOBJ_CONT_WKDATA[] = {
	{ WFLBY_MAPOBJID_DOUZOU00, WFLBY_3DMAPOBJ_BIGBRONZE },
	{ WFLBY_MAPOBJID_DOUZOU0100, WFLBY_3DMAPOBJ_SMALLBRONZE00 },
	{ WFLBY_MAPOBJID_DOUZOU0101, WFLBY_3DMAPOBJ_SMALLBRONZE01 },
	{ WFLBY_MAPOBJID_DOUZOU0102, WFLBY_3DMAPOBJ_SMALLBRONZE02 },
	{ WFLBY_MAPOBJID_DOUZOU0102, WFLBY_3DMAPOBJ_SMALLBRONZE02 },
	{ WFLBY_MAPOBJID_DOUZOU0103, WFLBY_3DMAPOBJ_SMALLBRONZE03 },
	{ WFLBY_MAPOBJID_TREE00, WFLBY_3DMAPOBJ_TREE },
	{ WFLBY_MAPOBJID_MG00, WFLBY_3DMAPOBJ_MG_00 },
	{ WFLBY_MAPOBJID_MG01, WFLBY_3DMAPOBJ_MG_01 },
	{ WFLBY_MAPOBJID_MG02, WFLBY_3DMAPOBJ_MG_02 },
	{ WFLBY_MAPOBJID_TABLE, WFLBY_3DMAPOBJ_TABLE },
	{ WFLBY_MAPOBJID_KANBAN00, WFLBY_3DMAPOBJ_KANBAN },
	{ WFLBY_MAPOBJID_KANBAN01, WFLBY_3DMAPOBJ_KANBAN },
	{ WFLBY_MAPOBJID_KANBAN02, WFLBY_3DMAPOBJ_KANBAN },
	{ WFLBY_MAPOBJID_LAMP00, WFLBY_3DMAPOBJ_LAMP00 },
	{ WFLBY_MAPOBJID_LAMP01, WFLBY_3DMAPOBJ_LAMP01 },
	{ WFLBY_MAPOBJID_LAMP00, WFLBY_3DMAPOBJ_HANABI },
	{ WFLBY_MAPOBJID_LAMP01, WFLBY_3DMAPOBJ_HANABI },
	{ WFLBY_MAPOBJID_LAMP00, WFLBY_3DMAPOBJ_HANABIBIG },
	{ WFLBY_MAPOBJID_LAMP01, WFLBY_3DMAPOBJ_HANABIBIG },
	{ WFLBY_MAPOBJID_DOUZOU0100, WFLBY_3DMAPOBJ_BRHANABI },
	{ WFLBY_MAPOBJID_DOUZOU0101, WFLBY_3DMAPOBJ_BRHANABI },
	{ WFLBY_MAPOBJID_DOUZOU0102, WFLBY_3DMAPOBJ_BRHANABI },
	{ WFLBY_MAPOBJID_DOUZOU0102, WFLBY_3DMAPOBJ_BRHANABI },
	{ WFLBY_MAPOBJID_DOUZOU0103, WFLBY_3DMAPOBJ_BRHANABI },
	{ WFLBY_MAPOBJID_DOUZOU0100, WFLBY_3DMAPOBJ_BRHANABIBIG },
	{ WFLBY_MAPOBJID_DOUZOU0101, WFLBY_3DMAPOBJ_BRHANABIBIG },
	{ WFLBY_MAPOBJID_DOUZOU0102, WFLBY_3DMAPOBJ_BRHANABIBIG },
	{ WFLBY_MAPOBJID_DOUZOU0102, WFLBY_3DMAPOBJ_BRHANABIBIG },
	{ WFLBY_MAPOBJID_DOUZOU0103, WFLBY_3DMAPOBJ_BRHANABIBIG },
	{ WFLBY_MAPOBJID_DOUZOU00FIRE00, WFLBY_3DMAPOBJ_HANABI },
	{ WFLBY_MAPOBJID_DOUZOU00FIRE01, WFLBY_3DMAPOBJ_HANABI },
	{ WFLBY_MAPOBJID_DOUZOU00FIRE00, WFLBY_3DMAPOBJ_HANABIBIG },
	{ WFLBY_MAPOBJID_DOUZOU00FIRE01, WFLBY_3DMAPOBJ_HANABIBIG },
	{ WFLBY_MAPOBJID_ANKETO_BOARD,	WFLBY_3DMAPOBJ_ANKETO },
};

// 大きい銅像用花火のオフセット設定
#define WFLBY_3DMAPOBJ_CONT_DOUZOU00_FIRE_NUM	(2)
static const s32 sc_WFLBY_3DMAPOBJ_CONT_DOUZOU00_FIRE_OFFS_X[ WFLBY_3DMAPOBJ_CONT_DOUZOU00_FIRE_NUM ] = {
	// 左側
	-11,

	// 右側
	12,
};
#define WFLBY_3DMAPOBJ_CONT_DOUZOU00_FIRE_OFFS_Y	(-10)


//-------------------------------------
///	フロート座席位置データ
//=====================================
static const WF2DMAP_POS sc_WFLBY_3DMAPOBJ_CONT_FLOATSHEET_POS[ WFLBY_3DMAPOBJ_CONT_MAPOBJFLOAT_SHEET_MAX ] = {
	{ -2,	10 },
	{ 14,	10 },
	{ 30,	10 },
};
#define WFLBY_3DMAPOBJ_CONT_FLOAT_Y	( FX32_CONST( 40 ) )	// フロート座席Yざひょう
#define WFLBY_3DMAPOBJ_CONT_FLOAT_X		( 0 )	// Ｚ座標
#define WFLBY_3DMAPOBJ_CONT_FLOAT_Z		( FX32_CONST( 40 ) )	// Ｚ座標

//-------------------------------------
///	フロート構成データ
//=====================================
static const WFLBY_3DMAPOBJ_FLOAT_DATA sc_WFLBY_3DMAPOBJJ_CONT_FLOAT_DATA[ WFLBY_FLOAT_MAX ] = {
	{ WFLBY_3DMAPOBJ_FLOAT_00, WFLBY_3DMAPOBJ_FLOAT_COL_00 },
	{ WFLBY_3DMAPOBJ_FLOAT_00, WFLBY_3DMAPOBJ_FLOAT_COL_00 },
	{ WFLBY_3DMAPOBJ_FLOAT_01, WFLBY_3DMAPOBJ_FLOAT_COL_00 },

	{ WFLBY_3DMAPOBJ_FLOAT_00, WFLBY_3DMAPOBJ_FLOAT_COL_01 },
	{ WFLBY_3DMAPOBJ_FLOAT_00, WFLBY_3DMAPOBJ_FLOAT_COL_01 },
	{ WFLBY_3DMAPOBJ_FLOAT_01, WFLBY_3DMAPOBJ_FLOAT_COL_01 },

	{ WFLBY_3DMAPOBJ_FLOAT_00, WFLBY_3DMAPOBJ_FLOAT_COL_02 },
	{ WFLBY_3DMAPOBJ_FLOAT_00, WFLBY_3DMAPOBJ_FLOAT_COL_02 },
	{ WFLBY_3DMAPOBJ_FLOAT_01, WFLBY_3DMAPOBJ_FLOAT_COL_02 },
};


//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
static BOOL WFLBY_3DOBJCONT_MAPOBJWK_CheckPosGrid( const WFLBY_3DMAPOBJ_WK* cp_wk, u8 gridx, u8 gridy );
static WFLBY_3DMAPOBJ_WK* WFLBY_3DOBJCONT_MAPOBJWK_GetGridWk( WFLBY_3DMAPOBJ_CONT* p_sys, u8 gridx, u8 gridy );

static void WFLBY_3DMAPOBJ_LAMP_YURE_SetCallBack( WFLBY_3DMAPOBJ* p_sys, WFLBY_3DMAPOBJ_WK* p_wk );
static void WFLBY_3DMAPOBJ_LAMP_NONE_SetCallBack( WFLBY_3DMAPOBJ* p_sys, WFLBY_3DMAPOBJ_WK* p_wk );

static void WFLBY_3DMAPOBJ_LAMP_DrawOff_SetCallBack( WFLBY_3DMAPOBJ* p_sys, WFLBY_3DMAPOBJ_WK* p_wk );


static void WFLBY_3DMAPOBJ_DouzouAnm_PlayNormal( WFLBY_3DMAPOBJ_CONT* p_sys, WFLBY_3DMAPOBJ_WK* p_wk );
static void WFLBY_3DMAPOBJ_DouzouAnm_PlaySpecial( WFLBY_3DMAPOBJ_CONT* p_sys, WFLBY_3DMAPOBJ_WK* p_wk );
static void WFLBY_3DMAPOBJ_DouzouAnm_PlayAnm( WFLBY_3DMAPOBJ_CONT* p_sys, WFLBY_3DMAPOBJ_WK* p_wk );


// 床花火ワーク
static void WFLBY_3DMAPOBJ_HANABIWK_Init( WFLBY_3DMAPOBJ_HANABIWK* p_wk, WFLBY_3DMAPOBJ_WK* p_hanabi );
static void WFLBY_3DMAPOBJ_HANABIWK_Main( WFLBY_3DMAPOBJ_HANABIWK* p_wk, WFLBY_3DMAPOBJ* p_mapobjsys );
static void WFLBY_3DMAPOBJ_HANABIWK_On( WFLBY_3DMAPOBJ_HANABIWK* p_wk, WFLBY_3DMAPOBJ* p_mapobjsys, fx32 speed );
static void WFLBY_3DMAPOBJ_HANABIWK_Off( WFLBY_3DMAPOBJ_HANABIWK* p_wk );
static void WFLBY_3DMAPOBJ_HANABIWK_OnDraw( WFLBY_3DMAPOBJ_HANABIWK* p_wk, WFLBY_3DMAPOBJ* p_mapobjsys );
static void WFLBY_3DMAPOBJ_HANABIWK_OffDraw( WFLBY_3DMAPOBJ_HANABIWK* p_wk, WFLBY_3DMAPOBJ* p_mapobjsys );
static void WFLBY_3DMAPOBJ_HANABIWK_OffMoveStart( WFLBY_3DMAPOBJ_HANABIWK* p_wk );
static BOOL WFLBY_3DMAPOBJ_HANABIWK_OffMove( WFLBY_3DMAPOBJ_HANABIWK* p_wk, WFLBY_3DMAPOBJ* p_mapobjsys );

// 大きい花火用
static void WFLBY_3DMAPOBJ_HANABIWK_OnBig( WFLBY_3DMAPOBJ_HANABIWK* p_wk, fx32 speed );
static void WFLBY_3DMAPOBJ_HANABIWK_MainBig( WFLBY_3DMAPOBJ_HANABIWK* p_wk, WFLBY_3DMAPOBJ* p_mapobjsys );
static void WFLBY_3DMAPOBJ_HANABIWK_OnBigDraw( WFLBY_3DMAPOBJ_HANABIWK* p_wk, WFLBY_3DMAPOBJ* p_mapobjsys );

// 銅像大きい花火用（開始はOnBigを使用）
static void WFLBY_3DMAPOBJ_HANABIWK_MainBrBig( WFLBY_3DMAPOBJ_HANABIWK* p_wk, WFLBY_3DMAPOBJ* p_mapobjsys );
static void WFLBY_3DMAPOBJ_HANABIWK_OnBrBigDraw( WFLBY_3DMAPOBJ_HANABIWK* p_wk, WFLBY_3DMAPOBJ* p_mapobjsys );

// 大きい銅像用花火座標設定関数
static void WFLBY_3DMAPOBJ_DOUZOU00_HANABI_SetPos( WFLBY_3DMAPOBJ_WK* p_hanabi, u32 mapid );

//----------------------------------------------------------------------------
/**
 *	@brief	マップ配置オブジェクト管理システム	初期化
 *
 *	@param	season	シーズン	
 *	@param	room	部屋タイプ
 *	@param	cp_map	マップデータ
 *	@param	heapID	ヒープID
 *	@param	gheapID	グラフィックヒープID
 *
 *	@return	システムワーク
 */
//-----------------------------------------------------------------------------
WFLBY_3DMAPOBJ_CONT* WFLBY_3DMAPOBJCONT_Init( WFLBY_SEASON_TYPE season, WFLBY_ROOM_TYPE room, const WFLBY_MAPCONT* cp_map, u32 heapID, u32 gheapID )
{
	WFLBY_3DMAPOBJ_CONT* p_sys;
	int i;

	p_sys = sys_AllocMemory( heapID, sizeof(WFLBY_3DMAPOBJ_CONT) );
	memset( p_sys, 0, sizeof(WFLBY_3DMAPOBJ_CONT) );

	// マップデータ保存
	p_sys->cp_mapdata = cp_map;

	// シーズン部屋データ保存
	p_sys->season	= season;
	p_sys->room		= room;

	// 配置オブジェシステム作成
	p_sys->p_mapobjsys = WFLBY_3DMAPOBJ_Init( WFLBY_FLOAT_MAX, WFLBY_3DMAPOBJ_CONT_MAPOBJWK_MAX, heapID, gheapID );

	// 配置オブジェリソース読み込み
	WFLBY_3DMAPOBJ_ResLoad( p_sys->p_mapobjsys, room, season, heapID, gheapID );

	// 配置オブジェクトを登録
	{
		int i;
		u32 count;
		u16 gridx, gridy;

		for( i=0; i<NELEMS(sc_WFLBY_3DMAPOBJ_CONT_WKDATA); i++ ){
			count = 0;
			while( WFLBY_MAPCONT_SarchObjID( p_sys->cp_mapdata, sc_WFLBY_3DMAPOBJ_CONT_WKDATA[i].mapid, &gridx, &gridy, count ) == TRUE ){


				// グリッド位置に登録
				p_sys->p_mapwkobj[ p_sys->objaddnum ] = WFLBY_3DMAPOBJ_WK_Add( 
						p_sys->p_mapobjsys, sc_WFLBY_3DMAPOBJ_CONT_WKDATA[i].mdlid, gridx, gridy );

				// 各モデルの初期化処理
				switch( sc_WFLBY_3DMAPOBJ_CONT_WKDATA[i].mdlid ){
				// ミニゲーム
				case WFLBY_3DMAPOBJ_MG_00:
				case WFLBY_3DMAPOBJ_MG_01:
				case WFLBY_3DMAPOBJ_MG_02:
					WFLBY_3DMAPOBJ_WK_AddAnm( p_sys->p_mapobjsys, 
							p_sys->p_mapwkobj[ p_sys->objaddnum ], 
							WFLBY_3DMAPOBJ_MG_ANM_ALL, WFLBY_3DMAPOBJ_WK_ANM_LOOP );	

					// ミニゲームタイプのごとに振り分ける
					p_sys->anmcont.p_mgmachine[ sc_WFLBY_3DMAPOBJ_CONT_WKDATA[i].mdlid-WFLBY_3DMAPOBJ_MG_00 ] = p_sys->p_mapwkobj[ p_sys->objaddnum ];
					break;

				case WFLBY_3DMAPOBJ_BIGBRONZE:
					WFLBY_3DMAPOBJ_WK_AddAnm( p_sys->p_mapobjsys, 
							p_sys->p_mapwkobj[ p_sys->objaddnum ], 
							WFLBY_3DMAPOBJ_BIGBRONZEZ_ANM_ALL, WFLBY_3DMAPOBJ_WK_ANM_LOOP );	
					WFLBY_3DMAPOBJ_WK_AddAnm( p_sys->p_mapobjsys, 
							p_sys->p_mapwkobj[ p_sys->objaddnum ], 
							WFLBY_3DMAPOBJ_BIGBRONZEZ_ANM_ALL01, WFLBY_3DMAPOBJ_WK_ANM_LOOP );	
					WFLBY_3DMAPOBJ_WK_AddAnm( p_sys->p_mapobjsys, 
							p_sys->p_mapwkobj[ p_sys->objaddnum ], 
							WFLBY_3DMAPOBJ_BIGBRONZEZ_ANM_ALL02, WFLBY_3DMAPOBJ_WK_ANM_LOOP );	
					break;
				
				// 看板
				case WFLBY_3DMAPOBJ_KANBAN:
					// とりあえず動作中アニメも
					WFLBY_3DMAPOBJ_WK_AddAnm( p_sys->p_mapobjsys, 
							p_sys->p_mapwkobj[ p_sys->objaddnum ], 
							WFLBY_3DMAPOBJ_KANBAN_ANM_LAMP, WFLBY_3DMAPOBJ_WK_ANM_NOAUTO );	
					// ミニゲームタイプのごとに振り分ける
					p_sys->anmcont.p_mgkanban[ sc_WFLBY_3DMAPOBJ_CONT_WKDATA[i].mapid-WFLBY_MAPOBJID_KANBAN00 ] = p_sys->p_mapwkobj[ p_sys->objaddnum ];
					break;

				case WFLBY_3DMAPOBJ_LAMP00:
				case WFLBY_3DMAPOBJ_LAMP01:
					WFLBY_3DMAPOBJ_WK_AddAnm( p_sys->p_mapobjsys, 
							p_sys->p_mapwkobj[ p_sys->objaddnum ], 
							WFLBY_3DMAPOBJ_LAMP_ANM_ON, WFLBY_3DMAPOBJ_WK_ANM_NOAUTO );	
					break;

	
				// 花火類
				case WFLBY_3DMAPOBJ_HANABI:
					WFLBY_3DMAPOBJ_HANABIWK_Init( 
							&p_sys->anmcont.hanabi[ p_sys->anmcont.hanabi_count ],
							p_sys->p_mapwkobj[ p_sys->objaddnum ]	);
					p_sys->anmcont.hanabi_count ++;
					GF_ASSERT( p_sys->anmcont.hanabi_count <= WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_NUM );
					WFLBY_3DMAPOBJ_WK_SetDraw( p_sys->p_mapwkobj[ p_sys->objaddnum ], FALSE );

					// 大きい銅像用の花火は位置を調整する
					if( (sc_WFLBY_3DMAPOBJ_CONT_WKDATA[i].mapid == WFLBY_MAPOBJID_DOUZOU00FIRE00) ||
						(sc_WFLBY_3DMAPOBJ_CONT_WKDATA[i].mapid == WFLBY_MAPOBJID_DOUZOU00FIRE01) ){

						WFLBY_3DMAPOBJ_DOUZOU00_HANABI_SetPos( p_sys->p_mapwkobj[ p_sys->objaddnum ], sc_WFLBY_3DMAPOBJ_CONT_WKDATA[i].mapid );
							
					}
					
					break;

				case WFLBY_3DMAPOBJ_HANABIBIG:
					WFLBY_3DMAPOBJ_HANABIWK_Init( 
							&p_sys->anmcont.hanabibig[ p_sys->anmcont.hanabibig_count ],
							p_sys->p_mapwkobj[ p_sys->objaddnum ]	);
					p_sys->anmcont.hanabibig_count ++;
					GF_ASSERT( p_sys->anmcont.hanabi_count <= WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_NUM );
					WFLBY_3DMAPOBJ_WK_SetDraw( p_sys->p_mapwkobj[ p_sys->objaddnum ], FALSE );

					// 大きい銅像用の花火は位置を調整する
					if( (sc_WFLBY_3DMAPOBJ_CONT_WKDATA[i].mapid == WFLBY_MAPOBJID_DOUZOU00FIRE00) ||
						(sc_WFLBY_3DMAPOBJ_CONT_WKDATA[i].mapid == WFLBY_MAPOBJID_DOUZOU00FIRE01) ){

						WFLBY_3DMAPOBJ_DOUZOU00_HANABI_SetPos( p_sys->p_mapwkobj[ p_sys->objaddnum ], sc_WFLBY_3DMAPOBJ_CONT_WKDATA[i].mapid );
							
					}
					break;
					
				case WFLBY_3DMAPOBJ_BRHANABI:
					p_sys->anmcont.p_brhanabi[ p_sys->anmcont.brhanabi_count ] = p_sys->p_mapwkobj[ p_sys->objaddnum ];
					p_sys->anmcont.brhanabi_count ++;
					GF_ASSERT( p_sys->anmcont.brhanabi_count <= WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_NUM );
					WFLBY_3DMAPOBJ_WK_SetDraw( p_sys->p_mapwkobj[ p_sys->objaddnum ], FALSE );
					break;
					
				case WFLBY_3DMAPOBJ_BRHANABIBIG:
					WFLBY_3DMAPOBJ_HANABIWK_Init( 
							&p_sys->anmcont.brhanabibig[ p_sys->anmcont.brhanabibig_count ],
							p_sys->p_mapwkobj[ p_sys->objaddnum ]	);
					p_sys->anmcont.brhanabibig_count ++;
					GF_ASSERT( p_sys->anmcont.brhanabibig_count <= WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_NUM );
					WFLBY_3DMAPOBJ_WK_SetDraw( p_sys->p_mapwkobj[ p_sys->objaddnum ], FALSE );

					// 位置を調整
					{
						WF2DMAP_POS pos;
						pos = WFLBY_3DMAPOBJ_WK_GetPos( p_sys->p_mapwkobj[ p_sys->objaddnum ] );
						pos.x += WFLBY_3DMAPOBJ_CONT_DOUZOU_BIGFIRE_POS_OFS;
						pos.y += WFLBY_3DMAPOBJ_CONT_DOUZOU_BIGFIRE_POS_OFS_Z;
						WFLBY_3DMAPOBJ_WK_SetPos( p_sys->p_mapwkobj[ p_sys->objaddnum ], pos );
					}
					break;

				}

				p_sys->objaddnum ++;

				// 何番目のOBJIDを探すか
				count ++;

			}
		}
	}


	//  フロートの作成
	{	
		VecFx32 matrix;
		matrix.x	= WFLBY_3DMAPOBJ_CONT_FLOAT_X;
		matrix.z	= WFLBY_3DMAPOBJ_CONT_FLOAT_Z;
		matrix.y	= WFLBY_3DMATRIX_FLOOR_MAT;
		for( i=0; i<WFLBY_FLOAT_MAX; i++ ){
			p_sys->p_mapfloat[ i ] = WFLBY_3DMAPOBJ_FLOAT_Add( 
					p_sys->p_mapobjsys, 
					sc_WFLBY_3DMAPOBJJ_CONT_FLOAT_DATA[i].floatnum, sc_WFLBY_3DMAPOBJJ_CONT_FLOAT_DATA[i].col, &matrix );

			// 非表示
			WFLBY_3DMAPOBJ_FLOAT_SetDraw( p_sys->p_mapfloat[ i ], FALSE );
		}
		p_sys->floataddnum = WFLBY_FLOAT_MAX;
	}

	return p_sys;
}

//----------------------------------------------------------------------------
/**
 *	@brief	マップオブジェ管理システム破棄
 *
 *	@param	p_sys	ワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJCONT_Exit( WFLBY_3DMAPOBJ_CONT* p_sys )
{
	// 全フロート破棄
	{
		int i;

		for( i=0; i<p_sys->floataddnum; i++ ){
			WFLBY_3DMAPOBJ_FLOAT_Del( p_sys->p_mapfloat[ i ] );
		}
	}
	
	// 全オブジェを破棄
	{
		int i;
		for( i=0; i<p_sys->objaddnum; i++ ){
			WFLBY_3DMAPOBJ_WK_Del( p_sys->p_mapobjsys, p_sys->p_mapwkobj[i] );
		}
	}

	// 配置オブジェリソース破棄
	WFLBY_3DMAPOBJ_ResRelease( p_sys->p_mapobjsys );	

	// 配置オブジェシステム破棄
	WFLBY_3DMAPOBJ_Exit( p_sys->p_mapobjsys );

	// 自分自身を破棄
	sys_FreeMemoryEz( p_sys );
}

//----------------------------------------------------------------------------
/**
 *	@brief	メイン処理
 *
 *	@param	p_sys		ワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJCONT_Main( WFLBY_3DMAPOBJ_CONT* p_sys )
{
	int i;

#ifndef WFLBY_3DMAPOBJ_CONT_DOUZOU_ANM_RESTART_FRAME
	if( sys.trg & PAD_BUTTON_L ){
		WFLBY_3DMAPOBJ_CONT_DOUZOU_ANM_RESTART_FRAME += FX32_HALF;
		OS_TPrintf( "frame 0x%x\n", WFLBY_3DMAPOBJ_CONT_DOUZOU_ANM_RESTART_FRAME );
	}
	if( sys.trg & PAD_BUTTON_R ){
		WFLBY_3DMAPOBJ_CONT_DOUZOU_ANM_RESTART_FRAME -= FX32_HALF;
		OS_TPrintf( "frame 0x%x\n", WFLBY_3DMAPOBJ_CONT_DOUZOU_ANM_RESTART_FRAME );
	}
#endif

	// 床花火ワークメイン
	for( i=0; i<p_sys->anmcont.hanabi_count; i++ ){
		WFLBY_3DMAPOBJ_HANABIWK_Main( &p_sys->anmcont.hanabi[i], p_sys->p_mapobjsys );
	}

	// 床でか花火ワークメイン
	for( i=0; i<p_sys->anmcont.hanabibig_count; i++ ){
		WFLBY_3DMAPOBJ_HANABIWK_MainBig( &p_sys->anmcont.hanabibig[i], p_sys->p_mapobjsys );
	}

	// 銅像でか花火ワークメイン
	for( i=0; i<p_sys->anmcont.brhanabibig_count; i++ ){
		WFLBY_3DMAPOBJ_HANABIWK_MainBrBig( &p_sys->anmcont.brhanabibig[i], p_sys->p_mapobjsys );
	}
	
	// 配置オブジェシステムメイン
	WFLBY_3DMAPOBJ_Main( p_sys->p_mapobjsys );
}

//----------------------------------------------------------------------------
/**
 *	@brief	描画処理
 *
 *	@param	p_sys	ワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJCONT_Draw( WFLBY_3DMAPOBJ_CONT* p_sys )
{
	// 配置オブジェシステム描画
	WFLBY_3DMAPOBJ_Draw( p_sys->p_mapobjsys );
}

//----------------------------------------------------------------------------
/**
 *	@brief	VBlank処理
 *
 *	@param	p_sys	ワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJCONT_VBlank( WFLBY_3DMAPOBJ_CONT* p_sys )
{
	// 配置オブジェシステムVBlank
	WFLBY_3DMAPOBJ_VBlank( p_sys->p_mapobjsys );
}




//----------------------------------------------------------------------------
/**
 *	@brief	フロートの位置を取得
 *
 *	@param	cp_sys		システム
 *	@param	idx			フロートナンバー
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJCONT_FLOAT_GetPos( const WFLBY_3DMAPOBJ_CONT* cp_sys, u32 idx, VecFx32* p_vec )
{
	GF_ASSERT( cp_sys->floataddnum > idx );
	WFLBY_3DMAPOBJ_FLOAT_GetPos( cp_sys->p_mapfloat[ idx ],  p_vec );
}

//----------------------------------------------------------------------------
/**
 *	@brief	フロートの位置を設定
 *
 *	@param	p_sys		システム
 *	@param	idx			フロートナンバー
 *	@param	x			x位置
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJCONT_FLOAT_SetPos( WFLBY_3DMAPOBJ_CONT* p_sys, u32 idx, fx32 x )
{
	VecFx32 matrix;
	
	GF_ASSERT( p_sys->floataddnum > idx );

	matrix.x = x;
	matrix.z = WFLBY_3DMAPOBJ_CONT_FLOAT_Z;
	matrix.y = WFLBY_3DMATRIX_FLOOR_MAT;
	WFLBY_3DMAPOBJ_FLOAT_SetPos( p_sys->p_mapfloat[ idx ], &matrix );
}

//----------------------------------------------------------------------------
/**
 *	@brief	フロートオフセット座標を設定します。	エフェクトなどに使用してください
 *
 *	@param	p_sys		システムワーク
 *	@param	idx			フロートインデックス
 *	@param	cp_vec		座標
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJCONT_FLOAT_SetOfsPos( WFLBY_3DMAPOBJ_CONT* p_sys, u32 idx, const VecFx32* cp_vec )
{
	GF_ASSERT( p_sys->floataddnum > idx );
	WFLBY_3DMAPOBJ_FLOAT_SetOfsPos( p_sys->p_mapfloat[ idx ], cp_vec );
}

//----------------------------------------------------------------------------
/**
 *	@brief	フロートの座席の位置を取得
 *
 *	@param	cp_sys		システム
 *	@param	idx			フロートナンバー
 *	@param	no			座席ナンバー
 *	@param	p_vec		位置格納先
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJCONT_FLOAT_GetSheetPos( const WFLBY_3DMAPOBJ_CONT* cp_sys, u32 idx, u32 no, VecFx32* p_vec )
{
	GF_ASSERT( cp_sys->floataddnum > idx );
	GF_ASSERT( no < WFLBY_3DMAPOBJ_CONT_MAPOBJFLOAT_SHEET_MAX );
	
	WFLBY_3DMAPOBJ_FLOAT_GetPos( cp_sys->p_mapfloat[ idx ], p_vec );
	p_vec->x += (sc_WFLBY_3DMAPOBJ_CONT_FLOATSHEET_POS[ no ].x * WFLBY_3DMATRIX_GRID_SIZ);
	p_vec->z += (sc_WFLBY_3DMAPOBJ_CONT_FLOATSHEET_POS[ no ].y * WFLBY_3DMATRIX_GRID_SIZ);
	p_vec->y = WFLBY_3DMAPOBJ_CONT_FLOAT_Y;
}

//----------------------------------------------------------------------------
/**
 *	@brief	フロート	Sound時アニメ	開始
 *
 *	@param	p_sys		システム
 *	@param	idx			インデックス
 *
 *	@retval	TRUE		アニメ設定できた
 *	@retval	FALSE		アニメ設定できなかった
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_3DMAPOBJCONT_FLOAT_SetAnmSound( WFLBY_3DMAPOBJ_CONT* p_sys, u32 idx )
{
	GF_ASSERT( p_sys->floataddnum > idx );
	return WFLBY_3DMAPOBJ_FLOAT_SetAnmSound( p_sys->p_mapfloat[ idx ] );
}

//----------------------------------------------------------------------------
/**
 *	@brief	フロート	Bodyアニメ	開始
 *
 *	@param	p_sys		システム
 *	@param	idx			インデックス
 *
 *	@retval	TRUE		アニメ設定できた
 *	@retval	FALSE		アニメ設定できなかった
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_3DMAPOBJCONT_FLOAT_SetAnmBody( WFLBY_3DMAPOBJ_CONT* p_sys, u32 idx )
{
	GF_ASSERT( p_sys->floataddnum > idx );
	return WFLBY_3DMAPOBJ_FLOAT_SetAnmBody( p_sys->p_mapfloat[ idx ] );
}

//----------------------------------------------------------------------------
/**
 *	@brief	フロートの描画設定
 *
 *	@param	p_sys		システム
 *	@param	idx			インデックス
 *	@param	flag		フラグ
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJCONT_FLOAT_SetDraw( WFLBY_3DMAPOBJ_CONT* p_sys, u32 idx, BOOL flag )
{
	GF_ASSERT( p_sys->floataddnum > idx );
	WFLBY_3DMAPOBJ_FLOAT_SetDraw( p_sys->p_mapfloat[ idx ], flag );
}

//----------------------------------------------------------------------------
/**
 *	@brief	フロートの描画取得
 *
 *	@param	cp_sys		システム
 *	@param	idx			インデックス
 *
 *	@retval	TRUE	描画中
 *	@retval	FALSE	描画前
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_3DMAPOBJCONT_FLOAT_GetDraw( const WFLBY_3DMAPOBJ_CONT* cp_sys, u32 idx )
{
	GF_ASSERT( cp_sys->floataddnum > idx );
	return WFLBY_3DMAPOBJ_FLOAT_GetDraw( cp_sys->p_mapfloat[ idx ] );
}

//----------------------------------------------------------------------------
/**
 *	@brief	回転角度を設定する
 *
 *	@param	p_sys		システム
 *	@param	idx			インデックス
 *	@param	x			ｘ軸回転角度
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJCONT_FLOAT_SetRot( WFLBY_3DMAPOBJ_CONT* p_sys, u32 idx, u16 x )
{
	GF_ASSERT( p_sys->floataddnum > idx );
	WFLBY_3DMAPOBJ_FLOAT_SetRotaX( p_sys->p_mapfloat[ idx ], x );
}

//----------------------------------------------------------------------------
/**
 *	@brief	フロート音声アニメ中かチェック
 *
 *	@param	cp_sys		システム
 *	@param	idx			インデックス
 *
 *	@retval	TRUE	音声アニメ	再生中
 *	@retval	FALSE	音声アニメ	再生してない
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_3DMAPOBJCONT_FLOAT_CheckAnmSound( const WFLBY_3DMAPOBJ_CONT* cp_sys, u32 idx )
{
	GF_ASSERT( cp_sys->floataddnum > idx );
	return WFLBY_3DMAPOBJ_FLOAT_CheckAnmSound( cp_sys->p_mapfloat[ idx ] );
}

//----------------------------------------------------------------------------
/**
 *	@brief	フロート胴体アニメ	再生中チェック
 *
 *	@param	cp_sys		システム
 *	@param	idx			インデックス
 *
 *	@retval	TRUE	胴体アニメ	再生中
 *	@retval	FALSE	胴体アニメ	停止中
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_3DMAPOBJCONT_FLOAT_CheckAnmBody( const WFLBY_3DMAPOBJ_CONT* cp_sys, u32 idx )
{
	GF_ASSERT( cp_sys->floataddnum > idx );
	return WFLBY_3DMAPOBJ_FLOAT_CheckAnmBody( cp_sys->p_mapfloat[ idx ] );
}


//----------------------------------------------------------------------------
/**
 *	@brief	マップポール表示
 *
 *	@param	p_sys	システムワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJCONT_MAP_OnPoll( WFLBY_3DMAPOBJ_CONT* p_sys )
{
	WFLBY_3DMAPOBJ_MAP_OnPoll( p_sys->p_mapobjsys );
}

//----------------------------------------------------------------------------
/**
 *	@brief	マップポール表示アニメしなくする
 *
 *	@param	p_sys	システムワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJCONT_MAP_OffPoll( WFLBY_3DMAPOBJ_CONT* p_sys )
{
	WFLBY_3DMAPOBJ_MAP_OffPoll( p_sys->p_mapobjsys );
}


//----------------------------------------------------------------------------
/**
 *	@brief	銅像アニメ開始
 *
 *	@param	p_sys	システムワーク
 *	@param	gridx	グリッドｘ
 *	@param	gridy	グリッドｙ
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJCONT_MAP_StartDouzouAnm( WFLBY_3DMAPOBJ_CONT* p_sys, u8 gridx, u8 gridy )
{
	int i;
	BOOL result[2];
	u32  mdltype;

	
	// 銅像のモデルの位置を調べ、位置の一致する銅像をアニメさせる
	for( i=0; i<p_sys->objaddnum; i++ ){
		result[0] = WFLBY_3DOBJCONT_MAPOBJWK_CheckPosGrid( p_sys->p_mapwkobj[i], gridx-1, gridy-1 );
		result[1] = WFLBY_3DOBJCONT_MAPOBJWK_CheckPosGrid( p_sys->p_mapwkobj[i], gridx, gridy-1 );
		if( (result[0] == TRUE) || (result[1] == TRUE) ){
			// そいつが銅像かチェック
			mdltype = WFLBY_3DMAPOBJ_WK_GetType( p_sys->p_mapwkobj[i] );
			switch( mdltype ){
			case WFLBY_3DMAPOBJ_SMALLBRONZE00:	// それぞれ４パターン
			case WFLBY_3DMAPOBJ_SMALLBRONZE01:	// 
			case WFLBY_3DMAPOBJ_SMALLBRONZE02:	// 
			case WFLBY_3DMAPOBJ_SMALLBRONZE03:	// 
				if( p_sys->room == WFLBY_ROOM_SPECIAL ){
					WFLBY_3DMAPOBJ_DouzouAnm_PlaySpecial( p_sys, p_sys->p_mapwkobj[i] );
				}else{
					WFLBY_3DMAPOBJ_DouzouAnm_PlayNormal( p_sys, p_sys->p_mapwkobj[i] );
				}
				break;
				
			default:
				break;
			}
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	銅像花火設定
 *
 *	@param	p_sys		システム	
 *	@param	flag		フラグ
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJCONT_MAP_SetDouzouFire( WFLBY_3DMAPOBJ_CONT* p_sys, BOOL flag )
{
	int i;
	u32  mdltype;

	
	// 銅像のモデルの位置を調べ、位置の一致する銅像をアニメさせる
	for( i=0; i<p_sys->objaddnum; i++ ){
		// そいつが銅像かチェック
		mdltype = WFLBY_3DMAPOBJ_WK_GetType( p_sys->p_mapwkobj[i] );
		switch( mdltype ){
		// 銅像の操作
		case WFLBY_3DMAPOBJ_SMALLBRONZE00:	// それぞれ４パターン
		case WFLBY_3DMAPOBJ_SMALLBRONZE01:	// 
		case WFLBY_3DMAPOBJ_SMALLBRONZE02:	// 
		case WFLBY_3DMAPOBJ_SMALLBRONZE03:	// 
			if( flag==TRUE ){
				WFLBY_3DMAPOBJ_WK_SetLightFlag( p_sys->p_mapobjsys, p_sys->p_mapwkobj[i], 1<<WFLBY_DRAW_LIGHT_ROOM );
			}
			break;


		// 花火の操作
		case WFLBY_3DMAPOBJ_BRHANABI:	
			//  花火再生
			if( flag==TRUE ){

				WFLBY_3DMAPOBJ_WK_SetAnmSpeed( p_sys->p_mapwkobj[i], FX32_HALF );

				WFLBY_3DMAPOBJ_WK_AddAnm( p_sys->p_mapobjsys, 
						p_sys->p_mapwkobj[i], 
						WFLBY_3DMAPOBJ_BRHANABI_ANM_NICA, WFLBY_3DMAPOBJ_WK_ANM_RANDSTART );	

				WFLBY_3DMAPOBJ_WK_AddAnm( p_sys->p_mapobjsys, 
						p_sys->p_mapwkobj[i], 
						WFLBY_3DMAPOBJ_BRHANABI_ANM_NITA, WFLBY_3DMAPOBJ_WK_ANM_RANDSTART );	

				WFLBY_3DMAPOBJ_WK_AddAnm( p_sys->p_mapobjsys, 
						p_sys->p_mapwkobj[i], 
						WFLBY_3DMAPOBJ_BRHANABI_ANM_NITP, WFLBY_3DMAPOBJ_WK_ANM_RANDSTART );	

				WFLBY_3DMAPOBJ_WK_SetDraw( p_sys->p_mapwkobj[i], TRUE );

			}else{
				WFLBY_3DMAPOBJ_WK_DelAnm( p_sys->p_mapobjsys, 
						p_sys->p_mapwkobj[i], 
						WFLBY_3DMAPOBJ_BRHANABI_ANM_NICA );	

				WFLBY_3DMAPOBJ_WK_DelAnm( p_sys->p_mapobjsys, 
						p_sys->p_mapwkobj[i], 
						WFLBY_3DMAPOBJ_BRHANABI_ANM_NITA );	

				WFLBY_3DMAPOBJ_WK_DelAnm( p_sys->p_mapobjsys, 
						p_sys->p_mapwkobj[i], 
						WFLBY_3DMAPOBJ_BRHANABI_ANM_NITP );	

				WFLBY_3DMAPOBJ_WK_SetDraw( p_sys->p_mapwkobj[i], FALSE );
			}
			break;
			
		default:
			break;
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	銅像大きい花火再生
 *
 *	@param	p_sys	ワーク
 *	@param	speed	再生スピード
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJCONT_MAP_SetDouzouBigFire( WFLBY_3DMAPOBJ_CONT* p_sys, fx32 speed )
{
	int i;
	u32  mdltype;

	
	// 銅像のモデルの位置を調べ、位置の一致する銅像をアニメさせる
	for( i=0; i<p_sys->anmcont.brhanabibig_count; i++ ){

		WFLBY_3DMAPOBJ_HANABIWK_OnBig( &p_sys->anmcont.brhanabibig[i], speed );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	銅像のライトをOFFにする
 *
 *	@param	p_sys	ワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJCONT_MAP_OffDouzouLight( WFLBY_3DMAPOBJ_CONT* p_sys )
{
	int i;
	u32  mdltype;

	
	// 銅像のモデルの位置を調べ、位置の一致する銅像をアニメさせる
	for( i=0; i<p_sys->objaddnum; i++ ){
		// そいつが銅像かチェック
		mdltype = WFLBY_3DMAPOBJ_WK_GetType( p_sys->p_mapwkobj[i] );
		switch( mdltype ){
		// 銅像の操作
		case WFLBY_3DMAPOBJ_SMALLBRONZE00:	// それぞれ４パターン
		case WFLBY_3DMAPOBJ_SMALLBRONZE01:	// 
		case WFLBY_3DMAPOBJ_SMALLBRONZE02:	// 
		case WFLBY_3DMAPOBJ_SMALLBRONZE03:	// 
			WFLBY_3DMAPOBJ_WK_SetLightFlag( p_sys->p_mapobjsys, p_sys->p_mapwkobj[i], 1<<WFLBY_DRAW_LIGHT_ROOM );
			break;
			
		default:
			break;
		}
	}
}


// 床ライト
//----------------------------------------------------------------------------
/**
 *	@brief	床ライト設定
 *
 *	@param	p_sys	システムワーク
 *	@param	anm		アニメデータ
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJCONT_MAP_SetFloorLight( WFLBY_3DMAPOBJ_CONT* p_sys, WFLBY_3DMAPOBJ_CONT_LAMP_ANM anm )
{
	int i;
	u32  mdltype;

	
	// ランプのモデルの位置を調べ、位置の一致するランプをアニメさせる
	for( i=0; i<p_sys->objaddnum; i++ ){
		// そいつがランプかチェック
		mdltype = WFLBY_3DMAPOBJ_WK_GetType( p_sys->p_mapwkobj[i] );
		switch( mdltype ){
		case WFLBY_3DMAPOBJ_LAMP00:
		case WFLBY_3DMAPOBJ_LAMP01:

			switch( anm ){
			// 何も出ていない状態
			case WFLBY_3DMAPOBJ_CONT_LAMP_NONE:	
				WFLBY_3DMAPOBJ_WK_AddAnm( p_sys->p_mapobjsys, 
						p_sys->p_mapwkobj[ i ], 
						WFLBY_3DMAPOBJ_LAMP_ANM_ON, WFLBY_3DMAPOBJ_WK_ANM_NOAUTO );	
				p_sys->anmcont.ramp_cont_yure = FALSE;	// ゆれないめしない
				break;
			// NONEから光を出す状態にする		その後ゆれアニメになります
			case WFLBY_3DMAPOBJ_CONT_LAMP_ON:	
				WFLBY_3DMAPOBJ_WK_AddAnmAnmCallBack( p_sys->p_mapobjsys, 
						p_sys->p_mapwkobj[ i ], 
						WFLBY_3DMAPOBJ_LAMP_ANM_ON, WFLBY_3DMAPOBJ_WK_ANM_NOLOOP,
						WFLBY_3DMAPOBJ_LAMP_YURE_SetCallBack );	
				p_sys->anmcont.ramp_cont_yure = TRUE;	// ゆれる
				break;
			// 光が出ている状態から光を消す。	その後NONE状態になります。
			case WFLBY_3DMAPOBJ_CONT_LAMP_OFF:	
				WFLBY_3DMAPOBJ_WK_AddAnmAnmCallBack( p_sys->p_mapobjsys, 
						p_sys->p_mapwkobj[ i ], 
						WFLBY_3DMAPOBJ_LAMP_ANM_ON, WFLBY_3DMAPOBJ_WK_ANM_BACKNOLOOP,
						WFLBY_3DMAPOBJ_LAMP_NONE_SetCallBack );	

				// 他アニメは破棄
				WFLBY_3DMAPOBJ_WK_DelAnm( p_sys->p_mapobjsys, 
						p_sys->p_mapwkobj[ i ], 
						WFLBY_3DMAPOBJ_LAMP_ANM_YURE );	
				p_sys->anmcont.ramp_cont_yure = FALSE;	// ゆれない
				break;
			// ゆれアニメ
			case WFLBY_3DMAPOBJ_CONT_LAMP_YURE:	
				// ゆれアニメ設定
				WFLBY_3DMAPOBJ_WK_AddAnm( p_sys->p_mapobjsys, 
						p_sys->p_mapwkobj[ i ], 
						WFLBY_3DMAPOBJ_LAMP_ANM_YURE, WFLBY_3DMAPOBJ_WK_ANM_LOOP );	

				// 他アニメは破棄
				WFLBY_3DMAPOBJ_WK_DelAnm( p_sys->p_mapobjsys, 
						p_sys->p_mapwkobj[ i ], 
						WFLBY_3DMAPOBJ_LAMP_ANM_ON );	
				p_sys->anmcont.ramp_cont_yure = TRUE;	// ゆれる
				break;
			default:
				break;
			}
			break;
			
		default:
			break;
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	ゆれアニメ中かチェック
 *
 *	@param	cp_sys	管理システム
 *
 *	@retval	TRUE	ゆれアニメ中
 *	@retval	FALSE	ゆれアニメしてない
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_3DMAPOBJCONT_MAP_CheckFloorLightYure( const WFLBY_3DMAPOBJ_CONT* cp_sys )
{
	return cp_sys->anmcont.ramp_cont_yure;	
}

//----------------------------------------------------------------------------
/**
 *	@brief	床ライト花火設定
 *
 *	@param	p_sys	システムワーク
 *	@param	flag	つけるか消すか
 *	@param	speed	スピード
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJCONT_MAP_SetFloorLightFire( WFLBY_3DMAPOBJ_CONT* p_sys, BOOL  flag, fx32 speed )
{
	int i;

	
	// 銅像のモデルの位置を調べ、位置の一致する銅像をアニメさせる
	for( i=0; i<p_sys->anmcont.hanabi_count; i++ ){
		
		WFLBY_3DMAPOBJCONT_MAP_SetFloorLightFireIdx( p_sys, i, flag, speed );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	床花火	ここに発射
 *
 *	@param	p_sys		システムワーク
 *	@param	idx			インデックス
 *	@param	flag		フラグ
 *	@param	speed		スピード
 *
 *	@retval	TRUE	設定できた
 *	@retval	FALSE	そのインデックスの花火はない（これ以降の花火はない）
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_3DMAPOBJCONT_MAP_SetFloorLightFireIdx( WFLBY_3DMAPOBJ_CONT* p_sys, u32 idx, BOOL  flag,  fx32 speed )
{

	int i;
	WFLBY_3DMAPOBJ_HANABIWK* p_hanabi;

	if( idx < p_sys->anmcont.hanabi_count ){

		p_hanabi = &p_sys->anmcont.hanabi[idx];

		if( flag ){
			WFLBY_3DMAPOBJ_HANABIWK_On( p_hanabi, p_sys->p_mapobjsys, speed );
		}else{
			WFLBY_3DMAPOBJ_HANABIWK_Off( p_hanabi );
		}

		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	床花火	でっかいの発射
 *
 *	@param	p_sys	システム
 *	@param	speed	再生スピード
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJCONT_MAP_SetFloorLightBigFire( WFLBY_3DMAPOBJ_CONT* p_sys, fx32 speed )
{
	int i;
	u32  mdltype;

	
	// 銅像のモデルの位置を調べ、位置の一致する銅像をアニメさせる
	for( i=0; i<p_sys->anmcont.hanabibig_count; i++ ){
		WFLBY_3DMAPOBJ_HANABIWK_OnBig( &p_sys->anmcont.hanabibig[i], speed );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	看板アニメの設定
 *
 *	@param	p_sys		システム
 *	@param	game		ゲームタイプ
 *	@param	num			接続人数
 *	@param	recruit		リクルート
 *	@param	play		遊び中か
 *	@param	light		消灯中か
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJCONT_MAP_SetMGAnm( WFLBY_3DMAPOBJ_CONT* p_sys, WFLBY_GAMETYPE game, u32 num, BOOL recruit, BOOL play, BOOL light )
{
	BOOL result;
	WFLBY_3DMAPOBJ_WK* p_kanban;
	WFLBY_3DMAPOBJ_WK* p_machine;
	u32 idx;

	
	// ゲームタイプからミニゲームインデックスを求める
	idx = game - WFLBY_GAME_BALLSLOW;

	p_machine	= p_sys->anmcont.p_mgmachine[ idx ];
	p_kanban	= p_sys->anmcont.p_mgkanban[ idx ];


	// 募集＆遊び中
	if( (play == TRUE) || (recruit == TRUE) ){
		// ミニゲーム機を動作させる
		if( WFLBY_3DMAPOBJ_WK_GetAnmFlag( p_machine, WFLBY_3DMAPOBJ_MG_ANM_MOVE ) == FALSE ){
			// とりあえず動作中アニメも
			WFLBY_3DMAPOBJ_WK_AddAnm( p_sys->p_mapobjsys, 
					p_machine, 
					WFLBY_3DMAPOBJ_MG_ANM_MOVE, WFLBY_3DMAPOBJ_WK_ANM_LOOP );	
		}

		// 看板
		// 人数のフレーム数を設定
		if( play == TRUE ){
			WFLBY_3DMAPOBJ_WK_SetAnmFrame( p_sys->p_mapobjsys, p_kanban, 
					WFLBY_3DMAPOBJ_KANBAN_ANM_LAMP, 
					FX32_CONST( (WFLBY_3DMAPOBJ_CONT_KANBAN_ANM_PLAY_0+num-1)*WFLBY_3DMAPOBJ_CONT_KANBAN_ANM_ONEFRAME ) );
		}else{
			// 募集中は点滅させる
			p_sys->anmcont.kanban_count = (p_sys->anmcont.kanban_count + 1) % WFLBY_3DMAPOBJ_CONT_KANBAN_ANM_MAX;
			if( p_sys->anmcont.kanban_count < WFLBY_3DMAPOBJ_CONT_KANBAN_ANM_HALF ){
				WFLBY_3DMAPOBJ_WK_SetAnmFrame( p_sys->p_mapobjsys, p_kanban, 
						WFLBY_3DMAPOBJ_KANBAN_ANM_LAMP, 
						FX32_CONST( (WFLBY_3DMAPOBJ_CONT_KANBAN_ANM_REC_0+num-1)*WFLBY_3DMAPOBJ_CONT_KANBAN_ANM_ONEFRAME ) );
			}else{
				WFLBY_3DMAPOBJ_WK_SetAnmFrame( p_sys->p_mapobjsys, p_kanban, 
						WFLBY_3DMAPOBJ_KANBAN_ANM_LAMP, 
						FX32_CONST( WFLBY_3DMAPOBJ_CONT_KANBAN_ANM_NONE*WFLBY_3DMAPOBJ_CONT_KANBAN_ANM_ONEFRAME ) );
			}
		}
	}else{

		// それぞれのアニメを設定する
		// 電源OFF時
		if( light ){
			WFLBY_3DMAPOBJ_WK_DelAnmAll( p_sys->p_mapobjsys, p_machine );
			WFLBY_3DMAPOBJ_WK_DelAnmAll( p_sys->p_mapobjsys, p_kanban );
			// ライトフラグをOFFにする
			WFLBY_3DMAPOBJ_WK_SetLightFlag( p_sys->p_mapobjsys, p_machine, 1<<WFLBY_DRAW_LIGHT_ROOM );
		}else{
			// ミニゲームはオブジェをとめる
			WFLBY_3DMAPOBJ_WK_DelAnm( p_sys->p_mapobjsys, 
					p_machine, WFLBY_3DMAPOBJ_MG_ANM_MOVE );	

			// 看板はフレーム０にする
			WFLBY_3DMAPOBJ_WK_SetAnmFrame( p_sys->p_mapobjsys, p_kanban, 
						WFLBY_3DMAPOBJ_KANBAN_ANM_LAMP, 0 );
		}
	}
}









//-----------------------------------------------------------------------------
/**
 *			プライベート関数
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	グリッドとオブジェの位置が一致するかチェック
 *
 *	@param	cp_wk		ワーク
 *	@param	gridx		グリッドｘ
 *	@param	gridy		グリッドｙ
 *
 *	@retval	TRUE	座標とグリッドが一致する
 *	@retval	FALSE	一致しない
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_3DOBJCONT_MAPOBJWK_CheckPosGrid( const WFLBY_3DMAPOBJ_WK* cp_wk, u8 gridx, u8 gridy )
{
	WF2DMAP_POS pos;
	WF2DMAP_POS chk_pos;

	// グリッドを座標に変換
	chk_pos.x = WF2DMAP_GRID2POS(gridx);
	chk_pos.y = WF2DMAP_GRID2POS(gridy);
	
	pos = WFLBY_3DMAPOBJ_WK_GetPos( cp_wk );
	if( (pos.x == chk_pos.x) &&
		(pos.y == chk_pos.y) ){
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	グリッド位置にいる配置オブジェを取得する
 *
 *	@param	p_sys		システム
 *	@param	gridx		グリッド
 *	@param	gridy 
 *
 *	@retval	NULL	いない
 *	@retval	そのグリッドにいるワーク
 */
//-----------------------------------------------------------------------------
static WFLBY_3DMAPOBJ_WK* WFLBY_3DOBJCONT_MAPOBJWK_GetGridWk( WFLBY_3DMAPOBJ_CONT* p_sys, u8 gridx, u8 gridy )
{
	int i;
	BOOL result;

	
	// 銅像のモデルの位置を調べ、位置の一致する銅像をアニメさせる
	for( i=0; i<p_sys->objaddnum; i++ ){
		result = WFLBY_3DOBJCONT_MAPOBJWK_CheckPosGrid( p_sys->p_mapwkobj[i], gridx, gridy );
		if( result == TRUE ){
			return p_sys->p_mapwkobj[i];
		}
	}
	return NULL;
}


//----------------------------------------------------------------------------
/**
 *	@brief	ランプ　ゆれ設定コールバック
 *
 *	@param	p_sys		システム
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_LAMP_YURE_SetCallBack( WFLBY_3DMAPOBJ* p_sys, WFLBY_3DMAPOBJ_WK* p_wk )
{
	u32 mdltype;
	
	// オブジェタイプがちゃんとランプかチェック
	mdltype = WFLBY_3DMAPOBJ_WK_GetType( p_wk );
	GF_ASSERT( (mdltype == WFLBY_3DMAPOBJ_LAMP00) || (mdltype == WFLBY_3DMAPOBJ_LAMP01) );
	if( (mdltype == WFLBY_3DMAPOBJ_LAMP00) || (mdltype == WFLBY_3DMAPOBJ_LAMP01) ){
		WFLBY_3DMAPOBJ_WK_AddAnm( p_sys, 
				p_wk, 
				WFLBY_3DMAPOBJ_LAMP_ANM_YURE, WFLBY_3DMAPOBJ_WK_ANM_LOOP );	
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	ランプなしアニメ設定
 *
 *	@param	p_sys	システム
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_LAMP_NONE_SetCallBack( WFLBY_3DMAPOBJ* p_sys, WFLBY_3DMAPOBJ_WK* p_wk )
{
	u32 mdltype;
	
	// オブジェタイプがちゃんとランプかチェック
	mdltype = WFLBY_3DMAPOBJ_WK_GetType( p_wk );
	GF_ASSERT( (mdltype == WFLBY_3DMAPOBJ_LAMP00) || (mdltype == WFLBY_3DMAPOBJ_LAMP01) );
	if( (mdltype == WFLBY_3DMAPOBJ_LAMP00) || (mdltype == WFLBY_3DMAPOBJ_LAMP01) ){
		WFLBY_3DMAPOBJ_WK_AddAnm( p_sys, p_wk, 
				WFLBY_3DMAPOBJ_LAMP_ANM_ON, WFLBY_3DMAPOBJ_WK_ANM_NOAUTO );	
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	表示をＯＦＦするコールバック
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_LAMP_DrawOff_SetCallBack( WFLBY_3DMAPOBJ* p_sys, WFLBY_3DMAPOBJ_WK* p_wk )
{
	WFLBY_3DMAPOBJ_WK_SetDraw( p_wk, FALSE );
}



//----------------------------------------------------------------------------
/**
 *	@brief	銅像アニメの再生	通常
 *
 *	@param	p_sys		システムワーク
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_DouzouAnm_PlayNormal( WFLBY_3DMAPOBJ_CONT* p_sys, WFLBY_3DMAPOBJ_WK* p_wk )
{
	fx32 anm_frame;
	BOOL anm_start;
	int i;

	// アニメ開始
	anm_start = FALSE;	

	// アニメが開始中かチェック
	if( WFLBY_3DMAPOBJ_WK_CheckAnm( p_sys->p_mapobjsys, p_wk, WFLBY_3DMAPOBJ_SMALLBRONZE_ANM_TOUCH ) == FALSE ){
		anm_start = TRUE;	// 絶対にアニメ開始
	}else{
		// フレーム数が規定の位置まで行ってなければアニメ開始させない
		anm_frame = WFLBY_3DMAPOBJ_WK_GetAnmFrame( p_wk, WFLBY_3DMAPOBJ_SMALLBRONZE_ANM_TOUCH );
		if( anm_frame > WFLBY_3DMAPOBJ_CONT_DOUZOU_ANM_RESTART_FRAME ){
			anm_start = TRUE;
		}
	}
	
	// anm_startがTRUEならアニメ再生
	if( anm_start == TRUE ){
		
		// 銅像アニメ再生
		WFLBY_3DMAPOBJ_DouzouAnm_PlayAnm( p_sys, p_wk );	


		// 銅像動作音設定
		Snd_SePlay( WFLBY_SND_DOUZOU );
	}
}
// 特殊部屋用
static void WFLBY_3DMAPOBJ_DouzouAnm_PlaySpecial( WFLBY_3DMAPOBJ_CONT* p_sys, WFLBY_3DMAPOBJ_WK* p_wk )
{
	BOOL anm_start;

	// アニメ開始
	anm_start = FALSE;	

	// アニメが開始中かチェック
	if( WFLBY_3DMAPOBJ_WK_CheckAnm( p_sys->p_mapobjsys, p_wk, WFLBY_3DMAPOBJ_SMALLBRONZE_ANM_TOUCH ) == FALSE ){
		anm_start = TRUE;	// 絶対にアニメ開始
	}
	
	// anm_startがTRUEならアニメ再生
	if( anm_start == TRUE ){

		// 銅像アニメ再生
		WFLBY_3DMAPOBJ_DouzouAnm_PlayAnm( p_sys, p_wk );	

		// 銅像動作音設定
		Snd_SePlay( WFLBY_SND_DOUZOU2 );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	実際に銅像アニメを再生
 *
 *	@param	p_sys		システムワーク
 *	@param	p_wk		描画ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_DouzouAnm_PlayAnm( WFLBY_3DMAPOBJ_CONT* p_sys, WFLBY_3DMAPOBJ_WK* p_wk )
{
	int i;

	for( i=WFLBY_3DMAPOBJ_SMALLBRONZE_ANM_TOUCH; i<WFLBY_3DMAPOBJ_SMALLBRONZE_ANM_NUM; i++ ){

		WFLBY_3DMAPOBJ_WK_AddAnm( p_sys->p_mapobjsys, p_wk,
				i, WFLBY_3DMAPOBJ_WK_ANM_NOLOOP );
	}
}



//----------------------------------------------------------------------------
/**
 *	@brief	床花火管理システム	初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_hanabi	花火ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_HANABIWK_Init( WFLBY_3DMAPOBJ_HANABIWK* p_wk, WFLBY_3DMAPOBJ_WK* p_hanabi )
{
	p_wk->p_hanabi	= p_hanabi;
	p_wk->status	= WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_ST_ONWAIT;
}

//----------------------------------------------------------------------------
/**
 *	@brief	床花火管理システム	メイン
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_HANABIWK_Main( WFLBY_3DMAPOBJ_HANABIWK* p_wk, WFLBY_3DMAPOBJ* p_mapobjsys )
{
	switch( p_wk->status ){
	// 表示開始待ち
	case WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_ST_ONWAIT:
		break;

	// 表示開始
	case WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_ST_ONSTART:
		WFLBY_3DMAPOBJ_HANABIWK_OnDraw( p_wk, p_mapobjsys );

		p_wk->status = WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_ST_ON;
		break;
		
	// 表示中
	case WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_ST_ON:
		break;

	// 表示OFF開始
	case WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_ST_OFFSTART:
		WFLBY_3DMAPOBJ_HANABIWK_OffMoveStart( p_wk );
		p_wk->status		= WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_ST_OFF;
		break;
		
	// 表示OFF
	case WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_ST_OFF:
		if( WFLBY_3DMAPOBJ_HANABIWK_OffMove( p_wk, p_mapobjsys ) == TRUE ){

			// 終了
			WFLBY_3DMAPOBJ_HANABIWK_OffDraw( p_wk, p_mapobjsys );

			// 表示開始待ち処理へ
			p_wk->status = WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_ST_ONWAIT;
		}
		break;

	default:
		break;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	床花火管理システム	花火表示開始
 *
 *	@param	p_wk		ワーク
 *	@param	p_mapobjsys	配置オブジェ描画システム
 *	@param	speed		アニメスピード
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_HANABIWK_On( WFLBY_3DMAPOBJ_HANABIWK* p_wk, WFLBY_3DMAPOBJ* p_mapobjsys, fx32 speed )
{
	// もう表示中なら何もしない
	if( (p_wk->status == WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_ST_ONSTART) ||
		(p_wk->status == WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_ST_ON) ){
		return ;
	}

	// 破棄中の場合、強制的に破棄する
	if( (p_wk->status == WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_ST_OFFSTART) ||
		(p_wk->status == WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_ST_OFF) ){
		WFLBY_3DMAPOBJ_HANABIWK_OffDraw( p_wk, p_mapobjsys );
	}

	// 表示開始
	p_wk->status		= WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_ST_ONSTART;
	p_wk->start_speed	= speed;
}

//----------------------------------------------------------------------------
/**
 *	@brief	床花火管理システム	花火表示終了
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_HANABIWK_Off( WFLBY_3DMAPOBJ_HANABIWK* p_wk )
{
	// 破棄中なら何もしない
	if( (p_wk->status == WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_ST_OFFSTART) ||
		(p_wk->status == WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_ST_OFF) ){
		return ;
	}

	// 描画してないなら何もしない
	if( (p_wk->status == WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_ST_ONSTART) ||
		(p_wk->status == WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_ST_ONWAIT) ){
		p_wk->status = WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_ST_ONWAIT;
		return ;
	}
	
	// 破棄
	p_wk->status = WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_ST_OFFSTART;
}


//----------------------------------------------------------------------------
/**
 *	@brief	床花火管理システム	実際に花火を表示する
 *	
 *	@param	p_wk		ワーク
 *	@param	p_mapobjsys	配置オブジェ描画管理システム
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_HANABIWK_OnDraw( WFLBY_3DMAPOBJ_HANABIWK* p_wk, WFLBY_3DMAPOBJ* p_mapobjsys )
{
	WFLBY_3DMAPOBJ_WK_SetAnmSpeed( p_wk->p_hanabi, p_wk->start_speed );
	
	WFLBY_3DMAPOBJ_WK_AddAnm( p_mapobjsys, 
			p_wk->p_hanabi, 
			WFLBY_3DMAPOBJ_HANABI_ANM_NICA, WFLBY_3DMAPOBJ_WK_ANM_LOOP );	
	WFLBY_3DMAPOBJ_WK_AddAnm( p_mapobjsys, 
			p_wk->p_hanabi, 
			WFLBY_3DMAPOBJ_HANABI_ANM_NITA, WFLBY_3DMAPOBJ_WK_ANM_LOOP );	
	WFLBY_3DMAPOBJ_WK_AddAnm( p_mapobjsys, 
			p_wk->p_hanabi, 
			WFLBY_3DMAPOBJ_HANABI_ANM_NITP, WFLBY_3DMAPOBJ_WK_ANM_LOOP );	

	WFLBY_3DMAPOBJ_WK_SetDraw( p_wk->p_hanabi, TRUE );
}

//----------------------------------------------------------------------------
/**
 *	@brief	床花火管理システム	実際に花火を表示をやめる
 *	
 *	@param	p_wk		ワーク
 *	@param	p_mapobjsys	配置オブジェ描画管理システム
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_HANABIWK_OffDraw( WFLBY_3DMAPOBJ_HANABIWK* p_wk, WFLBY_3DMAPOBJ* p_mapobjsys )
{
	WFLBY_3DMAPOBJ_WK_DelAnm( p_mapobjsys, 
			p_wk->p_hanabi, 
			WFLBY_3DMAPOBJ_HANABI_ANM_NICA );	
	WFLBY_3DMAPOBJ_WK_DelAnm( p_mapobjsys, 
			p_wk->p_hanabi, 
			WFLBY_3DMAPOBJ_HANABI_ANM_NITA );	
	WFLBY_3DMAPOBJ_WK_DelAnm( p_mapobjsys, 
			p_wk->p_hanabi, 
			WFLBY_3DMAPOBJ_HANABI_ANM_NITP );	

	WFLBY_3DMAPOBJ_WK_SetDraw( p_wk->p_hanabi, FALSE );

	WFLBY_3DMAPOBJ_WK_ResetAlpha( p_wk->p_hanabi );

	WFLBY_3DMAPOBJ_WK_SetScale( p_wk->p_hanabi, FX32_ONE, FX32_ONE, FX32_ONE );
}

//----------------------------------------------------------------------------
/**
 *	@brief	床花火管理システム	花火終了動作	初期化
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_HANABIWK_OffMoveStart( WFLBY_3DMAPOBJ_HANABIWK* p_wk )
{
	p_wk->count = 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief	床花火管理システム	花火終了動作	メイン
 *
 *	@param	p_wk		ワーク
 *	@param	p_mapobjsys	配置オブジェ管理システム
 *
 *	@retval	TRUE	終了
 *	@retval	FALSE	途中
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_3DMAPOBJ_HANABIWK_OffMove( WFLBY_3DMAPOBJ_HANABIWK* p_wk, WFLBY_3DMAPOBJ* p_mapobjsys )
{
	if( p_wk->count < WFLBY_3DMAPOBJ_CONT_HANABI_COUNT_MAX ){
		p_wk->count ++;

		// 再生フレーム設定
		{
			fx32 dif_speed;
			fx32 now_speed;
			dif_speed = p_wk->start_speed - WFLBY_3DMAPOBJ_CONT_HANABI_SPEED_END;
			now_speed = FX_Mul( p_wk->count<<FX32_SHIFT, dif_speed );
			now_speed = FX_Div( now_speed, WFLBY_3DMAPOBJ_CONT_HANABI_COUNT_MAX<<FX32_SHIFT );
			now_speed += p_wk->start_speed;
			WFLBY_3DMAPOBJ_WK_SetAnmSpeed( p_wk->p_hanabi, now_speed );
		}

		// アルファ設定
		{
			u32 alpha;

			alpha = (p_wk->count * WFLBY_3DMAPOBJ_CONT_HANABI_ALPHA_DEF) / WFLBY_3DMAPOBJ_CONT_HANABI_COUNT_MAX;
			alpha = WFLBY_3DMAPOBJ_CONT_HANABI_ALPHA_DEF - alpha;
			WFLBY_3DMAPOBJ_WK_SetAlpha( p_wk->p_hanabi, alpha, WFLBY_3DMAPOBJ_CONT_HANABI_ALPHA_DEF );
		}

		// スケール
		{
			fx32 scale;

			scale = FX_Mul( p_wk->count<<FX32_SHIFT, WFLBY_3DMAPOBJ_CONT_HANABI_SCALE_DIF );
			scale = FX_Div( scale, WFLBY_3DMAPOBJ_CONT_HANABI_COUNT_MAX<<FX32_SHIFT );
			scale = FX32_ONE + scale;
			
			WFLBY_3DMAPOBJ_WK_SetScale( p_wk->p_hanabi, scale, scale, scale );
		}

		return FALSE;
	}

	return TRUE;
}


//----------------------------------------------------------------------------
/**
 *	@brief	床花火管理システム	花火を表示開始	でかい花火
 *
 *	@param	p_wk			ワーク
 *	@param	speed			アニメスピード
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_HANABIWK_OnBig( WFLBY_3DMAPOBJ_HANABIWK* p_wk, fx32 speed )
{
	// もう表示中なら何もしない
	if( (p_wk->status == WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_ST_ONSTART) ||
		(p_wk->status == WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_ST_ON) ){
		return ;
	}

	// 破棄中の場合はそのまま新しく出しちゃう
/*	if( (p_wk->status == WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_ST_OFFSTART) ||
		(p_wk->status == WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_ST_OFF) ){
		return ;
	}//*/

	// 表示開始
	p_wk->status		= WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_ST_ONSTART;
	p_wk->start_speed	= speed;
}

//----------------------------------------------------------------------------
/**
 *	@brief	床花火管理システム	メイン
 *
 *	@param	p_wk		ワーク
 *	@param	p_mapobjsys 配置オブジェ描画管理システム
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_HANABIWK_MainBig( WFLBY_3DMAPOBJ_HANABIWK* p_wk, WFLBY_3DMAPOBJ* p_mapobjsys )
{
	switch( p_wk->status ){
	// 表示開始待ち
	case WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_ST_ONWAIT:
		break;

	// 表示開始
	case WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_ST_ONSTART:
		WFLBY_3DMAPOBJ_HANABIWK_OnBigDraw( p_wk, p_mapobjsys );

		p_wk->status = WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_ST_ON;
		break;
		
	// 表示中
	case WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_ST_ON:
		// アニメの終了待ち
		if( WFLBY_3DMAPOBJ_WK_CheckAnm( p_mapobjsys, p_wk->p_hanabi, 0 ) == FALSE ){

			// 開始待ちへ
			p_wk->status = WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_ST_OFFSTART;
		}
		break;

	// 表示OFF開始
	case WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_ST_OFFSTART:
		WFLBY_3DMAPOBJ_HANABIWK_OffMoveStart( p_wk );
		p_wk->status		= WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_ST_OFF;
		break;
		
	// 表示OFF
	case WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_ST_OFF:
		if( WFLBY_3DMAPOBJ_HANABIWK_OffMove( p_wk, p_mapobjsys ) == TRUE ){

			// 表示OFF終了
			WFLBY_3DMAPOBJ_WK_SetDraw( p_wk->p_hanabi, FALSE );

			WFLBY_3DMAPOBJ_WK_ResetAlpha( p_wk->p_hanabi );

			WFLBY_3DMAPOBJ_WK_SetScale( p_wk->p_hanabi, FX32_ONE, FX32_ONE, FX32_ONE );

			// 表示開始待ち処理へ
			p_wk->status = WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_ST_ONWAIT;
		}
		break;

	default:
		GF_ASSERT( 0 );
		break;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	床花火管理システム	実際に花火を表示開始	でかい花火
 *	
 *	@param	p_wk		ワーク
 *	@param	p_mapobjsys	配置オブジェ描画管理システム
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_HANABIWK_OnBigDraw( WFLBY_3DMAPOBJ_HANABIWK* p_wk, WFLBY_3DMAPOBJ* p_mapobjsys )
{
	WFLBY_3DMAPOBJ_WK_SetAnmSpeed( p_wk->p_hanabi, p_wk->start_speed );

	WFLBY_3DMAPOBJ_WK_AddAnm( p_mapobjsys, 
			p_wk->p_hanabi, 
			WFLBY_3DMAPOBJ_HANABIBIG_ANM_NICA, WFLBY_3DMAPOBJ_WK_ANM_NOLOOP );	
	WFLBY_3DMAPOBJ_WK_AddAnm( p_mapobjsys, 
			p_wk->p_hanabi, 
			WFLBY_3DMAPOBJ_HANABIBIG_ANM_NITP, WFLBY_3DMAPOBJ_WK_ANM_NOLOOP );	

	WFLBY_3DMAPOBJ_WK_SetDraw( p_wk->p_hanabi, TRUE );
}



//----------------------------------------------------------------------------
/**
 *	@brief	床花火管理システム	メイン		銅像でか花火用
 *
 *	@param	p_wk		ワーク
 *	@param	p_mapobjsys 配置オブジェ描画管理システム
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_HANABIWK_MainBrBig( WFLBY_3DMAPOBJ_HANABIWK* p_wk, WFLBY_3DMAPOBJ* p_mapobjsys )
{
	switch( p_wk->status ){
	// 表示開始待ち
	case WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_ST_ONWAIT:
		break;

	// 表示開始
	case WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_ST_ONSTART:
		WFLBY_3DMAPOBJ_HANABIWK_OnBrBigDraw( p_wk, p_mapobjsys );

		p_wk->status = WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_ST_ON;
		break;
		
	// 表示中
	case WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_ST_ON:
		// アニメの終了待ち
		if( WFLBY_3DMAPOBJ_WK_CheckAnm( p_mapobjsys, p_wk->p_hanabi, 0 ) == FALSE ){

			// 開始待ちへ
			p_wk->status = WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_ST_OFFSTART;
		}
		break;

	// 表示OFF開始
	case WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_ST_OFFSTART:
		WFLBY_3DMAPOBJ_HANABIWK_OffMoveStart( p_wk );
		p_wk->status		= WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_ST_OFF;
		break;
		
	// 表示OFF
	case WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_ST_OFF:
		if( WFLBY_3DMAPOBJ_HANABIWK_OffMove( p_wk, p_mapobjsys ) == TRUE ){

			// 表示OFF終了
			WFLBY_3DMAPOBJ_WK_SetDraw( p_wk->p_hanabi, FALSE );

			WFLBY_3DMAPOBJ_WK_ResetAlpha( p_wk->p_hanabi );

			WFLBY_3DMAPOBJ_WK_SetScale( p_wk->p_hanabi, FX32_ONE, FX32_ONE, FX32_ONE );

			// 表示開始待ち処理へ
			p_wk->status = WFLBY_3DMAPOBJ_CONT_HANABI_OBJ_ST_ONWAIT;
		}
		break;

	default:
		GF_ASSERT( 0 );
		break;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	床花火管理システム	実際に花火を表示開始	銅像でかい花火
 *	
 *	@param	p_wk		ワーク
 *	@param	p_mapobjsys	配置オブジェ描画管理システム
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_HANABIWK_OnBrBigDraw( WFLBY_3DMAPOBJ_HANABIWK* p_wk, WFLBY_3DMAPOBJ* p_mapobjsys )
{
	WFLBY_3DMAPOBJ_WK_SetAnmSpeed( p_wk->p_hanabi, p_wk->start_speed );
	
	//  花火再生
	WFLBY_3DMAPOBJ_WK_AddAnm( p_mapobjsys, 
			p_wk->p_hanabi, 
			WFLBY_3DMAPOBJ_BRHANABIBIG_ANM_NICA, WFLBY_3DMAPOBJ_WK_ANM_NOLOOP );	

	WFLBY_3DMAPOBJ_WK_AddAnm( p_mapobjsys, 
			p_wk->p_hanabi, 
			WFLBY_3DMAPOBJ_BRHANABIBIG_ANM_NITA, WFLBY_3DMAPOBJ_WK_ANM_NOLOOP );	

	WFLBY_3DMAPOBJ_WK_AddAnm( p_mapobjsys, 
			p_wk->p_hanabi, 
			WFLBY_3DMAPOBJ_BRHANABIBIG_ANM_NITP, WFLBY_3DMAPOBJ_WK_ANM_NOLOOP );	

	WFLBY_3DMAPOBJ_WK_SetDraw( p_wk->p_hanabi, TRUE );
}


//----------------------------------------------------------------------------
/**
 *	@brief	大きい銅像用花火座標を設定する
 *
 *	@param	p_hanabi	花火ワーク
 *	@param	mapid		配置オブジェ管理ID
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_DOUZOU00_HANABI_SetPos( WFLBY_3DMAPOBJ_WK* p_hanabi, u32 mapid )
{
	WF2DMAP_POS pos;
	u32 fire_offs_idx;

	GF_ASSERT( (mapid == WFLBY_MAPOBJID_DOUZOU00FIRE00) || (mapid == WFLBY_MAPOBJID_DOUZOU00FIRE01) );

	fire_offs_idx = mapid - WFLBY_MAPOBJID_DOUZOU00FIRE00;
	pos = WFLBY_3DMAPOBJ_WK_GetPos( p_hanabi );
	pos.x += sc_WFLBY_3DMAPOBJ_CONT_DOUZOU00_FIRE_OFFS_X[fire_offs_idx];
	pos.y += WFLBY_3DMAPOBJ_CONT_DOUZOU00_FIRE_OFFS_Y;
	WFLBY_3DMAPOBJ_WK_SetPos( p_hanabi, pos );
}
