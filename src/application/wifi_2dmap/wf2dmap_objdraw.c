//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wf2dmap_objdraw.c
 *	@brief		オブジェクトデータ表示モジュール
 *	@author		tomoya takahashi
 *	@data		2007.03.16
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "common.h"

#define __WF2DMAP_OBJDRAW_H_GLOBAL
#include "application/wifi_2dmap/wf2dmap_objdraw.h"

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
// 表示優先順位計算マクロ
#define WF2DMAP_DRAWPRI_STATR	(20000)	// 表示優先順位開始位置
#define WF2DMAP_DRAWPRI_MAX		(20000)	//たてに10000ドットサイズの部屋はないだろう
#define WF2DMAP_DRAWPRI_HERO_GET(y)	( WF2DMAP_DRAWPRI_MAX - (y*2) + WF2DMAP_DRAWPRI_STATR )	// 主人公の表示優先順位
#define WF2DMAP_DRAWPRI_GET(y)	( WF2DMAP_DRAWPRI_HERO_GET(y)+1 )	// NPCの表示優先順位
#define WF2DMAP_DRAWPRI_SHADOW	( 40001 )

// 登録デフォルト設定
#define WF2DMAP_BGPRI_DEF	(2)	// 基本ＢＧ優先順位

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	表示ワーク
//=====================================
typedef struct _WF2DMAP_OBJDRAWWK{
	const WF2DMAP_OBJWK* cp_objwk;	// リンクしているオブジェクトワーク
	WF_2DCWK* p_drawwk;	// 表示ワーク
	u16 status;	// 今の状態
	u16 way;	// 方向
	u16 playid;	// プレイヤーＩＤ
	u16 lastframe;	// 1つ前の終了フレーム
	u16 lastanm;	// 1つ前のアニメ
	u8 hero;	// 主人公フラグ
	u8 updata;	// アップデートフラグ
}WF2DMAP_OBJDRAWWK;


//-------------------------------------
///	表示システムワーク
//=====================================
typedef struct _WF2DMAP_OBJDRAWSYS{
	WF_2DCSYS* p_drawsys;
	WF2DMAP_OBJDRAWWK* p_wk;
	u16 objnum;
	u8	bg_pri;
	u8	draw_type;
}WF2DMAP_OBJDRAWSYS;

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
static BOOL WF2DMAP_OBJDrawWkCleanCheck( const WF2DMAP_OBJDRAWWK* cp_wk );
static WF2DMAP_OBJDRAWWK* WF2DMAP_OBJDrawSysCleanWkGet( WF2DMAP_OBJDRAWSYS* p_sys );
static WF_2DC_ANMTYPE WF2DMAP_OBJDrawWF2DMAP_OBJSTtoWF_2DC_ANMTYPE( const WF_2DCWK* p_drawwk, WF2DMAP_OBJST status );
static void WF2DMAP_OBJDrawWkFramePosGet( const WF2DMAP_OBJDRAWWK* cp_wk, s16* p_x, s16* p_y );

static void WF2DMAP_OBJDrawWkAnmSet( WF2DMAP_OBJDRAWWK* p_wk, WF_2DC_ANMTYPE anmtype, WF_COMMON_WAY anmway );

//----------------------------------------------------------------------------
/**
 *	@brief	オブジェクト描画システム初期化
 *	
 *	@param	p_clset			セルアクターポインタ
 *	@param	p_pfd			パレットフェードポインタ
 *	@param	objnum			オブジェクト数
 *	@param	draw_type		全人物の表示先
 *	@param	heapID			ヒープＩＤ
 *
 *	@return	システムワーク
 *
 *
 *	draw_type
	mainに登録：NNS_G2D_VRAM_TYPE_2DMAIN
	subに登録 ：NNS_G2D_VRAM_TYPE_2DSUB
	両方に登録：NNS_G2D_VRAM_TYPE_2DMAX
 */
//-----------------------------------------------------------------------------
WF2DMAP_OBJDRAWSYS* WF2DMAP_OBJDrawSysInit( CLACT_SET_PTR p_clset, PALETTE_FADE_PTR p_pfd, u32 objnum, u32 draw_type, u32 heapID )
{
	WF2DMAP_OBJDRAWSYS* p_sys;

	// システムワーク作成
	p_sys = sys_AllocMemory( heapID, sizeof(WF2DMAP_OBJDRAWSYS) );

	// 表示システム作成
	p_sys->p_drawsys = WF_2DC_SysInit( p_clset, p_pfd, objnum, heapID );

	// オブジェクトワーク作成
	p_sys->objnum = objnum;
	p_sys->p_wk = sys_AllocMemory( heapID, sizeof(WF2DMAP_OBJDRAWWK)*p_sys->objnum );
	memset( p_sys->p_wk, 0, sizeof(WF2DMAP_OBJDRAWWK)*p_sys->objnum );

	// 基本設定
	p_sys->bg_pri		= WF2DMAP_BGPRI_DEF;
	p_sys->draw_type	= draw_type;

	return p_sys;
}

//----------------------------------------------------------------------------
/**
 *	@brief	オブジェクト描画システム初期化		陰あり
 *	
 *	@param	p_clset			セルアクターポインタ
 *	@param	p_pfd			パレットフェードポインタ
 *	@param	objnum			オブジェクト数
 *	@param	hero_charid		主人公キャラクタID
 *	@param	hero_movetype	主人公の動作
 *	@param	draw_type		全人物の表示先
 *	@param	heapID			ヒープＩＤ
 *
 *	@return	システムワーク
 *
 *
 *	draw_type
	mainに登録：NNS_G2D_VRAM_TYPE_2DMAIN
	subに登録 ：NNS_G2D_VRAM_TYPE_2DSUB
	両方に登録：NNS_G2D_VRAM_TYPE_2DMAX
 */
//-----------------------------------------------------------------------------
WF2DMAP_OBJDRAWSYS* WF2DMAP_OBJDrawSysInit_Shadow( CLACT_SET_PTR p_clset, PALETTE_FADE_PTR p_pfd, u32 objnum, u32 hero_charid, WF_2DC_MOVETYPE hero_movetype,  u32 draw_type,u32 heapID )
{
	WF2DMAP_OBJDRAWSYS* p_sys;

	// システムワーク作成
	p_sys = sys_AllocMemory( heapID, sizeof(WF2DMAP_OBJDRAWSYS) );

	// 表示システム作成
	p_sys->p_drawsys = WF_2DC_SysInit( p_clset, p_pfd, objnum, heapID );

	// オブジェクトワーク作成
	p_sys->objnum = objnum;
	p_sys->p_wk = sys_AllocMemory( heapID, sizeof(WF2DMAP_OBJDRAWWK)*p_sys->objnum );
	memset( p_sys->p_wk, 0, sizeof(WF2DMAP_OBJDRAWWK)*p_sys->objnum );

	// 基本設定
	p_sys->bg_pri		= WF2DMAP_BGPRI_DEF;
	p_sys->draw_type	= draw_type;

	// 主人公のリソースを読み込む
	WF_2DC_ResSet( p_sys->p_drawsys, hero_charid, p_sys->draw_type, hero_movetype, heapID );

	// 陰のリソース読み込み
	WF_2DC_ShadowResSet( p_sys->p_drawsys, p_sys->draw_type, WF2DMAP_DRAWPRI_SHADOW, heapID );

	return p_sys;
}

//----------------------------------------------------------------------------
/**
 *	@brief	オブジェクト描画システム破棄
 *
 *	@param	p_sys 
 */
//-----------------------------------------------------------------------------
void WF2DMAP_OBJDrawSysExit( WF2DMAP_OBJDRAWSYS* p_sys )
{
	int i;
	
	// オブジェクトワーク破棄
	for( i=0; i<p_sys->objnum; i++ ){
		if( WF2DMAP_OBJDrawWkCleanCheck( &p_sys->p_wk[i] ) == FALSE ){
			WF2DMAP_OBJDrawWkDel( &p_sys->p_wk[i] );
		}
	}


	// 陰のリソース破棄
	WF_2DC_ShadowResDel( p_sys->p_drawsys );

	// リソースはき
	WF2DMAP_OBJDrawSysAllResDel( p_sys );

	// 表示システムはき
	WF_2DC_SysExit( p_sys->p_drawsys );
	
	// ワークバッファ破棄
	sys_FreeMemoryEz( p_sys->p_wk );
	sys_FreeMemoryEz( p_sys );
}

//----------------------------------------------------------------------------
/**
 *	@brief	BG優先順位の設定
 *
 *	@param	p_sys	オブジェクト表示システムワーク
 *	@param	bg_pri	ＢＧ優先順位
 */
//-----------------------------------------------------------------------------
void WF2DMAP_OBJDrawSysDefBgPriSet( WF2DMAP_OBJDRAWSYS* p_sys, u32 bg_pri )
{
	p_sys->bg_pri = bg_pri;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ＢＧ優先順位の取得
 *
 *	@param	cp_sys	オブジェクト表示システムワーク
 *
 *	@return	ＢＧ優先順位
 */
//-----------------------------------------------------------------------------
u32 WF2DMAP_OBJDrawSysDefBgPriGet( const WF2DMAP_OBJDRAWSYS* cp_sys )
{
	return cp_sys->bg_pri;
}


//----------------------------------------------------------------------------
/**
 *	@brief	キャラクタリソース設定
 *
 *	@param	p_sys		システムワーク	
 *	@param	charaid		キャラクタＩＤ
 *	@param	movetype	動作タイプ
 *	@param	heapID		ヒープＩＤ

	movetype
	WF_2DC_MOVERUN,		// 歩き＋振り向き＋走りアニメ（主人公のみ）
	WF_2DC_MOVENORMAL,	// 歩き＋振り向きのみアニメ
	WF_2DC_MOVETURN,	// 振り向きのみアニメ
 */
//-----------------------------------------------------------------------------
void WF2DMAP_OBJDrawSysResSet( WF2DMAP_OBJDRAWSYS* p_sys, u32 charaid, WF_2DC_MOVETYPE movetype, u32 heapID )
{
	// 多重読み込み回避
	if( WF_2DC_ResCheck( p_sys->p_drawsys, charaid ) == FALSE ){
		WF_2DC_ResSet( p_sys->p_drawsys, charaid, p_sys->draw_type, movetype, heapID );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	キャラクターリソースの破棄
 *
 *	@param	p_sys		システムワーク
 *	@param	charid		キャラクタータイプ
 */
//-----------------------------------------------------------------------------
void WF2DMAP_OBJDrawSysResDel( WF2DMAP_OBJDRAWSYS* p_sys, u32 charaid )
{
	WF_2DC_ResDel( p_sys->p_drawsys, charaid );
}

//----------------------------------------------------------------------------
/**
 *	@brief	キャラクターリソースが読み込まれているかチェック
 *
 *	@param	cp_sys		システムワーク
 *	@param	charid		キャラクタＩＤ
 *
 *	@retval	読み込んである
 *	@retval	読み込んでない
 */
//-----------------------------------------------------------------------------
BOOL WF2DMAP_OBJDrawSysResCheck( const WF2DMAP_OBJDRAWSYS* cp_sys, u32 charid )
{
	return WF_2DC_ResCheck( cp_sys->p_drawsys, charid );
}

//----------------------------------------------------------------------------
/**
 *	@brief	全リソースの破棄
 *	
 *	@param	p_sys	システムワーク
 */
//-----------------------------------------------------------------------------
void WF2DMAP_OBJDrawSysAllResDel( WF2DMAP_OBJDRAWSYS* p_sys )
{
	WF_2DC_AllResDel( p_sys->p_drawsys );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ユニオンキャラクタリソース登録
 *
 *	@param	p_sys		システムワーク
 *	@param	movetype	動作タイプ
 *	@param	heap		ヒープID
 *

	movetype
	WF_2DC_MOVERUN,		// 歩き＋振り向き＋走りアニメ（主人公のみ）
	WF_2DC_MOVENORMAL,	// 歩き＋振り向きのみアニメ
	WF_2DC_MOVETURN,	// 振り向きのみアニメ
 */
//-----------------------------------------------------------------------------
void WF2DMAP_OBJDrawSysUniResSet( WF2DMAP_OBJDRAWSYS* p_sys, WF_2DC_MOVETYPE movetype, u32 heap )
{
	WF_2DC_UnionResSet( p_sys->p_drawsys, p_sys->draw_type, movetype, heap );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ユニオンキャラクタリソース破棄
 *
 *	@param	p_sys		システムワーク
 */
//-----------------------------------------------------------------------------
void WF2DMAP_OBJDrawSysUniResDel( WF2DMAP_OBJDRAWSYS* p_sys )
{
	WF_2DC_UnionResDel( p_sys->p_drawsys );
}

//----------------------------------------------------------------------------
/**
 *	@brief	表示オブジェクトの生成
 *
 *	@param	p_sys		システムワーク
 *	@param	cp_objwk	オブジェクトワーク
 *	@param	hero		主人公フラグ	（このオブジェクトが自分:TRUE　他人:FALSE）
 *	@param	heapID		ヒープＩＤ
 *
 *	@return	表示オブジェクトワーク
 */
//-----------------------------------------------------------------------------
WF2DMAP_OBJDRAWWK* WF2DMAP_OBJDrawWkNew( WF2DMAP_OBJDRAWSYS* p_sys, const WF2DMAP_OBJWK* cp_objwk, BOOL hero, u32 heapID )
{
	WF2DMAP_OBJDRAWWK* p_wk;
	WF_2DC_WKDATA add;
	s32 charid;

	p_wk = WF2DMAP_OBJDrawSysCleanWkGet( p_sys );
	
	p_wk->status = WF2DMAP_OBJWkDataGet( cp_objwk, WF2DMAP_OBJPM_ST );
	p_wk->way	 = WF2DMAP_OBJWkDataGet( cp_objwk, WF2DMAP_OBJPM_WAY );
	p_wk->playid = WF2DMAP_OBJWkDataGet( cp_objwk, WF2DMAP_OBJPM_PLID );
	p_wk->cp_objwk = cp_objwk;	// オブジェクトワーク保存
	p_wk->hero = hero;
	p_wk->updata = TRUE;
	p_wk->lastframe = 0;
	p_wk->lastanm = 0;

	// セルアクターデータ作成
	add.x = WF2DMAP_OBJWkDataGet( cp_objwk, WF2DMAP_OBJPM_X );
	add.y = WF2DMAP_OBJWkDataGet( cp_objwk, WF2DMAP_OBJPM_Y );
	add.pri = WF2DMAP_OBJDrawWkDrawPriCalc( add.y, p_wk->hero );
	add.bgpri = p_sys->bg_pri;

	// アクター登録
	charid = WF2DMAP_OBJWkDataGet( cp_objwk, WF2DMAP_OBJPM_CHARA );
	p_wk->p_drawwk = WF_2DC_WkAdd( p_sys->p_drawsys, &add, charid, heapID );

	// 一度APDATA
	WF2DMAP_OBJDrawWkUpdata( p_wk );

	return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief	表示オブジェクトの破棄
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
void WF2DMAP_OBJDrawWkDel( WF2DMAP_OBJDRAWWK* p_wk )
{
	// ワーク破棄
	WF_2DC_WkDel( p_wk->p_drawwk );
	memset( p_wk, 0, sizeof(WF2DMAP_OBJDRAWWK) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	オブジェクトデータ更新
 *
 *	@param	p_sys		オブジェクト表示管理システム
 */
//-----------------------------------------------------------------------------
void WF2DMAP_OBJDrawSysUpdata( WF2DMAP_OBJDRAWSYS* p_sys )
{
	int i;

	for( i=0; i<p_sys->objnum; i++ ){
		// データが入っていたら更新
		if( WF2DMAP_OBJDrawWkCleanCheck(&p_sys->p_wk[i]) == FALSE ){
			WF2DMAP_OBJDrawWkUpdata( &p_sys->p_wk[i] );
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	オブジェクトデータ更新	個々に
 *
 *	@param	p_wk		オブジェクト表示ワーク
 */
//-----------------------------------------------------------------------------
void WF2DMAP_OBJDrawWkUpdata( WF2DMAP_OBJDRAWWK* p_wk )
{
	s32 status;
	s32 way;
	s16 set_x, set_y;
	s16 frame;
	u16 pri;
	WF_2DC_ANMTYPE anmtype;

	// 更新フラグチェック
	if( p_wk->updata == FALSE ){
		return;
	}

	status = WF2DMAP_OBJWkDataGet( p_wk->cp_objwk, WF2DMAP_OBJPM_ST );
	way = WF2DMAP_OBJWkDataGet( p_wk->cp_objwk, WF2DMAP_OBJPM_WAY );
	frame = WF2DMAP_OBJWkDataGet( p_wk->cp_objwk, WF2DMAP_OBJPM_FRAME );
	
	// データ更新
	if( (p_wk->status != status) || (p_wk->way != way) || (frame == 0) ){
		p_wk->status = status;
		p_wk->way = way;
		// アニメデータ変更
		anmtype = WF2DMAP_OBJDrawWF2DMAP_OBJSTtoWF_2DC_ANMTYPE( p_wk->p_drawwk, status );
		WF2DMAP_OBJDrawWkAnmSet( p_wk, anmtype, way );
	}

	// 座標を求める
	WF2DMAP_OBJDrawWkFramePosGet( p_wk, &set_x, &set_y );
	WF_2DC_WkMatrixSet( p_wk->p_drawwk, set_x, set_y );
	pri = WF2DMAP_OBJDrawWkDrawPriCalc( set_y, p_wk->hero );
	WF_2DC_WkDrawPriSet( p_wk->p_drawwk, pri );
	
	// アニメーションを進める
	WF_2DC_WkPatAnmAddFrame( p_wk->p_drawwk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	アップデートフラグの設定
 *
 *	@param	p_wk		ワーク
 *	@param	flag		ON/OFFフラグ
 */
//-----------------------------------------------------------------------------
void WF2DMAP_OBJDrawWkUpdataFlagSet( WF2DMAP_OBJDRAWWK* p_wk, BOOL flag )
{
	p_wk->updata = flag;
}

//----------------------------------------------------------------------------
/**
 *	@brief	アップデートフラグの取得
 *
 *	@param	cp_wk		ワーク
 *
 *	@return	フラグ
 */
//-----------------------------------------------------------------------------
BOOL WF2DMAP_OBJDrawWkUpdataFlagGet( const WF2DMAP_OBJDRAWWK* cp_wk )
{ 
	return cp_wk->updata;
}

//----------------------------------------------------------------------------
/**
 *	@brief	回転アニメの開始
 *
 *	@param	p_wk	ワーク
 *
 *	＊回転アニメ終了を実行するまで回り続ける
 */
//-----------------------------------------------------------------------------
void WF2DMAP_OBJDrawWkKuruAnimeStart( WF2DMAP_OBJDRAWWK* p_wk )
{
	WF2DMAP_OBJDrawWkAnmSet( p_wk, WF_2DC_ANMROTA, p_wk->way );
}

//----------------------------------------------------------------------------
/**
 *	@brief	回転アニメメイン
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
void WF2DMAP_OBJDrawWkKuruAnimeMain( WF2DMAP_OBJDRAWWK* p_wk )
{
	// アニメーションを進める
	WF_2DC_WkPatAnmAddFrame( p_wk->p_drawwk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	回転アニメ終了
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
void WF2DMAP_OBJDrawWkKuruAnimeEnd( WF2DMAP_OBJDRAWWK* p_wk )
{
	WF_2DC_ANMTYPE anmtype;

	// アニメデータ変更
	anmtype = WF2DMAP_OBJDrawWF2DMAP_OBJSTtoWF_2DC_ANMTYPE( p_wk->p_drawwk, p_wk->status );
	WF2DMAP_OBJDrawWkAnmSet( p_wk, anmtype, p_wk->way );

	// １回アップデート
	WF2DMAP_OBJDrawWkUpdata( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	描画座標の取得
 *
 *	@param	cp_wk		ワーク
 *
 *	@return	座標
 */
//-----------------------------------------------------------------------------
WF2DMAP_POS WF2DMAP_OBJDrawWkMatrixGet( const WF2DMAP_OBJDRAWWK* cp_wk )
{
	WF2DMAP_POS pos;

	pos.x = WF_2DC_WkMatrixGet( cp_wk->p_drawwk, WF_2DC_GET_X );
	pos.y = WF_2DC_WkMatrixGet( cp_wk->p_drawwk, WF_2DC_GET_Y );

	return pos;
}

//----------------------------------------------------------------------------
/**
 *	@brief	描画座標の設定
 *
 *	@param	p_wk		ワーク
 *	@param	pos			座標
 */
//-----------------------------------------------------------------------------
void WF2DMAP_OBJDrawWkMatrixSet( WF2DMAP_OBJDRAWWK* p_wk, WF2DMAP_POS pos )
{
	WF_2DC_WkMatrixSet( p_wk->p_drawwk, pos.x, pos.y );
}

//----------------------------------------------------------------------------
/**
 *	@brief	表示非表示を取得
 *
 *	@param	cp_wk	ワーク
 *	
 *	@retval	TRUE	表示
 *	@retval	FALSE	非表示
 */
//-----------------------------------------------------------------------------
BOOL WF2DMAP_OBJDrawWkDrawFlagGet( const WF2DMAP_OBJDRAWWK* cp_wk )
{
	return WF_2DC_WkDrawFlagGet( cp_wk->p_drawwk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	表示非表示設定
 *
 *	@param	p_wk		ワーク
 *	@param	flag		フラグ
 */
//-----------------------------------------------------------------------------
void WF2DMAP_OBJDrawWkDrawFlagSet( WF2DMAP_OBJDRAWWK* p_wk, BOOL flag )
{
	WF_2DC_WkDrawFlagSet( p_wk->p_drawwk, flag );
}

//----------------------------------------------------------------------------
/**
 *	@brief	表示優先順位の取得
 *
 *	@param	cp_wk		ワーク
 *
 *	@return	表示優先順位
 */
//-----------------------------------------------------------------------------
u16 WF2DMAP_OBJDrawWkDrawPriGet( const WF2DMAP_OBJDRAWWK* cp_wk )
{
	return WF_2DC_WkDrawPriGet( cp_wk->p_drawwk );
}

//----------------------------------------------------------------------------
/**
 *	@brieif	停止状態の絵の方向設定
 *
 *	@param	p_wk		ワーク
 *	@param	way			方向
 *
 *	*歩いていたりしてもとまっている絵の方向を出します。
 */
//-----------------------------------------------------------------------------
void WF2DMAP_OBJDrawWkWaySet( WF2DMAP_OBJDRAWWK* p_wk, WF2DMAP_WAY way )
{
	WF2DMAP_OBJDrawWkAnmSet( p_wk, WF_2DC_ANMWAY, way );
	WF_2DC_WkPatAnmAddFrame( p_wk->p_drawwk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	表示優先順位取得
 *
 *	@param	y		ｙ座標
 *	@param	hero	主人公　可否
 *
 *	@return	表示優先順位
 */
//-----------------------------------------------------------------------------
u32 WF2DMAP_OBJDrawWkDrawPriCalc( s16 y, BOOL hero )
{
	if( hero == TRUE ){
		return WF2DMAP_DRAWPRI_HERO_GET(y);
	}
	return WF2DMAP_DRAWPRI_GET(y);
}

//----------------------------------------------------------------------------
/**
 *	@brief	参照パレットナンバーを取得
 *
 *	@param	cp_wk	ワーク
 *
 *	@return	今参照しているパレット位置
 */
//-----------------------------------------------------------------------------
u32 WF2DMAP_OBJDrawWkPaletteNoGet( const WF2DMAP_OBJDRAWWK* cp_wk )
{
	return CLACT_PaletteOffsetGet( WF_2DC_WkConstClWkGet( cp_wk->p_drawwk ) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	陰の位置を設定する
 *	
 *	@param	p_wk	ワーク
 *	@param	pos		陰位置
 */
//-----------------------------------------------------------------------------
void WF2DMAP_OBJDrawWkShadowMatrixSet( WF2DMAP_OBJDRAWWK* p_wk, WF2DMAP_POS pos )
{
	WF_2DC_WkShadowMatrixSet( p_wk->p_drawwk, pos.x, pos.y );
}



//-----------------------------------------------------------------------------
/**
 *			プライベート関数
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	オブジェワークが空いているかチェック
 *	
 *	@param	cp_wk		ワーク
 *
 *	@retval	TRUE	空いている
 *	@retval	FALSE	空いていない
 */
//-----------------------------------------------------------------------------
static BOOL WF2DMAP_OBJDrawWkCleanCheck( const WF2DMAP_OBJDRAWWK* cp_wk )
{
	if( cp_wk->p_drawwk == NULL ){
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	空のオブジェクトワークを取得
 *
 *	@param	p_sys		オブジェクト表示システム
 *
 *	@return	空のオブジェクトワーク
 */
//-----------------------------------------------------------------------------
static WF2DMAP_OBJDRAWWK* WF2DMAP_OBJDrawSysCleanWkGet( WF2DMAP_OBJDRAWSYS* p_sys )
{
	int i;

	for( i=0; i<p_sys->objnum; i++ ){
		if( WF2DMAP_OBJDrawWkCleanCheck( &p_sys->p_wk[i] ) == TRUE ){
			return &p_sys->p_wk[i];
		}
	}

	GF_ASSERT(0);
	return NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ステータスをアニメーションタイプにコンバートする
 *
 *	@param	p_drawwk	wifi_2dchar
 *	@param	status		状態
 *
 *	@return	アニメーションタイプ
 */
//-----------------------------------------------------------------------------
static WF_2DC_ANMTYPE WF2DMAP_OBJDrawWF2DMAP_OBJSTtoWF_2DC_ANMTYPE( const WF_2DCWK* p_drawwk, WF2DMAP_OBJST status )
{
	static const u8 WF2DMAP_OBJSTtoWF_2DC_ANMTYPE[ WF2DMAP_OBJST_NUM ] = {
		WF_2DC_ANMWAY,
		WF_2DC_ANMTURN,
		WF_2DC_ANMWALK,
		WF_2DC_ANMRUN,
		WF_2DC_ANMWAY,
		WF_2DC_ANMWALLWALK,
		WF_2DC_ANMSLOWWALK,
		WF_2DC_ANMHIGHWALK4,
		WF_2DC_ANMHIGHWALK2,
		WF_2DC_ANMHIGHWALK4,
		WF_2DC_ANMWALK,
		WF_2DC_ANMSLOWWALK,
	};
	WF_2DC_MOVETYPE movetype;

	// 動作タイプ取得
	movetype = WF_2DC_WkMoveTypeGet( p_drawwk );
	
	// 動作タイプが振り向きのみならどんなときでも振り向きしかできない
	if( movetype == WF_2DC_MOVETURN ){
		return WF_2DC_ANMWAY;
	}

	return WF2DMAP_OBJSTtoWF_2DC_ANMTYPE[ status ];
}

//----------------------------------------------------------------------------
/**
 *	@brief	フレーム数から座標を取得する
 *
 *	@param	cp_wk		ワーク
 *	@param	p_x			座標設定先
 *	@param	p_y			座標設定先
 */
//-----------------------------------------------------------------------------
static void WF2DMAP_OBJDrawWkFramePosGet( const WF2DMAP_OBJDRAWWK* cp_wk, s16* p_x, s16* p_y )
{
	WF2DMAP_POS pos;

	pos = WF2DMAP_OBJWkFrameMatrixGet( cp_wk->cp_objwk );

	*p_x = pos.x;
	*p_y = pos.y;
}

//----------------------------------------------------------------------------
/**
 *	@brief	アニメーションを設定
 *
 *	@param	p_wk		ワーク		
 *	@param	anmtype		アニメタイプ
 *	@param	anmway		アニメ方向
 */
//-----------------------------------------------------------------------------
static void WF2DMAP_OBJDrawWkAnmSet( WF2DMAP_OBJDRAWWK* p_wk, WF_2DC_ANMTYPE anmtype, WF_COMMON_WAY anmway )
{
	u16 frame;
	u16 lastanmtype;
	static const u8 WF2DMAP_OBJFRAME_NEXT[ WF_2DC_ANMNUM ] = {
		FALSE,		// 向き変え
		FALSE,		// 回転
		TRUE,		// 歩き
		FALSE,		// 振り向き
		TRUE,		// 走り
		TRUE,		// 壁歩き
		TRUE,		// ゆっくり歩き
		TRUE,		// 高速歩き2フレーム
		TRUE,		// 高速歩き3フレーム
		FALSE,		// 何もなし
	};

	// 描画フレームを取得
	frame = WF_2DC_WkAnmFrameGet( p_wk->p_drawwk ); 
	lastanmtype = WF_2DC_WkAnmTypeGet( p_wk->p_drawwk );

	// アニメーション変更
	WF_2DC_WkPatAnmStart( p_wk->p_drawwk, anmtype, anmway );

	// 今のアニメと設定したアニメが違うとき
	// もし歩き、走りなら以前のフレームから開始させる
	if( lastanmtype != anmtype ){
		
		// アニメーションフレーム引継ぎなのかチェック
		if( WF2DMAP_OBJFRAME_NEXT[ anmtype ] == TRUE ){
			// １つ前アニメタイプと同じならフレームをそれからにする
			if( p_wk->lastanm == anmtype ){
//				TOMOYA_PRINT( "anmtype %d anmframe %d\n", anmtype, p_wk->lastframe );
				WF_2DC_WkAnmFrameSet( p_wk->p_drawwk, p_wk->lastframe );
			}
		}

		// 保存データ更新
		if( WF2DMAP_OBJFRAME_NEXT[ lastanmtype ] == TRUE ){
			p_wk->lastanm = lastanmtype;
			p_wk->lastframe = frame;
		}
	}
}
