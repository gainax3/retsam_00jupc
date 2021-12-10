//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wflby_3dmapobj.c
 *	@brief		マップ表示物管理
 *	@author		tomoya takahashi
 *	@data		2007.11.14
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "common.h"

#include "system/arc_tool.h"

#include "graphic/wifi_lobby.naix"

#include "wflby_3dmapobj.h"
#include "wflby_3dmatrix.h"
#include "map_conv/wflby_3dmapobj_data.h"
#include "map_conv/wflby_maparc.h"


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
//#define WFLBY_DEBUG_3DMAPOBJ_PRINT_TIME	// 処理速度を表示
#endif

#ifdef WFLBY_DEBUG_3DMAPOBJ_PRINT_TIME

static OSTick	WFLBY_DEBUG_3DMAPOBJ_PRINT_TIME_Tick;
#define WFLBY_DEBUG_3DMAPOBJ_PRINT_TIME_TICK_INIT	WFLBY_DEBUG_3DMAPOBJ_PRINT_TIME_Tick = OS_GetTick();
#define WFLBY_DEBUG_3DMAPOBJ_PRINT_TIME_TICK_PRINT(line)	OS_TPrintf( "	od line[%d] time %d micro\n", (line), OS_TicksToMicroSeconds(OS_GetTick() - WFLBY_DEBUG_3DMAPOBJ_PRINT_TIME_Tick) );

#else		// WFLBY_DEBUG_3DMAPOBJ_PRINT_TIME

#define		WFLBY_DEBUG_3DMAPOBJ_PRINT_TIME_TICK_INIT			((void)0);
#define		WFLBY_DEBUG_3DMAPOBJ_PRINT_TIME_TICK_PRINT(line)	((void)0);

#endif		// WFLBY_DEBUG_3DMAPOBJ_PRINT_TIME


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
///	地面ワーク
//=====================================
typedef struct _WFLBY_3DMAPOBJ_MAP{
	BOOL		on;
	D3DOBJ		obj[ WFLBY_3DMAPOBJ_MAPOBJ_NUM ];
	u32			anm_on[ WFLBY_3DMAPOBJ_MAP_ANM_NUM ];
	fx32		anm_frame[ WFLBY_3DMAPOBJ_MAP_ANM_NUM ];
}WFLBY_3DMAPOBJ_MAP;

//-------------------------------------
///	地面リソース
//=====================================
typedef struct {
	D3DOBJ_MDL	mdl[ WFLBY_3DMAPOBJ_MAPOBJ_NUM ];
	D3DOBJ_ANM	anm[ WFLBY_3DMAPOBJ_MAP_ANM_NUM ];
	BOOL		anm_load[ WFLBY_3DMAPOBJ_MAP_ANM_NUM ];
} WFLBY_3DMAPOBJ_MAPRES;


//-------------------------------------
///	フロートワーク
//=====================================
typedef struct _WFLBY_3DMAPOBJ_FLOAT{
	u8			on;	
	u8			col;	// 色
	u8			mdlno;	// モデルナンバー
	u8			pad;
	D3DOBJ		obj;
	u32			anm_on[ WFLBY_3DMAPOBJ_FLOAT_ANM_NUM ];
	fx32		anm_frame[ WFLBY_3DMAPOBJ_FLOAT_ANM_NUM ];
	VecFx32		mat;
	VecFx32		ofs;
}WFLBY_3DMAPOBJ_FLOAT;

//-------------------------------------
///	フロートリソース
//=====================================
typedef struct {
	D3DOBJ_MDL	mdl[ WFLBY_3DMAPOBJ_FLOAT_NUM ];
	void*		p_texres[ WFLBY_3DMAPOBJ_FLOAT_NUM ][ WFLBY_3DMAPOBJ_FLOAT_COL_NUM ];
	D3DOBJ_ANM	anm[ WFLBY_3DMAPOBJ_FLOAT_NUM ][ WFLBY_3DMAPOBJ_FLOAT_ANM_NUM ];
} WFLBY_3DMAPOBJ_FLOATRES;

//-------------------------------------
///	その他の物ワーク
//	(表示非表示くらいしか出来ない)
//=====================================
typedef struct _WFLBY_3DMAPOBJ_WK{
	u16			on;
	u16			mdlid;
	D3DOBJ		obj;
	D3DOBJ_ANM	anm[ WFLBY_3DMAPOBJ_ALL_ANM_MAX ];
	u8			anm_on[ WFLBY_3DMAPOBJ_ALL_ANM_MAX ];
	u8			anm_on_pad;
	u8			play[ WFLBY_3DMAPOBJ_ALL_ANM_MAX ];
	u8			play_pad;
	fx32		anm_frame[ WFLBY_3DMAPOBJ_ALL_ANM_MAX ];
	u8			wait_def;							// ランダムウエイト定数	
	u8			wait[ WFLBY_3DMAPOBJ_ALL_ANM_MAX ];	// ウエイト数
	pWFLBY_3DMAPOBJ_WK_AnmCallBack	p_anmcallback[ WFLBY_3DMAPOBJ_ALL_ANM_MAX ];
	fx32		speed;		// アニメスピード
	u8			alpha_flag;	// アルファ値設定フラグ
	u8			alpha;		// アルファ値
	u8			def_alpha;	// 基本アルファ値
	u8			pad;	// 基本アルファ値
}WFLBY_3DMAPOBJ_WK;

//-------------------------------------
///	その他の配置オブジェリソース
//=====================================
typedef struct {
	D3DOBJ_MDL	mdl[ WFLBY_3DMAPOBJ_WK_NUM ];
	void*		p_anm[ WFLBY_3DMAPOBJ_WK_NUM ][ WFLBY_3DMAPOBJ_ALL_ANM_MAX ];
} WFLBY_3DMAPOBJ_WKRES;

//-------------------------------------
///	マップ表示物管理システム
//=====================================
typedef struct _WFLBY_3DMAPOBJ{
	WFLBY_3DMAPOBJ_MAP		map;		// マップワーク	
	WFLBY_3DMAPOBJ_FLOAT*	p_float;	// フロート
	WFLBY_3DMAPOBJ_WK*		p_obj;		// 配置オブジェ
	u8						floatnum;	// フロート数
	u8						objnum;		// 配置オブジェ数
	u8						room;		// 部屋タイプ
	u8						season;		// シーズンタイプ

	BOOL						res_load;								// リソースを読み込んだか
	WFLBY_3DMAPOBJ_MAPRES		mapres;									// マップワーク	
	WFLBY_3DMAPOBJ_FLOATRES		floatres;								// フロート
	WFLBY_3DMAPOBJ_WKRES		objres;									// 配置オブジェ
	NNSFndAllocator				allocator;								// アロケータ
}WFLBY_3DMAPOBJ;



//-----------------------------------------------------------------------------
/**
 *				配置オブジェクトデータ
 */
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
// シーズンの部屋の読み込みデータ作成
static WFLBY_3DMAPOBJ_MDL_DATA* WFLBY_3DMAPOBJ_MDLRES_DATA_Init( WFLBY_ROOM_TYPE room, WFLBY_SEASON_TYPE season, u32 heapID );
static void WFLBY_3DMAPOBJ_MDLRES_DATA_Exit( WFLBY_3DMAPOBJ_MDL_DATA* p_data );


// 広場用モデル読み込みシステム
static void WFLBY_3DMAPOBJ_MDL_Load( D3DOBJ_MDL* p_mdl, ARCHANDLE* p_handle, u32 data_idx, u32 gheapID );
static void WFLBY_3DMAPOBJ_MDL_Delete( D3DOBJ_MDL* p_mdl );

// アニメフレーム管理
static void WFLBY_3DMAPOBJ_ANM_Loop( fx32* p_frame, const D3DOBJ_ANM* cp_anm, fx32 speed );
static BOOL WFLBY_3DMAPOBJ_ANM_NoLoop( fx32* p_frame, const D3DOBJ_ANM* cp_anm, fx32 speed );
static void WFLBY_3DMAPOBJ_ANM_BackLoop( fx32* p_frame, const D3DOBJ_ANM* cp_anm, fx32 speed );
static BOOL WFLBY_3DMAPOBJ_ANM_BackNoLoop( fx32* p_frame, const D3DOBJ_ANM* cp_anm, fx32 speed );

// マップワーク
static void WFLBY_3DMAPOBJ_MAP_Load( WFLBY_3DMAPOBJ_MAPRES* p_wk, ARCHANDLE* p_handle, const WFLBY_3DMAPOBJ_MDL_DATA* cp_data, u32 gheapID, NNSFndAllocator* p_allocator );
static void WFLBY_3DMAPOBJ_MAP_Release( WFLBY_3DMAPOBJ_MAPRES* p_wk, NNSFndAllocator* p_allocator );
static void WFLBY_3DMAPOBJ_MAP_Add( WFLBY_3DMAPOBJ_MAP* p_wk, WFLBY_3DMAPOBJ_MAPRES* p_res );
static void WFLBY_3DMAPOBJ_MAP_Delete( WFLBY_3DMAPOBJ_MAP* p_wk, WFLBY_3DMAPOBJ_MAPRES* p_res );
static void WFLBY_3DMAPOBJ_MAP_Main( WFLBY_3DMAPOBJ_MAP* p_wk, WFLBY_3DMAPOBJ_MAPRES* p_res );
static void WFLBY_3DMAPOBJ_MAP_Draw( WFLBY_3DMAPOBJ_MAP* p_wk, WFLBY_3DMAPOBJ_MAPRES* p_res );

// フロートワーク
static WFLBY_3DMAPOBJ_FLOAT* WFLBY_3DMAPOBJ_FLOAT_GetCleanWk( WFLBY_3DMAPOBJ* p_sys );
static void WFLBY_3DMAPOBJ_FLOAT_Load( WFLBY_3DMAPOBJ_FLOATRES* p_wk, ARCHANDLE* p_handle, NNSFndAllocator* p_allocator, const WFLBY_3DMAPOBJ_MDL_DATA* cp_data, u32 gheapID );
static void WFLBY_3DMAPOBJ_FLOAT_Release( WFLBY_3DMAPOBJ_FLOATRES* p_wk, NNSFndAllocator* p_allocator );
static void WFLBY_3DMAPOBJ_FLOAT_Main( WFLBY_3DMAPOBJ_FLOAT* p_wk, WFLBY_3DMAPOBJ_FLOATRES* p_res );
static void WFLBY_3DMAPOBJ_FLOAT_Draw( WFLBY_3DMAPOBJ_FLOAT* p_wk, WFLBY_3DMAPOBJ_FLOATRES* p_res );

// 配置オブジェクトワーク
static WFLBY_3DMAPOBJ_WK* WFLBY_3DMAPOBJ_WK_GetCleanWk( WFLBY_3DMAPOBJ* p_sys );
static void WFLBY_3DMAPOBJ_WK_Load( WFLBY_3DMAPOBJ_WKRES* p_wk, ARCHANDLE* p_handle, NNSFndAllocator* p_allocator, const WFLBY_3DMAPOBJ_MDL_DATA* cp_data, u32 gheapID );
static void WFLBY_3DMAPOBJ_WK_Release( WFLBY_3DMAPOBJ_WKRES* p_wk, NNSFndAllocator* p_allocator );
static void WFLBY_3DMAPOBJ_WK_Draw( WFLBY_3DMAPOBJ_WKRES* p_res, WFLBY_3DMAPOBJ_WK* p_wk );
static void WFLBY_3DMAPOBJ_WK_Anm( WFLBY_3DMAPOBJ* p_sys, WFLBY_3DMAPOBJ_WK* p_wk );



// システム管理
//----------------------------------------------------------------------------
/**
 *	@brief	マップオブジェシステム	初期化
 *
 *	@param	float_num		フロート数
 *	@param	objwk_num		そのたの表示物数
 *	@param	heapID			ヒープID
 *	@param	gheapID			グラフィックヒープID
 *
 *	@return	システムワーク
 */
//-----------------------------------------------------------------------------
WFLBY_3DMAPOBJ* WFLBY_3DMAPOBJ_Init( u32 float_num, u32 objwk_num, u32 heapID, u32 gheapID )
{
	WFLBY_3DMAPOBJ* p_sys;

	p_sys = sys_AllocMemory( heapID, sizeof(WFLBY_3DMAPOBJ) );
	memset( p_sys,  0, sizeof(WFLBY_3DMAPOBJ) );

	// 各ワークを作成
	p_sys->p_float	= sys_AllocMemory( heapID, sizeof(WFLBY_3DMAPOBJ_FLOAT)*float_num );
	p_sys->p_obj	= sys_AllocMemory( heapID, sizeof(WFLBY_3DMAPOBJ_WK)*objwk_num );
	memset( p_sys->p_float, 0, sizeof(WFLBY_3DMAPOBJ_FLOAT)*float_num );
	memset( p_sys->p_obj, 0, sizeof(WFLBY_3DMAPOBJ_WK)*objwk_num );
	p_sys->floatnum	= float_num;
	p_sys->objnum	= objwk_num;
	
	
	return p_sys;
}

//----------------------------------------------------------------------------
/**
 *	@brief	マップオブジェシステム	破棄
 *
 *	@param	p_sys		ワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_Exit( WFLBY_3DMAPOBJ* p_sys )
{
	// データを読み込み済みなら破棄する
	if( p_sys->res_load ){
		WFLBY_3DMAPOBJ_ResRelease( p_sys );
	}

	// 全メモリを破棄
	sys_FreeMemoryEz( p_sys->p_float );
	sys_FreeMemoryEz( p_sys->p_obj );
	sys_FreeMemoryEz( p_sys );
}

//----------------------------------------------------------------------------
/**
 *	@brief	マップオブジェシステム	メイン処理
 *
 *	@param	p_sys		ワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_Main( WFLBY_3DMAPOBJ* p_sys )
{
	// アニメメイン
	
	//  マップメイン
	WFLBY_3DMAPOBJ_MAP_Main( &p_sys->map, &p_sys->mapres );
	
	// フロートメイン
	{
		int i;

		for( i=0; i<p_sys->floatnum; i++ ){
//			OS_TPrintf( "float idx=%d ",  i );
			WFLBY_3DMAPOBJ_FLOAT_Main( &p_sys->p_float[i], &p_sys->floatres );
		}
	}

	// 配置オブジェメイン
	{
		int i;

		for( i=0; i<p_sys->objnum; i++ ){
			WFLBY_3DMAPOBJ_WK_Anm( p_sys, &p_sys->p_obj[i] );
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	マップオブジェシステム	描画処理
 *
 *	@param	p_sys		ワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_Draw( WFLBY_3DMAPOBJ* p_sys )
{
	int i;
	GF_ASSERT( p_sys );
	GF_ASSERT( p_sys->p_obj );
	GF_ASSERT( p_sys->p_float );

//	WFLBY_DEBUG_3DMAPOBJ_PRINT_TIME_TICK_INIT;
	
	// マップ描画
	WFLBY_3DMAPOBJ_MAP_Draw( &p_sys->map, &p_sys->mapres );
//	WFLBY_DEBUG_3DMAPOBJ_PRINT_TIME_TICK_PRINT( __LINE__ );
	
	// 配置オブジェクト描画
	for( i=0; i<p_sys->objnum; i++ ){
		WFLBY_3DMAPOBJ_WK_Draw( &p_sys->objres, &p_sys->p_obj[i] );
	}
//	WFLBY_DEBUG_3DMAPOBJ_PRINT_TIME_TICK_PRINT( __LINE__ );
	
	// フロート描画
	for( i=0; i<p_sys->floatnum; i++ ){
//		OS_TPrintf( "float idx=%d ",  i );
		WFLBY_3DMAPOBJ_FLOAT_Draw( &p_sys->p_float[i], &p_sys->floatres );
	}
//	WFLBY_DEBUG_3DMAPOBJ_PRINT_TIME_TICK_PRINT( __LINE__ );
}

//----------------------------------------------------------------------------
/**
 *	@brief	マップオブジェシステム	VBLANK処理
 *
 *	@param	p_sys		ワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_VBlank( WFLBY_3DMAPOBJ* p_sys )
{
	// VBlank関数
}

// 部屋のリソース
//----------------------------------------------------------------------------
/**
 *	@brief	部屋のリソースを読み込む
 *
 *	@param	p_sys		システムワーク
 *	@param	room		部屋タイプ
 *	@param	season		シーズンタイプ
 *	@param	heapID		ヒープID
 *	@param	gheapID		グラフィックヒープID
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_ResLoad( WFLBY_3DMAPOBJ* p_sys, WFLBY_ROOM_TYPE room, WFLBY_SEASON_TYPE season, u32 heapID, u32 gheapID )
{
	ARCHANDLE* p_handle;
	WFLBY_3DMAPOBJ_MDL_DATA* p_data;

	// シーズンと部屋を保存
	p_sys->season	= season;
	p_sys->room		= room;

	// モデル読み込みデータを取得する
	p_data = WFLBY_3DMAPOBJ_MDLRES_DATA_Init( room, season, heapID );
	
	// シーズンの各リソースを読み込む
	p_handle = ArchiveDataHandleOpen( ARC_WFLBY_GRA, heapID );	// ハンドルオープン

	// アロケータを作成
	sys_InitAllocator( &p_sys->allocator, gheapID, 4 );
	
	// マップ
	WFLBY_3DMAPOBJ_MAP_Load( &p_sys->mapres, p_handle, p_data, gheapID, &p_sys->allocator );

	// 配置オブジェクト
	WFLBY_3DMAPOBJ_WK_Load( &p_sys->objres, p_handle, &p_sys->allocator, p_data, gheapID );


	// フロートオブジェクト
	WFLBY_3DMAPOBJ_FLOAT_Load( &p_sys->floatres, p_handle, &p_sys->allocator, p_data, gheapID );

	ArchiveDataHandleClose( p_handle );
	
	// 破棄
	WFLBY_3DMAPOBJ_MDLRES_DATA_Exit( p_data );	

	// マップはすでに登録する
	WFLBY_3DMAPOBJ_MAP_Add( &p_sys->map, &p_sys->mapres );

	p_sys->res_load = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	部屋リソースを破棄する
 *
 *	@param	p_sys	システムワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_ResRelease( WFLBY_3DMAPOBJ* p_sys )
{
	// マップを破棄する
	WFLBY_3DMAPOBJ_MAP_Delete( &p_sys->map, &p_sys->mapres );
	
	// マップワーク
	WFLBY_3DMAPOBJ_MAP_Release( &p_sys->mapres, &p_sys->allocator );
	
	// 配置オブジェワーク
	WFLBY_3DMAPOBJ_WK_Release( &p_sys->objres, &p_sys->allocator );
	
	// フロートワーク
	WFLBY_3DMAPOBJ_FLOAT_Release( &p_sys->floatres, &p_sys->allocator );

	p_sys->res_load = FALSE;
}


// マップワーク操作
//----------------------------------------------------------------------------
/**
 *	@brief	マップのポールを表示
 *
 *	@param	p_sys	システムワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_MAP_OnPoll( WFLBY_3DMAPOBJ* p_sys )
{
	if( p_sys->mapres.anm_load[ WFLBY_3DMAPOBJ_MAP_ANM_POLL ] == TRUE ){
		if( p_sys->map.anm_on[ WFLBY_3DMAPOBJ_MAP_ANM_POLL ] == FALSE ){
			p_sys->map.anm_on[ WFLBY_3DMAPOBJ_MAP_ANM_POLL ] = TRUE;
			D3DOBJ_AddAnm( &p_sys->map.obj[WFLBY_3DMAPOBJ_MAPOBJ_MAP], &p_sys->mapres.anm[WFLBY_3DMAPOBJ_MAP_ANM_POLL] );
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	マップのポールを表示しなくする
 *
 *	@param	p_sys	システムワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_MAP_OffPoll( WFLBY_3DMAPOBJ* p_sys )
{
	if( p_sys->mapres.anm_load[ WFLBY_3DMAPOBJ_MAP_ANM_POLL ] == TRUE ){
		if( p_sys->map.anm_on[ WFLBY_3DMAPOBJ_MAP_ANM_POLL ] == TRUE ){
			D3DOBJ_DelAnm( &p_sys->map.obj[WFLBY_3DMAPOBJ_MAPOBJ_MAP], &p_sys->mapres.anm[WFLBY_3DMAPOBJ_MAP_ANM_POLL] );
			p_sys->map.anm_on[ WFLBY_3DMAPOBJ_MAP_ANM_POLL ] = FALSE;
		}
	}
}


// フロートワーク操作
//----------------------------------------------------------------------------
/**
 *	@brief	フロートワーク読み込み
 *	
 *	@param	p_sys		ワーク
 *	@param	floattype	フロートナンバー
 *	@param	floatcol	フロートカラー
 *	@param	cp_mat		座標
 *
 *	@return	ワーク
 */
//-----------------------------------------------------------------------------
WFLBY_3DMAPOBJ_FLOAT* WFLBY_3DMAPOBJ_FLOAT_Add( WFLBY_3DMAPOBJ* p_sys, WFLBY_3DMAPOBJ_FLOAT_TYPE floattype, WFLBY_3DMAPOBJ_FLOAT_COL floatcol, const VecFx32* cp_mat )
{
	WFLBY_3DMAPOBJ_FLOAT* p_wk;

	GF_ASSERT( floattype <= WFLBY_3DMAPOBJ_FLOAT_NUM );
	GF_ASSERT( floatcol <= WFLBY_3DMAPOBJ_FLOAT_COL_NUM );
	
	// 空いているフロートワークを取得する	
	p_wk = WFLBY_3DMAPOBJ_FLOAT_GetCleanWk( p_sys );


	// 絵をくっつけてあげる
	D3DOBJ_Init( &p_wk->obj, &p_sys->floatres.mdl[ floattype ] );

	// 描画開始
	D3DOBJ_SetDraw( &p_wk->obj, TRUE );

	// 座標を設定
	WFLBY_3DMAPOBJ_FLOAT_SetPos( p_wk, cp_mat );
	{
		VecFx32 mat = {0,0,0};
		WFLBY_3DMAPOBJ_FLOAT_SetOfsPos( p_wk, &mat );
	}

	// モデルナンバーとカラーナンバーを保存
	p_wk->col	= floatcol;
	p_wk->mdlno	= floattype;

	p_wk->on = TRUE;


	// 常にアニメするものをアニメさせておく
	{
		p_wk->anm_on[WFLBY_3DMAPOBJ_FLOAT_ANM_00] = TRUE;
		p_wk->anm_on[WFLBY_3DMAPOBJ_FLOAT_ANM_02] = TRUE;
	}
	
	return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief	フロート破棄
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_FLOAT_Del( WFLBY_3DMAPOBJ_FLOAT* p_wk )
{
	//  描画フラグを落として、クリアする
	D3DOBJ_SetDraw( &p_wk->obj, FALSE );
	
	p_wk->on = FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	フロート座標を設定
 *
 *	@param	p_wk		ワーク
 *	@param	pos			座標
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_FLOAT_SetPos( WFLBY_3DMAPOBJ_FLOAT* p_wk, const VecFx32* cp_mat )
{
	p_wk->mat	= *cp_mat;
	D3DOBJ_SetMatrix( &p_wk->obj, p_wk->mat.x+p_wk->ofs.x, p_wk->mat.y+p_wk->ofs.y, p_wk->mat.z+p_wk->ofs.z );
}

//----------------------------------------------------------------------------
/**
 *	@brief	描画オフセット値を設定する
 *
 *	@param	p_wk		ワーク
 *	@param	cp_mat		座標
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_FLOAT_SetOfsPos( WFLBY_3DMAPOBJ_FLOAT* p_wk, const VecFx32* cp_mat )
{
	p_wk->ofs	= *cp_mat;
	D3DOBJ_SetMatrix( &p_wk->obj, p_wk->mat.x+p_wk->ofs.x, p_wk->mat.y+p_wk->ofs.y, p_wk->mat.z+p_wk->ofs.z );
}

//----------------------------------------------------------------------------
/**
 *	@brief	フロート座標を取得
 *
 *	@param	p_wk	ワーク
 *	@param	p_mat	マトリックス
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_FLOAT_GetPos( const WFLBY_3DMAPOBJ_FLOAT* cp_wk, VecFx32* p_mat )
{
	D3DOBJ_GetMatrix( &cp_wk->obj, &p_mat->x, &p_mat->y, &p_mat->z );
}

//----------------------------------------------------------------------------
/**
 *	@brief	フロート表示のＯＮＯＦＦ
 *
 *	@param	p_wk	ワーク
 *	@param	flag	フラグ
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_FLOAT_SetDraw( WFLBY_3DMAPOBJ_FLOAT* p_wk, BOOL flag )
{
	D3DOBJ_SetDraw( &p_wk->obj, flag );
}

//----------------------------------------------------------------------------
/**
 *	@brief	フロート描画チェック
 *
 *	@param	cp_wk	ワーク
 *
 *	@retval	TRUE	描画中
 *	@retval	FALSE	描画してない
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_3DMAPOBJ_FLOAT_GetDraw( const WFLBY_3DMAPOBJ_FLOAT* cp_wk )
{
	return D3DOBJ_GetDraw( &cp_wk->obj );
}

//----------------------------------------------------------------------------
/**
 *	@brief	Soundアニメ
 *
 *	@param	p_wk	ワーク
 *
 *	@retval	TRUE	再生した
 *	@retval	FALSE	再生中なので再生しない
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_3DMAPOBJ_FLOAT_SetAnmSound( WFLBY_3DMAPOBJ_FLOAT* p_wk )
{
	if( p_wk->anm_on[WFLBY_3DMAPOBJ_FLOAT_ANM_01] == FALSE ){
		p_wk->anm_on[WFLBY_3DMAPOBJ_FLOAT_ANM_01] = TRUE;
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	bodyアニメ
 *
 *	@param	p_wk	ワーク
 *
 *	@retval	TRUE	再生した
 *	@retval	FALSE	再生中なので再生しない
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_3DMAPOBJ_FLOAT_SetAnmBody( WFLBY_3DMAPOBJ_FLOAT* p_wk )
{
	if( p_wk->anm_on[WFLBY_3DMAPOBJ_FLOAT_ANM_03] == FALSE ){
		p_wk->anm_on[WFLBY_3DMAPOBJ_FLOAT_ANM_03] = TRUE;
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	回転角度を設定する
 *
 *	@param	p_wk		ワーク
 *	@param	x			ｘ角度
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_FLOAT_SetRotaX( WFLBY_3DMAPOBJ_FLOAT* p_wk, u16 x )
{
	D3DOBJ_SetRota( &p_wk->obj, x, D3DOBJ_ROTA_WAY_X );
}

//----------------------------------------------------------------------------
/**
 *	@brief	フロート音性アニメ再生チェック
 *
 *	@param	cp_wk	ワーク
 *
 *	@retval	TRUE	音声アニメ	再生中
 *	@retval	FALSE	音声アニメ	再生してない
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_3DMAPOBJ_FLOAT_CheckAnmSound( const WFLBY_3DMAPOBJ_FLOAT* cp_wk )
{
	return cp_wk->anm_on[WFLBY_3DMAPOBJ_FLOAT_ANM_01];
}

//----------------------------------------------------------------------------
/**
 *	@brief	フロート胴体アニメ	再生中
 *
 *	@param	cp_wk	ワーク
 *
 *	@retval	TRUE	胴体アニメ	再生中
 *	@retval	FALSE	胴体アニメ	停止中
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_3DMAPOBJ_FLOAT_CheckAnmBody( const WFLBY_3DMAPOBJ_FLOAT* cp_wk )
{
	return cp_wk->anm_on[WFLBY_3DMAPOBJ_FLOAT_ANM_03];
}



// 配置オブジェ操作
//----------------------------------------------------------------------------
/**
 *	@brief	配置オブジェ作成
 *
 *	@param	p_sys		システム
 *	@param	objtype		オブジェクトNO
 *	@param	gridx		グリッドX
 *	@param	gridy		グリッドY
 *
 *	@return	ワーク
 */
//-----------------------------------------------------------------------------
WFLBY_3DMAPOBJ_WK* WFLBY_3DMAPOBJ_WK_Add( WFLBY_3DMAPOBJ* p_sys, WFLBY_3DMAPOBJ_WK_TYPE objtype, u32 gridx, u32 gridy )
{
	WFLBY_3DMAPOBJ_WK* p_wk;
	int i;
	
	// 空いているワークを取得
	p_wk = WFLBY_3DMAPOBJ_WK_GetCleanWk( p_sys );


	// 選択したグラフィックを設定
	D3DOBJ_Init( &p_wk->obj, &p_sys->objres.mdl[objtype] );

	// アニメオブジェの初期化
	for( i=0; i<WFLBY_3DMAPOBJ_ALL_ANM_MAX; i++ ){
		if( p_sys->objres.p_anm[objtype][i] != NULL ){
			// アニメションオブジェの初期化
			D3DOBJ_AnmLoad_Data( &p_wk->anm[i], &p_sys->objres.mdl[objtype],
					p_sys->objres.p_anm[objtype][i], &p_sys->allocator );
		}
	}
	
	// 描画開始
	D3DOBJ_SetDraw( &p_wk->obj, TRUE );

	// グリッド座標を設定
	{
		WF2DMAP_POS	pos;
		pos.x	= WF2DMAP_GRID2POS(gridx);
		pos.y	= WF2DMAP_GRID2POS(gridy);
		WFLBY_3DMAPOBJ_WK_SetPos( p_wk, pos );
	}

	p_wk->on	= TRUE;
	p_wk->mdlid	= objtype;

	// アニメ設定
	p_wk->wait_def = WFLBY_3DMAPOBJ_WK_ANM_RANDWAIT_WAITDEF;

	// スピード
	p_wk->speed = FX32_ONE;

	// アルファ設定
	p_wk->alpha_flag	= FALSE;
	p_wk->alpha			= 31;
	p_wk->def_alpha		= 31;

	return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief	配置オブジェ破棄
 *
 *	@param	p_sys		システム
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_WK_Del( WFLBY_3DMAPOBJ* p_sys, WFLBY_3DMAPOBJ_WK* p_wk )
{
	int i;
	
	//  描画フラグを落として、クリアする
	D3DOBJ_SetDraw( &p_wk->obj, FALSE );


	// アニメオブジェの破棄
	for( i=0; i<WFLBY_3DMAPOBJ_ALL_ANM_MAX; i++ ){
		if( p_sys->objres.p_anm[p_wk->mdlid][i] != NULL ){
			// アニメションオブジェの初期化
			D3DOBJ_AnmLoad_Data( &p_wk->anm[i], &p_sys->objres.mdl[p_wk->mdlid],
					p_sys->objres.p_anm[p_wk->mdlid][i], &p_sys->allocator );
		}

	}
	
	p_wk->on = FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	オブジェタイプを取得する
 *
 *	@param	cp_wk		ワーク
 *
 *	@return	オブジェタイプ
 */
//-----------------------------------------------------------------------------
WFLBY_3DMAPOBJ_WK_TYPE WFLBY_3DMAPOBJ_WK_GetType( const WFLBY_3DMAPOBJ_WK* cp_wk )
{
	return cp_wk->mdlid;
}

//----------------------------------------------------------------------------
/**
 *	@brief	座標設定
 *	
 *	@param	p_wk	ワーク
 *	@param	pos		座標
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_WK_SetPos( WFLBY_3DMAPOBJ_WK* p_wk, WF2DMAP_POS pos )
{
	VecFx32 matrix;

	WFLBY_3DMATRIX_GetPosVec( &pos, &matrix );
	D3DOBJ_SetMatrix( &p_wk->obj, matrix.x, matrix.y, matrix.z );
}

//----------------------------------------------------------------------------
/**
 *	@brief	座標を取得する
 *
 *	@param	p_wk	ワーク
 *
 *	@return	座標
 */
//-----------------------------------------------------------------------------
WF2DMAP_POS WFLBY_3DMAPOBJ_WK_GetPos( const WFLBY_3DMAPOBJ_WK* cp_wk )
{
	VecFx32		matrix;
	WF2DMAP_POS	pos;

	D3DOBJ_GetMatrix( &cp_wk->obj, &matrix.x, &matrix.y, &matrix.z );
	WFLBY_3DMATRIX_GetVecPos( &matrix, &pos );

	return pos;
}

//----------------------------------------------------------------------------
/**
 *	@brief	アニメ登録
 *
 *	@param	p_sys		オブジェシステム
 *	@param	p_wk		ワーク
 *	@param	anm			アニメ
 *	@param	play		プレイ方式
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_WK_AddAnm( WFLBY_3DMAPOBJ* p_sys, WFLBY_3DMAPOBJ_WK* p_wk, WFLBY_3DMAPOBJ_WK_ANM_TYPE anm, WFLBY_3DMAPOBJ_WK_ANM_PLAY play )
{
	WFLBY_3DMAPOBJ_WK_AddAnmAnmCallBack( p_sys, p_wk, anm, play, NULL );
}

//----------------------------------------------------------------------------
/**
 *	@brief	アニメ再生	終了コールバックつき
 *
 *	@param	p_sys		オブジェシステム
 *	@param	p_wk		ワーク
 *	@param	anm			アニメ
 *	@param	play		プレイ方式
 *	@param	p_callback	アニメ終了コールバック
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_WK_AddAnmAnmCallBack( WFLBY_3DMAPOBJ* p_sys, WFLBY_3DMAPOBJ_WK* p_wk, WFLBY_3DMAPOBJ_WK_ANM_TYPE anm, WFLBY_3DMAPOBJ_WK_ANM_PLAY play, pWFLBY_3DMAPOBJ_WK_AnmCallBack p_callback )
{
	
	// そのアニメがあるかチェック
	GF_ASSERT( anm < WFLBY_3DMAPOBJ_ALL_ANM_MAX );
	GF_ASSERT( play < WFLBY_3DMAPOBJ_WK_ANM_PLAYNUM );
	GF_ASSERT( p_wk->mdlid < WFLBY_3DMAPOBJ_WK_NUM );
	
	if( (p_sys->objres.p_anm[p_wk->mdlid][anm] != NULL) ){
		
		if( p_wk->anm_on[ anm ] == FALSE ){
			//  アニメ設定
			D3DOBJ_AddAnm( &p_wk->obj, &p_wk->anm[anm] );
		}

		p_wk->anm_on[ anm ]		= TRUE;
		p_wk->play[anm]			= play;
		p_wk->p_anmcallback[anm]= p_callback;
		p_wk->wait[ anm ]		= 0;

		switch( play ){
		case WFLBY_3DMAPOBJ_WK_ANM_LOOP:
		case WFLBY_3DMAPOBJ_WK_ANM_NOLOOP:
		case WFLBY_3DMAPOBJ_WK_ANM_NOAUTO:
			p_wk->anm_frame[ anm ]	= 0;
			break;
		case WFLBY_3DMAPOBJ_WK_ANM_BACKLOOP:
		case WFLBY_3DMAPOBJ_WK_ANM_BACKNOLOOP:
			p_wk->anm_frame[ anm ]	= D3DOBJ_AnmGetFrameNum( &p_wk->anm[anm] );
			break;

		case WFLBY_3DMAPOBJ_WK_ANM_RANDWAIT:
			p_wk->anm_frame[ anm ]	= 0;
			p_wk->wait[ anm ] = gf_mtRand() % p_wk->wait_def;
			break;

		case WFLBY_3DMAPOBJ_WK_ANM_RANDSTART:
			p_wk->anm_frame[ anm ]	= 0;
			p_wk->wait[ anm ] = gf_mtRand() % p_wk->wait_def;
			break;
		}

		D3DOBJ_AnmSet( &p_wk->anm[anm], p_wk->anm_frame[anm] );

	}	
}

//----------------------------------------------------------------------------
/**
 *	@brief	アニメ破棄
 *
 *	@param	p_sys		オブジェシステム
 *	@param	p_wk		ワーク
 *	@param	anm			アニメ
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_WK_DelAnm( WFLBY_3DMAPOBJ* p_sys, WFLBY_3DMAPOBJ_WK* p_wk, WFLBY_3DMAPOBJ_WK_ANM_TYPE anm )
{
	// そのアニメがあるかチェック
	GF_ASSERT( anm < WFLBY_3DMAPOBJ_ALL_ANM_MAX );
	GF_ASSERT( p_wk->mdlid < WFLBY_3DMAPOBJ_WK_NUM );

	if( (p_sys->objres.p_anm[p_wk->mdlid][anm] != NULL) && (p_wk->anm_on[anm] == TRUE) ){
		//  アニメをはがす
		D3DOBJ_DelAnm( &p_wk->obj, &p_wk->anm[ anm ] );
		
		p_wk->anm_on[ anm ]		= FALSE;
		p_wk->anm_frame[ anm ]	= 0;
		p_wk->play[ anm ]		= 0;
		p_wk->wait[ anm ]		= 0;
		p_wk->p_anmcallback[anm]= NULL;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	全アニメをOFFする
 *
 *	@param	p_sys		システムワーク
 *	@param	p_wk		配置オブジェワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_WK_DelAnmAll( WFLBY_3DMAPOBJ* p_sys, WFLBY_3DMAPOBJ_WK* p_wk )
{
	int i;

	for( i=0; i<WFLBY_3DMAPOBJ_ALL_ANM_MAX; i++ ){
		if( WFLBY_3DMAPOBJ_WK_GetAnmFlag( p_wk, i ) == TRUE ){
			WFLBY_3DMAPOBJ_WK_DelAnm( p_sys,  p_wk, i );
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	アニメ再生中かチェック
 *
 *	@param	cp_sys		システム
 *	@param	cp_wk		ワーク
 *	@param	anm			アニメ
 *
 *	@retval	TRUE	再生中
 *	@retval	FALSE	再生してない
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_3DMAPOBJ_WK_CheckAnm( const WFLBY_3DMAPOBJ* cp_sys, const WFLBY_3DMAPOBJ_WK* cp_wk, WFLBY_3DMAPOBJ_WK_ANM_TYPE anm )
{
	// そのアニメがあるかチェック
	GF_ASSERT( anm < WFLBY_3DMAPOBJ_ALL_ANM_MAX );
	GF_ASSERT( cp_wk->mdlid < WFLBY_3DMAPOBJ_WK_NUM );
	
	if( cp_sys->objres.p_anm[cp_wk->mdlid][anm] != NULL ){
		return cp_wk->anm_on[ anm ];
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	アニメーションフレームを設定する
 *
 *	@param	p_sys		システム
 *	@param	p_wk		ワーク
 *	@param	anm			アニメタイプ
 *	@param	frame		フレーム数
 *
 *	アニメーションタイプがWFLBY_3DMAPOBJ_WK_ANM_NOAUTOのとき専用
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_WK_SetAnmFrame( WFLBY_3DMAPOBJ* p_sys, WFLBY_3DMAPOBJ_WK* p_wk, WFLBY_3DMAPOBJ_WK_ANM_TYPE anm, fx32 frame )
{
	// そのアニメがあるかチェック
	GF_ASSERT( anm < WFLBY_3DMAPOBJ_ALL_ANM_MAX );
	GF_ASSERT( p_wk->mdlid < WFLBY_3DMAPOBJ_WK_NUM );
	GF_ASSERT( p_sys->objres.p_anm[p_wk->mdlid][anm] != NULL );
	
	if( p_wk->play[ anm ] != WFLBY_3DMAPOBJ_WK_ANM_NOAUTO ){
		return ; 	// オートアニメ中なので操作しない
	}
	p_wk->anm_frame[ anm ]	= frame;
	D3DOBJ_AnmSet( &p_wk->anm[anm], p_wk->anm_frame[anm] );
}

//----------------------------------------------------------------------------
/**
 *	@brief	アニメ設定しているかチェック
 *
 *	@param	cp_wk		ワーク
 *	@param	anm			アニメタイプ
 *
 *	@retval	TRUE	アニメ中
 *	@retval	FALSE	アニメしてない
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_3DMAPOBJ_WK_GetAnmFlag( const WFLBY_3DMAPOBJ_WK* cp_wk, WFLBY_3DMAPOBJ_WK_ANM_TYPE anm )
{
	// そのアニメがあるかチェック
	GF_ASSERT( anm < WFLBY_3DMAPOBJ_ALL_ANM_MAX );
	GF_ASSERT( cp_wk->mdlid < WFLBY_3DMAPOBJ_WK_NUM );
	return cp_wk->anm_on[ anm ];
}

//----------------------------------------------------------------------------
/**
 *	@brief	アニメフレームを取得する
 *
 *	@param	cp_wk		ワーク
 *	@param	anm			アニメタイプ
 *
 *	@return	フレーム数
 */
//-----------------------------------------------------------------------------
fx32 WFLBY_3DMAPOBJ_WK_GetAnmFrame( const WFLBY_3DMAPOBJ_WK* cp_wk, WFLBY_3DMAPOBJ_WK_ANM_TYPE anm )
{
	// そのアニメがあるかチェック
	GF_ASSERT( anm < WFLBY_3DMAPOBJ_ALL_ANM_MAX );
	GF_ASSERT( cp_wk->mdlid < WFLBY_3DMAPOBJ_WK_NUM );
	return cp_wk->anm_frame[ anm ];
}

//----------------------------------------------------------------------------
/**
 *	@brief	ランダムウエイト再生用	ウエイト時間設定
 *
 *	@param	p_sys		システム
 *	@param	p_wk		ワーク
 *	@param	wait		ウエイト定数
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_WK_SetRandWait( WFLBY_3DMAPOBJ_WK* p_wk, u32 wait )
{
	p_wk->wait_def = wait;
}

//----------------------------------------------------------------------------
/**
 *	@brief	描画フラグを設定
 *
 *	@param	p_wk	ワーク
 *	@param	flag	描画フラグ
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_WK_SetDraw( WFLBY_3DMAPOBJ_WK* p_wk, BOOL flag )
{
	D3DOBJ_SetDraw( &p_wk->obj, flag );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ライトフラグを設定する
 *			【そのモデルと同じ形のモデルはすべてこのライト設定になってしまうので
 *			注意が必要です。】
 *
 *	@param	p_sys			システム
 *	@param	p_wk			ワーク
 *	@param	light_flag		ライトフラグ
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_WK_SetLightFlag( WFLBY_3DMAPOBJ* p_sys, WFLBY_3DMAPOBJ_WK* p_wk, u32 light_flag )
{
	// そのアニメがあるかチェック
	GF_ASSERT( p_wk->mdlid < WFLBY_3DMAPOBJ_WK_NUM );

	// フィールドの設定に変更
	NNS_G3dMdlSetMdlLightEnableFlagAll( p_sys->objres.mdl[ p_wk->mdlid ].pModel, light_flag );
}

//----------------------------------------------------------------------------
/**
 *	@brief	アニメスピード設定
 *
 *	@param	p_wk	ワーク
 *	@param	speed	スピード
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_WK_SetAnmSpeed( WFLBY_3DMAPOBJ_WK* p_wk, fx32 speed )
{
	p_wk->speed = speed;
}

//----------------------------------------------------------------------------
/**
 *	@brief	アニメスピードを取得する
 *
 *	@param	cp_wk	ワーク
 *
 *	@return	スピード
 */
//-----------------------------------------------------------------------------
fx32 WFLBY_3DMAPOBJ_WK_GetAnmSpeed( const WFLBY_3DMAPOBJ_WK* cp_wk )
{
	return cp_wk->speed;
}

//----------------------------------------------------------------------------
/**
 *	@brief	アルファ値設定
 *	
 *	@param	p_wk	ワーク
 *	@param	alpha	アルファ
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_WK_SetAlpha( WFLBY_3DMAPOBJ_WK* p_wk, u8 alpha, u8 def_alpha )
{
	p_wk->alpha_flag	= TRUE;
	p_wk->alpha			= alpha;
	p_wk->def_alpha		= def_alpha;
}

//----------------------------------------------------------------------------
/**
 *	@brief	アルファ値のリセット
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_WK_ResetAlpha( WFLBY_3DMAPOBJ_WK* p_wk )
{
	p_wk->alpha_flag	= FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	拡大値を設定
 *
 *	@param	p_wk	ワーク
 *	@param	x		ｘ方向
 *	@param	y		ｙ方向
 *	@param	z		ｚ方向
 */
//-----------------------------------------------------------------------------
void WFLBY_3DMAPOBJ_WK_SetScale( WFLBY_3DMAPOBJ_WK* p_wk, fx32 x, fx32 y, fx32 z )
{
	D3DOBJ_SetScale( &p_wk->obj, x, y, z );
}




//-----------------------------------------------------------------------------
/**
 *				プライベート関数
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	部屋ナンバーSeasonから読み込みデータを取得する
 *
 *	@param	room		部屋ナンバー
 *	@param	season		Seasonナンバー
 *	@param	heapID		ヒープID
 *
 *	@return	読み込みグラフィックデータ
 */	
//-----------------------------------------------------------------------------
static WFLBY_3DMAPOBJ_MDL_DATA* WFLBY_3DMAPOBJ_MDLRES_DATA_Init( WFLBY_ROOM_TYPE room, WFLBY_SEASON_TYPE season, u32 heapID )
{
	void* p_data;
	u32 idx;

	// 部屋インデックス
	idx = (season * WFLBY_ROOM_NUM) + room;

	GF_ASSERT( idx < (WFLBY_SEASON_NUM*WFLBY_ROOM_NUM) );
	
	p_data = ArcUtil_Load( ARC_WFLBY_MAP, 
			NARC_wflby_map_wflby_3dmapobj_data_0_dat+idx,
			FALSE, heapID, ALLOC_BOTTOM);

	return p_data;
}

//----------------------------------------------------------------------------
/**
 *	@brief	読み込みグラフィックデータ取得
 *
 *	@param	p_data	ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_MDLRES_DATA_Exit( WFLBY_3DMAPOBJ_MDL_DATA* p_data )
{
	sys_FreeMemoryEz( p_data );
}


//----------------------------------------------------------------------------
/**
 *	@brief	広場用モデル読み込み
 *
 *	@param	p_mdl		モデルワーク
 *	@param	p_handle	ハンドル
 *	@param	data_idx	データインデックス
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_MDL_Load( D3DOBJ_MDL* p_mdl, ARCHANDLE* p_handle, u32 data_idx, u32 gheapID )
{
	// テクスチャ実部分を破棄したテクスチャリソースを読み込む
	WFLBY_3DMAPOBJ_TEX_LoatCutTex( &p_mdl->pResMdl, p_handle, data_idx, gheapID );
	
	// 本データの各ポインタを取得する
	{
		p_mdl->pModelSet	= NNS_G3dGetMdlSet( p_mdl->pResMdl );
		p_mdl->pModel		= NNS_G3dGetMdlByIdx( p_mdl->pModelSet, 0 );
		p_mdl->pMdlTex		= NNS_G3dGetTex( p_mdl->pResMdl );
	}

	// テクスチャキーを設定して、モデルにバインドする
	{
		BindTexture( p_mdl->pResMdl, p_mdl->pMdlTex );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	広場用モデル破棄
 *
 *	@param	p_mdl		モデルワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_MDL_Delete( D3DOBJ_MDL* p_mdl )
{
	D3DOBJ_MdlDelete( p_mdl );
}

//----------------------------------------------------------------------------
/**
 *	@brief	フレーム管理
 *
 *	@param	p_frame		フレームポインタ
 *	@param	cp_anm		対象アニメーションオブジェ
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_ANM_Loop( fx32* p_frame, const D3DOBJ_ANM* cp_anm, fx32 speed )
{
	fx32 end_frame;

	end_frame = D3DOBJ_AnmGetFrameNum( cp_anm );

	// アニメを進める
	if( ((*p_frame) + speed) < end_frame ){
		(*p_frame) += speed;
	}else{
		(*p_frame) = ((*p_frame) + speed) % end_frame;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	フレーム管理
 *
 *	@param	p_frame		フレームポインタ
 *	@param	cp_anm		対象アニメーションオブジェ
 *
 *	@retval	TRUE	アニメ終了
 *	@retval	FALSE	アニメ途中
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_3DMAPOBJ_ANM_NoLoop( fx32* p_frame, const D3DOBJ_ANM* cp_anm, fx32 speed )
{
	fx32 end_frame;
	BOOL ret;

	end_frame = D3DOBJ_AnmGetFrameNum( cp_anm );

	// アニメを進める
	if( ((*p_frame) + speed) < end_frame ){
		(*p_frame) += speed;
		ret = FALSE;
	}else{
		(*p_frame) =  end_frame-FX32_HALF;
		ret = TRUE;
	}

	return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief	逆再生アニメ
 *	
 *	@param	p_frame		フレーム
 *	@param	cp_anm		アニメオブジェ
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_ANM_BackLoop( fx32* p_frame, const D3DOBJ_ANM* cp_anm, fx32 speed )
{
	fx32 end_frame;

	end_frame = D3DOBJ_AnmGetFrameNum( cp_anm );

	// アニメを進める
	if( ((*p_frame) - speed) >= 0 ){
		(*p_frame) -= speed;
	}else{
		(*p_frame) = ((*p_frame) - speed) + end_frame;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	逆再生ループなしアニメ
 *
 *	@param	p_frame		フレーム
 *	@param	cp_anm		アニメ
 *
 *	@retval	TRUE	アニメ終了
 *	@retval	FALSE	アニメ途中
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_3DMAPOBJ_ANM_BackNoLoop( fx32* p_frame, const D3DOBJ_ANM* cp_anm, fx32 speed )
{
	BOOL ret;

	// アニメを進める
	if( ((*p_frame) - speed) > 0 ){
		(*p_frame) -= speed;
		ret = FALSE;
	}else{
		(*p_frame) =  0;
		ret = TRUE;
	}

	return ret;
}



// マップワーク
//----------------------------------------------------------------------------
/**
 *	@brief	マップ読み込み
 *
 *	@param	p_wk		ワーク
 *	@param	p_handle	ハンドル
 *	@param	cp_data		データ
 *	@param	gheapID		ヒープID
 *	@param	p_allocator	アロケータ
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_MAP_Load( WFLBY_3DMAPOBJ_MAPRES* p_wk, ARCHANDLE* p_handle, const WFLBY_3DMAPOBJ_MDL_DATA* cp_data, u32 gheapID, NNSFndAllocator* p_allocator )
{
	int i, j;
	// モデルリソース読み込み
	for( i=0; i<WFLBY_3DMAPOBJ_MAPOBJ_NUM; i++ ){
//		OS_TPrintf( "map load mdl idx = %d\n",  cp_data->map_idx[i] );
		WFLBY_3DMAPOBJ_MDL_Load( &p_wk->mdl[ i ],
				p_handle, cp_data->map_idx[ i ], gheapID );


		// ライト設定
		WFLBY_LIGHT_SetUpMdl( p_wk->mdl[ i ].pResMdl );	// ライト反映
	}

	// アニメリソースを読み込む
	for( i=0; i<WFLBY_3DMAPOBJ_MAP_ANM_NUM; i++ ){

		// アニメIDXがモデルIDXと違ったら読み込む
		if( cp_data->map_anmidx[i] == cp_data->map_idx[WFLBY_3DMAPOBJ_MAPOBJ_MAP] ){
			p_wk->anm_load[i] = FALSE;
		}else{

			p_wk->anm_load[i] = TRUE;

//			OS_TPrintf( "map load anm idx = %d\n",  cp_data->map_anmidx[i] );

			if( i != WFLBY_3DMAPOBJ_MAP_ANM_MAT ){
			
				D3DOBJ_AnmLoadH( &p_wk->anm[i], &p_wk->mdl[ WFLBY_3DMAPOBJ_MAPOBJ_MAP ],
						p_handle, 
						cp_data->map_anmidx[i], gheapID, p_allocator );
			}else{

				D3DOBJ_AnmLoadH( &p_wk->anm[i], &p_wk->mdl[ WFLBY_3DMAPOBJ_MAPOBJ_MAT ],
						p_handle, 
						cp_data->map_anmidx[i], gheapID, p_allocator );
			}

#if 0
			// 動作させるNODEIDXを指定
			for( j=0; j<cp_data->map_nodenum; j++ ){
				if( cp_data->map_anmnodeidx[i] != j ){
					NNS_G3dAnmObjDisableID( p_wk->anm[i].pAnmObj, j );
				}
			}
#endif
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	モデルリソースの破棄
 *
 *	@param	p_wk	ワーク
 *	@param	p_allocator	アロケータ
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_MAP_Release( WFLBY_3DMAPOBJ_MAPRES* p_wk, NNSFndAllocator* p_allocator )
{
	int i;

	// アニメ破棄
	for( i=0; i<WFLBY_3DMAPOBJ_MAP_ANM_NUM; i++ ){
		if( p_wk->anm_load[i] ){
			D3DOBJ_AnmDelete( &p_wk->anm[i], p_allocator );
			p_wk->anm_load[i] = FALSE;
		}
	}

	//  モデル破棄
	for( i=0; i<WFLBY_3DMAPOBJ_MAPOBJ_NUM; i++ ){
		WFLBY_3DMAPOBJ_MDL_Delete( &p_wk->mdl[i] );	
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	地面オブジェの作成
 *
 *	@param	p_wk		ワーク
 *	@param	p_res		リソース 
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_MAP_Add( WFLBY_3DMAPOBJ_MAP* p_wk, WFLBY_3DMAPOBJ_MAPRES* p_res )
{
	int i;

	p_wk->on = TRUE;
	
	for( i=0; i<WFLBY_3DMAPOBJ_MAPOBJ_NUM; i++ ){
		D3DOBJ_Init( &p_wk->obj[i],  &p_res->mdl[i] );
		// 描画開始
		D3DOBJ_SetDraw( &p_wk->obj[i], TRUE );

		// 半グリッドずらす	
//		D3DOBJ_SetMatrix( &p_wk->obj[i], -WFLBY_3DMATRIX_GRID_GRIDSIZ_HALF, 0, 0 );	
	}

	// POLL以外のアニメを開始
	for( i=0; i<WFLBY_3DMAPOBJ_MAP_ANM_NUM-1; i++ ){

		if( p_res->anm_load[i] == TRUE ){
			p_wk->anm_on[i] = TRUE;

			// シート用マットアニメ以外はMAPにアニメ追加
			if( i != WFLBY_3DMAPOBJ_MAP_ANM_MAT ){

				D3DOBJ_AddAnm( &p_wk->obj[WFLBY_3DMAPOBJ_MAPOBJ_MAP], 
						&p_res->anm[i] );
			}else{

				D3DOBJ_AddAnm( &p_wk->obj[WFLBY_3DMAPOBJ_MAPOBJ_MAT], 
						&p_res->anm[i] );
			}
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	マップオブジェ	破棄
 *
 *	@param	p_wk	ワーク
 *	@param	p_res	リソースワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_MAP_Delete( WFLBY_3DMAPOBJ_MAP* p_wk, WFLBY_3DMAPOBJ_MAPRES* p_res )
{
	int i;

	// 全アニメをはがす
	for( i=0; i<WFLBY_3DMAPOBJ_MAP_ANM_NUM-1; i++ ){

		if( p_wk->anm_on[i] == TRUE ){
			p_wk->anm_on[i] = FALSE;

			// シート用マットアニメ以外はMAPにアニメ追加
			if( i != WFLBY_3DMAPOBJ_MAP_ANM_MAT ){

				D3DOBJ_DelAnm( &p_wk->obj[WFLBY_3DMAPOBJ_MAPOBJ_MAP], 
						&p_res->anm[i] );
			}else{

				D3DOBJ_DelAnm( &p_wk->obj[WFLBY_3DMAPOBJ_MAPOBJ_MAT], 
						&p_res->anm[i] );
			}
		}
	}
	
	
	p_wk->on = FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	マップメイン処理
 *
 *	@param	p_wk	ワーク
 *	@param	p_res	リソース
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_MAP_Main( WFLBY_3DMAPOBJ_MAP* p_wk, WFLBY_3DMAPOBJ_MAPRES* p_res )
{
	int i;

	// アニメが開始していたらフレームを設定する
	for( i=0; i<WFLBY_3DMAPOBJ_MAP_ANM_NUM; i++ ){
		if( p_wk->anm_on[i] ){
			WFLBY_3DMAPOBJ_ANM_Loop( &p_wk->anm_frame[i], &p_res->anm[i], FX32_ONE );
			// フレームをあわせる
			D3DOBJ_AnmSet( &p_res->anm[i], p_wk->anm_frame[i] );
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	マップ描画
 *
 *	@param	p_wk	ワーク
 *	@param	p_res	リソース
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_MAP_Draw( WFLBY_3DMAPOBJ_MAP* p_wk, WFLBY_3DMAPOBJ_MAPRES* p_res )
{
	int i, j;
	if( p_wk->on ){
		for( i=0; i<WFLBY_3DMAPOBJ_MAPOBJ_NUM; i++ ){

			D3DOBJ_Draw( &p_wk->obj[i] );

		}
	}
}


// フロートワーク
//----------------------------------------------------------------------------
/**
 *	@brief	空いているワークを取得する
 *
 *	@param	p_sys		ワーク
 *
 *	@return	空いているワーク
 */
//-----------------------------------------------------------------------------
static WFLBY_3DMAPOBJ_FLOAT* WFLBY_3DMAPOBJ_FLOAT_GetCleanWk( WFLBY_3DMAPOBJ* p_sys )
{
	int i;

	for( i=0; i<p_sys->floatnum; i++ ){
		if( p_sys->p_float[i].on == FALSE ){
			return &p_sys->p_float[i];
		}
	}

	GF_ASSERT(0);
	return NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief	フロートの読み込み
 *	
 *	@param	p_wk		ワーク
 *	@param	p_handle	ハンドル
 *	@param	p_allocator	アロケータ
 *	@param	cp_data		データ
 *	@param	gheapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_FLOAT_Load( WFLBY_3DMAPOBJ_FLOATRES* p_wk, ARCHANDLE* p_handle, NNSFndAllocator* p_allocator, const WFLBY_3DMAPOBJ_MDL_DATA* cp_data, u32 gheapID )
{
	int i, j, k;

	// テクスチャ読み込み
	{
		for( i=0; i<WFLBY_3DMAPOBJ_FLOAT_NUM; i++ ){
			for( j=0; j<WFLBY_3DMAPOBJ_FLOAT_COL_NUM; j++ ){
				WFLBY_3DMAPOBJ_TEX_LoatCutTex( &p_wk->p_texres[i][j], p_handle,
						cp_data->float_texidx[i][j], gheapID );
			}
		}
	}

	// モデル読み込み
	{
		for( i=0; i<WFLBY_3DMAPOBJ_FLOAT_NUM; i++ ){

			// モデルを読み込み
			p_wk->mdl[i].pResMdl	= ArcUtil_HDL_Load( p_handle, 
						cp_data->float_idx[i], FALSE, gheapID, ALLOC_TOP );
			p_wk->mdl[i].pModelSet	= NNS_G3dGetMdlSet( p_wk->mdl[i].pResMdl );
			p_wk->mdl[i].pModel		= NNS_G3dGetMdlByIdx( p_wk->mdl[i].pModelSet, 0 );

			// テクスチャは後でバインドする
			// とりあえず今はアニメ初期用に０のものを入れる
			p_wk->mdl[i].pMdlTex = NNS_G3dGetTex( p_wk->p_texres[i][0] );
		}
	}

	// アニメの読み込み
	{
		for( i=0; i<WFLBY_3DMAPOBJ_FLOAT_NUM; i++ ){
			for( j=0; j<WFLBY_3DMAPOBJ_FLOAT_ANM_NUM; j++ ){
	
				// モデルIDXとアニメIDXが一致したら読み込まない
				if( cp_data->float_idx[0] != cp_data->float_anmidx[i][j] ){

					D3DOBJ_AnmLoadH( &p_wk->anm[i][j], &p_wk->mdl[i], p_handle, 
							cp_data->float_anmidx[i][j], gheapID, p_allocator );

					// 頭と尻尾（か翼）のアニメなら反映先ノードIDを設定
					if( j >= WFLBY_3DMAPOBJ_FLOAT_ANM_01 ){

						// モデルノード数分アニメ設定をOFF
						for( k=0; k<cp_data->float_nodenum[i]; k++ ){
							if( cp_data->float_anm_node[i][j-WFLBY_3DMAPOBJ_FLOAT_ANM_01] != k ){
								NNS_G3dAnmObjDisableID( p_wk->anm[i][j].pAnmObj, k );
							}
						}
					}				
				}
			}
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	フロートリソース破棄
 *
 *	@param	p_wk		ワーク
 *	@param	p_allocator	アロケータ
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_FLOAT_Release( WFLBY_3DMAPOBJ_FLOATRES* p_wk, NNSFndAllocator* p_allocator )
{
	int i, j;

	// アニメの破棄
	{
		for( i=0; i<WFLBY_3DMAPOBJ_FLOAT_NUM; i++ ){
			for( j=0; j<WFLBY_3DMAPOBJ_FLOAT_ANM_NUM; j++ ){
				D3DOBJ_AnmDelete( &p_wk->anm[i][j], p_allocator );
			}
		}
	}

	// モデル破棄
	{
		for( i=0; i<WFLBY_3DMAPOBJ_FLOAT_NUM; i++ ){
			sys_FreeMemoryEz( p_wk->mdl[i].pResMdl );
		}
	}

	// テクスチャ破棄
	{
		NNSG3dTexKey texKey;
		NNSG3dTexKey tex4x4Key;
		NNSG3dPlttKey plttKey;
		NNSG3dResTex* p_tex;


		for( i=0; i<WFLBY_3DMAPOBJ_FLOAT_NUM; i++ ){
			for( j=0; j<WFLBY_3DMAPOBJ_FLOAT_COL_NUM; j++ ){
				p_tex = NNS_G3dGetTex( p_wk->p_texres[i][j] );
				//VramKey破棄
				NNS_G3dTexReleaseTexKey( p_tex, &texKey, &tex4x4Key );
				NNS_GfdFreeTexVram( texKey );	
				NNS_GfdFreeTexVram( tex4x4Key );	

				plttKey = NNS_G3dPlttReleasePlttKey( p_tex );
				NNS_GfdFreePlttVram( plttKey );

				// メモリ破棄
				sys_FreeMemoryEz( p_wk->p_texres[i][j] );
			}
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	フロート　メイン処理
 *
 *	@param	p_wk		ワーク
 *	@param	p_res		リソース
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_FLOAT_Main( WFLBY_3DMAPOBJ_FLOAT* p_wk, WFLBY_3DMAPOBJ_FLOATRES* p_res )
{
	int i;

	// アニメさせる
	{
		fx32 end_frame;
		for( i=0; i<WFLBY_3DMAPOBJ_FLOAT_ANM_NUM; i++ ){
			if( p_wk->anm_on[i] ){
				switch(i){
				case WFLBY_3DMAPOBJ_FLOAT_ANM_00:
				case WFLBY_3DMAPOBJ_FLOAT_ANM_02:		// 頭
//					OS_TPrintf( "anm roop %d=%d  ",  i, p_wk->anm_frame[i] );
					WFLBY_3DMAPOBJ_ANM_Loop( &p_wk->anm_frame[i], &p_res->anm[ p_wk->mdlno ][ i ], FX32_ONE );
					break;
				case WFLBY_3DMAPOBJ_FLOAT_ANM_01:		// 泣き声用
				case WFLBY_3DMAPOBJ_FLOAT_ANM_03:		// 尻尾
					{
						BOOL result;
//						OS_TPrintf( "anm noroop %d=%d  ",  i, p_wk->anm_frame[i] );
						result = WFLBY_3DMAPOBJ_ANM_NoLoop( &p_wk->anm_frame[i], &p_res->anm[ p_wk->mdlno ][ i ], FX32_ONE );
						if( result == TRUE ){
							// アニメ停止
							p_wk->anm_on[i] = FALSE;
							p_wk->anm_frame[i] = 0;
						}
					}
					break;
				default:
					GF_ASSERT(0);
					break;
				}
			}
		}
	}			
//	OS_TPrintf( "\n",  i, p_wk->anm_frame[i] );
}

//----------------------------------------------------------------------------
/**
 *	@brief	フロートの描画処理
 *
 *	@param	p_wk	ワーク
 *	@param	p_res	リソース
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_FLOAT_Draw( WFLBY_3DMAPOBJ_FLOAT* p_wk, WFLBY_3DMAPOBJ_FLOATRES* p_res )
{
	BOOL result;
	int i;
	
	if( p_wk->on == FALSE ){
		return ;
	}

	// FLOAT描画チェック
	if( WFLBY_3DMAPOBJ_MDL_BOXCheck( &p_res->mdl[ p_wk->mdlno ], &p_wk->obj ) == FALSE ){
		return ;
	}

	GF_ASSERT( p_wk->col < WFLBY_3DMAPOBJ_FLOAT_COL_NUM );
	GF_ASSERT( p_wk->mdlno < WFLBY_3DMAPOBJ_FLOAT_NUM );


	// カラーのテクスチャをバインド
	p_res->mdl[ p_wk->mdlno ].pMdlTex = NNS_G3dGetTex( p_res->p_texres[ p_wk->mdlno ][ p_wk->col ] );
	result = NNS_G3dBindMdlSet(p_res->mdl[ p_wk->mdlno ].pModelSet, p_res->mdl[ p_wk->mdlno ].pMdlTex );
	GF_ASSERT( result );

	// アニメの適用
	for( i=0; i<WFLBY_3DMAPOBJ_FLOAT_ANM_NUM; i++ ){
		if( p_wk->anm_on[i] ){
			D3DOBJ_AddAnm( &p_wk->obj, &p_res->anm[ p_wk->mdlno ][ i ] );
			// フレームをあわせる
			D3DOBJ_AnmSet( &p_res->anm[ p_wk->mdlno ][ i ], p_wk->anm_frame[i] );
		}
	}

	D3DOBJ_Draw( &p_wk->obj );

	// アニメを破棄
	for( i=0; i<WFLBY_3DMAPOBJ_FLOAT_ANM_NUM; i++ ){
		if( p_wk->anm_on[i] ){
			D3DOBJ_DelAnm( &p_wk->obj, &p_res->anm[ p_wk->mdlno ][ i ] );
		}
	}

	// バインド解除
	NNS_G3dReleaseMdlSet(p_res->mdl[ p_wk->mdlno ].pModelSet );
}

// 配置オブジェクトワーク
//----------------------------------------------------------------------------
/**
 *	@brief	空いているワークを取得する
 *
 *	@param	p_sys	システムワーク
 *	
 *	@return	空いているワーク
 */
//-----------------------------------------------------------------------------
static WFLBY_3DMAPOBJ_WK* WFLBY_3DMAPOBJ_WK_GetCleanWk( WFLBY_3DMAPOBJ* p_sys )
{
	int i;

	for( i=0; i<p_sys->objnum; i++ ){
		if( p_sys->p_obj[i].on == FALSE ){
			return &p_sys->p_obj[i];
		}
	}

	GF_ASSERT(0);
	return NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief	配置オブジェリソース読み込み
 *
 *	@param	p_wk		ワーク
 *	@param	p_handle	ハンドル
 *	@param	p_allocator	アロケータ
 *	@param	cp_data		データ
 *	@param	gheapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_WK_Load( WFLBY_3DMAPOBJ_WKRES* p_wk, ARCHANDLE* p_handle, NNSFndAllocator* p_allocator, const WFLBY_3DMAPOBJ_MDL_DATA* cp_data, u32 gheapID )
{
	int i, j;

	for( i=0; i<WFLBY_3DMAPOBJ_WK_NUM; i++ ){
//		OS_TPrintf( "wk load mdl idx = %d\n",  cp_data->obj_idx[i] );
		WFLBY_3DMAPOBJ_MDL_Load( &p_wk->mdl[i], p_handle, cp_data->obj_idx[i], gheapID );

		// ライト設定
		if( (i != WFLBY_3DMAPOBJ_LAMP00) && 
			(i != WFLBY_3DMAPOBJ_LAMP01) ){
			WFLBY_LIGHT_SetUpMdl( p_wk->mdl[ i ].pResMdl );	// ライト反映
		}

		// モデル分のアニメを読み込む
		for( j=0; j<WFLBY_3DMAPOBJ_ALL_ANM_MAX; j++ ){
			
			// オブジェのIDと同じなら読み込まない
			if( cp_data->obj_anm[i][j] != cp_data->obj_idx[i] ){
				p_wk->p_anm[i][j] = ArcUtil_HDL_Load( p_handle, cp_data->obj_anm[i][j], FALSE, gheapID, ALLOC_TOP );
//				OS_TPrintf( "wk load anm idx = %d\n",  cp_data->obj_anm[i][j] );
/*				D3DOBJ_AnmLoadH( &p_wk->anm[i][j], &p_wk->mdl[i],
						p_handle, 
						cp_data->obj_anm[i][j], gheapID, p_allocator );//*/
			}else{
				p_wk->p_anm[i][j] = NULL;
			}
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	配置オブジェリソース破棄
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_WK_Release( WFLBY_3DMAPOBJ_WKRES* p_wk, NNSFndAllocator* p_allocator )
{
	int i, j;

	for( i=0; i<WFLBY_3DMAPOBJ_WK_NUM; i++ ){

		// アニメリソースを破棄
		for( j=0; j<WFLBY_3DMAPOBJ_ALL_ANM_MAX; j++ ){
/*			if( p_wk->anm[i][j].pResAnm != NULL ){
				D3DOBJ_AnmDelete( &p_wk->anm[i][j], p_allocator );
				p_wk->anm[i][j].pResAnm = NULL;
			}//*/

			if( p_wk->p_anm[i][j] != NULL ){
				sys_FreeMemoryEz( p_wk->p_anm[i][j] );
				p_wk->p_anm[i][j] = NULL;
			}

		}
		
		WFLBY_3DMAPOBJ_MDL_Delete( &p_wk->mdl[i] );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	オブジェクト描画
 *
 *	@param	p_res	リソースオブジェ
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_WK_Draw( WFLBY_3DMAPOBJ_WKRES* p_res, WFLBY_3DMAPOBJ_WK* p_wk )
{
	int i;

	if( p_wk->on == FALSE ){
		return ;
	}

	WFLBY_DEBUG_3DMAPOBJ_PRINT_TIME_TICK_INIT;
	
	// 描画チェック
	if( WFLBY_3DMAPOBJ_MDL_BOXCheck( &p_res->mdl[ p_wk->mdlid ], &p_wk->obj ) == FALSE ){
		return ;
	}
	WFLBY_DEBUG_3DMAPOBJ_PRINT_TIME_TICK_PRINT( __LINE__ );


	/*
	// アニメiのフレーム設定
	for( i=0; i<WFLBY_3DMAPOBJ_ALL_ANM_MAX; i++ ){
		// アニメがONになっていてRANDWAITのwaitが終わってたらアニメフレーム設定
		if( p_wk->anm_on[i] && (p_wk->wait[i] == 0) ){
			// フレームをあわせる
			D3DOBJ_AnmSet( &p_wk->anm[i], p_wk->anm_frame[i] );
		}
	}
	WFLBY_DEBUG_3DMAPOBJ_PRINT_TIME_TICK_PRINT( __LINE__ );
	//*/

	// アルファ値の適用
	if( p_wk->alpha_flag == TRUE ){
		NNS_G3dMdlSetMdlAlphaAll( p_res->mdl[p_wk->mdlid].pModel, p_wk->alpha );
	}

	D3DOBJ_Draw( &p_wk->obj );
	WFLBY_DEBUG_3DMAPOBJ_PRINT_TIME_TICK_PRINT( __LINE__ );

	// アルファ値を戻す
	if( p_wk->alpha_flag == TRUE ){
		NNS_G3dMdlSetMdlAlphaAll( p_res->mdl[p_wk->mdlid].pModel, p_wk->def_alpha );
	}

}


//----------------------------------------------------------------------------
/**
 *	@brief	起動中のアニメをループ再生
 *
 *	@param	p_sys	システムワーク
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_3DMAPOBJ_WK_Anm( WFLBY_3DMAPOBJ* p_sys, WFLBY_3DMAPOBJ_WK* p_wk )
{
	int i;
	BOOL result;
	pWFLBY_3DMAPOBJ_WK_AnmCallBack p_callback;

	for( i=0; i<WFLBY_3DMAPOBJ_ALL_ANM_MAX; i++ ){
		if( p_wk->anm_on[i] ){
			switch( p_wk->play[i] ){
			// ループ
			case WFLBY_3DMAPOBJ_WK_ANM_LOOP:
				WFLBY_3DMAPOBJ_ANM_Loop( &p_wk->anm_frame[i], &p_wk->anm[i], p_wk->speed );
				D3DOBJ_AnmSet( &p_wk->anm[i], p_wk->anm_frame[i] );
				break;

			// ループなし
			case WFLBY_3DMAPOBJ_WK_ANM_NOLOOP:
				result = WFLBY_3DMAPOBJ_ANM_NoLoop( &p_wk->anm_frame[i], &p_wk->anm[i], p_wk->speed );
				D3DOBJ_AnmSet( &p_wk->anm[i], p_wk->anm_frame[i] );
				if( result == TRUE ){
					
					// 保存
					p_callback = p_wk->p_anmcallback[i];

					//  アニメを破棄
					WFLBY_3DMAPOBJ_WK_DelAnm( p_sys, p_wk, i );

					// コールバックがあったら呼ぶ
					if( p_callback ){
						p_callback( p_sys, p_wk );
					}

				}
				break;

			// 逆再生ループ
			case WFLBY_3DMAPOBJ_WK_ANM_BACKLOOP:
				WFLBY_3DMAPOBJ_ANM_BackLoop( &p_wk->anm_frame[i], &p_wk->anm[i], p_wk->speed );
				D3DOBJ_AnmSet( &p_wk->anm[i], p_wk->anm_frame[i] );
				break;

			// 逆再生ループなし
			case WFLBY_3DMAPOBJ_WK_ANM_BACKNOLOOP:
				result = WFLBY_3DMAPOBJ_ANM_BackNoLoop( &p_wk->anm_frame[i], &p_wk->anm[i], p_wk->speed );
				D3DOBJ_AnmSet( &p_wk->anm[i], p_wk->anm_frame[i] );
				if( result == TRUE ){
					// 保存
					p_callback = p_wk->p_anmcallback[i];

					//  アニメを破棄
					WFLBY_3DMAPOBJ_WK_DelAnm( p_sys, p_wk, i );

					// コールバックがあったら呼ぶ
					if( p_callback ){
						p_callback( p_sys, p_wk );
					}

				}
				break;

			// ランダムウエイト再生	ループ
			case WFLBY_3DMAPOBJ_WK_ANM_RANDWAIT:
				// ウエイトしてから再生
				if( p_wk->wait[i] > 0 ){
					p_wk->wait[i]--;
				}else{
					result = WFLBY_3DMAPOBJ_ANM_NoLoop( &p_wk->anm_frame[i], &p_wk->anm[i], p_wk->speed );
					if( result == TRUE ){
						// ループ再生
						p_wk->wait[i] = gf_mtRand() % p_wk->wait_def;
						p_wk->anm_frame[i] = 0;
					}
					D3DOBJ_AnmSet( &p_wk->anm[i], p_wk->anm_frame[i] );
				}
				break;

			// ランダムスタート再生	ループ
			case WFLBY_3DMAPOBJ_WK_ANM_RANDSTART:
				// ウエイトしてから再生
				if( p_wk->wait[i] > 0 ){
					p_wk->wait[i]--;
				}else{
					WFLBY_3DMAPOBJ_ANM_Loop( &p_wk->anm_frame[i], &p_wk->anm[i], p_wk->speed );
					D3DOBJ_AnmSet( &p_wk->anm[i], p_wk->anm_frame[i] );
				}
				break;

			// なにもしない
			case WFLBY_3DMAPOBJ_WK_ANM_NOAUTO:
			default:
				break;

			}

		}
	}
}
