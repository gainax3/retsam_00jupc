//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wf2dmap_obj.c
 *	@brief		オブジェクトデータ管理
 *	@author		tomoya takahashi
 *	@data		2007.03.14
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "common.h"

#define __WF2DMAP_OBJ_H_GLOBAL
#include "application/wifi_2dmap/wf2dmap_obj.h"

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
//#define DEBUG_STCHECK	// 動作途中からコマンドリクエストされていないかチェックする

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
///	オブジェクトワーク
//=====================================
typedef struct _WF2DMAP_OBJWK {
	BOOL move;				// 動作フラグ
	WF2DMAP_POS pos;		// 現在座標
	WF2DMAP_POS last_pos;	// 前座標
	u16 playid;				// プレイヤーID
	u16 charaid;			// キャラクタID
	u16 frame;				// 動作フレーム
	u16 endframe;			// 終了フレーム
	u8	way;				// 方向
	u8	status;				// 状態
	u16 st_frame;			// この状態になっているフレーム数
}WF2DMAP_OBJWK;

//-------------------------------------
///	オブジェクトデータ管理システム
//=====================================
typedef struct _WF2DMAP_OBJSYS {
	WF2DMAP_OBJWK* p_buff;
	u32 num;
}WF2DMAP_OBJSYS;

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
static WF2DMAP_OBJWK* WF2DMAP_OBJSysCleanWkGet( WF2DMAP_OBJSYS* p_sys );




static void WF2DMAP_OBJSysWkMoveInit( WF2DMAP_OBJWK* p_wk, const WF2DMAP_ACTCMD* cp_cmd );
static void WF2DMAP_OBJSysWkMoveMain( WF2DMAP_OBJWK* p_wk );


static void WF2DMAP_OBJSysWkNoneInit( WF2DMAP_OBJWK* p_wk, WF2DMAP_POS pos, WF2DMAP_WAY way );
static void WF2DMAP_OBJSysWkTurnInit( WF2DMAP_OBJWK* p_wk, WF2DMAP_POS pos, WF2DMAP_WAY way );
static void WF2DMAP_OBJSysWkWalkInit( WF2DMAP_OBJWK* p_wk, WF2DMAP_POS pos, WF2DMAP_WAY way );
static void WF2DMAP_OBJSysWkRunInit( WF2DMAP_OBJWK* p_wk, WF2DMAP_POS pos, WF2DMAP_WAY way );
static void WF2DMAP_OBJSysWkBusyInit( WF2DMAP_OBJWK* p_wk, WF2DMAP_POS pos, WF2DMAP_WAY way );
static void WF2DMAP_OBJSysWkWallWalkInit( WF2DMAP_OBJWK* p_wk, WF2DMAP_POS pos, WF2DMAP_WAY way );
static void WF2DMAP_OBJSysWkSlowWalkInit( WF2DMAP_OBJWK* p_wk, WF2DMAP_POS pos, WF2DMAP_WAY way );
static void WF2DMAP_OBJSysWkWalk4Init( WF2DMAP_OBJWK* p_wk, WF2DMAP_POS pos, WF2DMAP_WAY way );
static void WF2DMAP_OBJSysWkStayWalk2Init( WF2DMAP_OBJWK* p_wk, WF2DMAP_POS pos, WF2DMAP_WAY way );
static void WF2DMAP_OBJSysWkStayWalk4Init( WF2DMAP_OBJWK* p_wk, WF2DMAP_POS pos, WF2DMAP_WAY way );
static void WF2DMAP_OBJSysWkStayWalk8Init( WF2DMAP_OBJWK* p_wk, WF2DMAP_POS pos, WF2DMAP_WAY way );
static void WF2DMAP_OBJSysWkStayWalk16Init( WF2DMAP_OBJWK* p_wk, WF2DMAP_POS pos, WF2DMAP_WAY way );

static BOOL WF2DMAP_OBJSysWkNoneMain( WF2DMAP_OBJWK* p_wk );
static BOOL WF2DMAP_OBJSysWkTurnMain( WF2DMAP_OBJWK* p_wk );
static BOOL WF2DMAP_OBJSysWkWalkMain( WF2DMAP_OBJWK* p_wk );
static BOOL WF2DMAP_OBJSysWkRunMain( WF2DMAP_OBJWK* p_wk );
static BOOL WF2DMAP_OBJSysWkBusyMain( WF2DMAP_OBJWK* p_wk );
static BOOL WF2DMAP_OBJSysWkWallWalkMain( WF2DMAP_OBJWK* p_wk );
static BOOL WF2DMAP_OBJSysWkStayWalk2Main( WF2DMAP_OBJWK* p_wk );

static void WF2DMAP_OBJSysWkMoveEnd( WF2DMAP_OBJWK* p_wk );

static WF2DMAP_OBJST WF2DMAP_OBJSysActionCmdLinkOBJStGet( WF2DMAP_CMD cmd );
static WF2DMAP_POS WF2DMAP_OBJSysWayPosGet( WF2DMAP_POS pos, WF2DMAP_WAY way );
static BOOL WF2DMAP_OBJSysFrameCont( WF2DMAP_OBJWK* p_wk );


//----------------------------------------------------------------------------
/**
 *	@brief	オブジェクトデータ管理システム初期化
 *
 *	@param	objnum	オブジェクト数
 *	@param	heapID	ヒープID
 *
 *	@return	オブジェクトデータ管理システム
 */
//-----------------------------------------------------------------------------
WF2DMAP_OBJSYS* WF2DMAP_OBJSysInit( u32 objnum, u32 heapID )
{
	WF2DMAP_OBJSYS* p_sys;

	p_sys = sys_AllocMemory( heapID, sizeof(WF2DMAP_OBJSYS) );
	GF_ASSERT( p_sys );

	p_sys->num = objnum;

	p_sys->p_buff = sys_AllocMemory( heapID, sizeof(WF2DMAP_OBJWK)*p_sys->num );
	GF_ASSERT( p_sys->p_buff );
	memset( p_sys->p_buff, 0, sizeof(WF2DMAP_OBJWK)*p_sys->num );

	return p_sys;
}

//----------------------------------------------------------------------------
/**
 *	@brief	オブジェクトデータ管理システム破棄
 *
 *	@param	p_obj	オブジェクトデータ管理システム
 */
//-----------------------------------------------------------------------------
void WF2DMAP_OBJSysExit( WF2DMAP_OBJSYS* p_sys )
{
	GF_ASSERT( p_sys );
	sys_FreeMemoryEz( p_sys->p_buff );
	sys_FreeMemoryEz( p_sys );
}

//----------------------------------------------------------------------------
/**
 *	@brief	アクションコマンドの設定
 *
 *	@param	p_objsys	オブジェクトデータ管理システム
 *	@param	cp_cmd		設定アクションコマンド
 * 
 */
//-----------------------------------------------------------------------------
void WF2DMAP_OBJSysCmdSet( WF2DMAP_OBJSYS* p_objsys, const WF2DMAP_ACTCMD* cp_cmd )
{
	WF2DMAP_OBJWK* p_wk;
	
	// コマンド実行
	p_wk = WF2DMAP_OBJWkGet( p_objsys, cp_cmd->playid );
	WF2DMAP_OBJSysWkMoveInit( p_wk, cp_cmd );
}

//----------------------------------------------------------------------------
/**
 *	@brief	オブジェクト管理	コマンド実行
 *
 *	@param	p_objsys		オブジェクト管理ワーク
 *	
 */
//-----------------------------------------------------------------------------
void WF2DMAP_OBJSysMain( WF2DMAP_OBJSYS* p_objsys )
{
	int i;
	WF2DMAP_OBJWK* p_wk;
	u32 objnum;

	objnum = WF2DMAP_OBJSysObjNumGet( p_objsys );
	
	// 動作実行
	for( i=0; i<objnum; i++ ){
		p_wk = WF2DMAP_OBJWkIdxGet( p_objsys, i );	//buffインデックスでワーク取得
		if( p_wk ){
			WF2DMAP_OBJSysWkMoveMain( p_wk );
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	オブジェクトデータ管理システム　管理オブジェクト数取得
 *
 *	@param	cp_sys	オブジェクト管理システム
 *
 *	@return	オブジェクトの数
 */
//-----------------------------------------------------------------------------
u32 WF2DMAP_OBJSysObjNumGet( const WF2DMAP_OBJSYS* cp_sys )
{
	GF_ASSERT( cp_sys );
	return cp_sys->num;
}

//----------------------------------------------------------------------------
/**
 *	@brief	オブジェクトデータ生成
 *
 *	@param	p_obj		オブジェクト管理
 *	@param	cp_data 
 *
 *	@return	オブジェクトワーク
 */
//-----------------------------------------------------------------------------
WF2DMAP_OBJWK* WF2DMAP_OBJWkNew( WF2DMAP_OBJSYS* p_sys, const WF2DMAP_OBJDATA* cp_data )
{
	WF2DMAP_OBJWK* p_wk;
	GF_ASSERT( p_sys );
	GF_ASSERT( cp_data );
	
	// 空きを探す
	p_wk = WF2DMAP_OBJSysCleanWkGet( p_sys );
	
	// データ設定
	p_wk->pos.x = cp_data->x;		// x座標
	p_wk->pos.y = cp_data->y;		// y座標
	p_wk->last_pos.x = cp_data->x;		// 前のx座標
	p_wk->last_pos.y = cp_data->y;		// 前のy座標
	p_wk->playid = cp_data->playid;	// プレイヤーID
	p_wk->status = cp_data->status;	// 状態
	p_wk->way = cp_data->way;		// 方向
	p_wk->charaid = cp_data->charaid;// キャラクター

	// 動作開始
	p_wk->move = TRUE;

	return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief	オブジェクトワーク破棄
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
void WF2DMAP_OBJWkDel( WF2DMAP_OBJWK* p_wk )
{
	memset( p_wk, 0, sizeof(WF2DMAP_OBJWK) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	オブジェクトワーク取得
 *
 *	@param	p_sys		オブジェクトデータ管理システム
 *	@param	playid		プレイヤーID
 *	
 *	@retval	オブジェクトワーク
 *	@retval	NULL	そのオブジェクトはいない
 */
//-----------------------------------------------------------------------------
WF2DMAP_OBJWK* WF2DMAP_OBJWkGet( WF2DMAP_OBJSYS* p_sys, u16 playid )
{
	int i;
	
	for( i=0; i<p_sys->num; i++ ){
		if( p_sys->p_buff[i].move == TRUE ){

			if( p_sys->p_buff[i].playid == playid ){
				return &p_sys->p_buff[i];
			}
		}
	}
	return NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief	オブジェクトワークをインデックスで取得する
 *
 *	@param	p_sys		オブジェクトデータ管理システム
 *	@param	idx			インデックス
 *
 *	@retval	オブジェクトワーク
 *	pretval	NULL	動作ちゅうでない
 */
//-----------------------------------------------------------------------------
WF2DMAP_OBJWK* WF2DMAP_OBJWkIdxGet( WF2DMAP_OBJSYS* p_sys, u16 idx )
{
	GF_ASSERT( p_sys );
	GF_ASSERT( idx < p_sys->num );

	if( p_sys->p_buff[ idx ].move == TRUE ){
		return &p_sys->p_buff[ idx ];
	}
	return NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief	コンストワークの取得
 *
 *	@param	cp_sys		オブジェクトデータ管理システム
 *	@param	playid		プレイヤーID
 *
 *	@return	コンストワーク
 */
//-----------------------------------------------------------------------------
const WF2DMAP_OBJWK* WF2DMAP_OBJWkConstGet( const WF2DMAP_OBJSYS* cp_sys, u16 playid )
{
	// やってることは一緒なので再利用する
	return WF2DMAP_OBJWkGet( (WF2DMAP_OBJSYS*)cp_sys, playid );
}

//----------------------------------------------------------------------------
/**
 *	@brief	オブジェクトワークをインデックスで取得する
 *
 *	@param	p_sys		オブジェクトデータ管理システム
 *	@param	idx			インデックス
 *
 *	@retval	オブジェクトワーク
 *	pretval	NULL	動作ちゅうでない
 */
//-----------------------------------------------------------------------------
const WF2DMAP_OBJWK* WF2DMAP_OBJWkConstIdxGet( const WF2DMAP_OBJSYS* cp_sys, u16 idx )
{
	// やってることは一緒なので再利用する
	return WF2DMAP_OBJWkIdxGet( (WF2DMAP_OBJSYS*)cp_sys, idx );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ワークのデータ取得
 *
 *	@param	cp_wk		ワーク
 *	@param	id			取得したデータ定数
 *
 *	@return	指定したデータ
 */
//-----------------------------------------------------------------------------
s32 WF2DMAP_OBJWkDataGet( const WF2DMAP_OBJWK* cp_wk, WF2DMAP_OBJPARAM id )
{
	s32 data;

	GF_ASSERT( cp_wk );
	
	switch( id ){
	case WF2DMAP_OBJPM_X:		// x座標
		data = cp_wk->pos.x;
		break;
		
	case WF2DMAP_OBJPM_Y:		// y座標
		data = cp_wk->pos.y;
		break;
		
	case WF2DMAP_OBJPM_LX:		// 前のx座標
		data = cp_wk->last_pos.x;
		break;
		
	case WF2DMAP_OBJPM_LY:		// 前のy座標
		data = cp_wk->last_pos.y;
		break;
		
	case WF2DMAP_OBJPM_PLID:		// プレイヤーID
		data = cp_wk->playid;
		break;
		
	case WF2DMAP_OBJPM_ST:		// 状態
		data = cp_wk->status;
		break;
		
	case WF2DMAP_OBJPM_WAY:		// 方向
		data = cp_wk->way;
		break;
		
	case WF2DMAP_OBJPM_CHARA:	// キャラクター
		data = cp_wk->charaid;
		break;
		
	case WF2DMAP_OBJPM_FRAME:	// 動作フレーム
		data = cp_wk->frame;
		break;
		
	case WF2DMAP_OBJPM_ENDFRAME:	// 終了動作フレーム
		data = cp_wk->endframe;
		break;

	case WF2DMAP_OBJPM_STFRAME:
		data = cp_wk->st_frame;	// 今の状態になって何フレームたっているか
		break;
		
	default:
		// そんな定数ない
		GF_ASSERT(0);
		break;
	}
	return data;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ワークのデータ取得
 *
 *	@param	p_wk	ワーク
 *	@param	id		設定するデータ定数
 *	@param	data	データ
 */
//-----------------------------------------------------------------------------
void WF2DMAP_OBJWkDataSet( WF2DMAP_OBJWK* p_wk, WF2DMAP_OBJPARAM id, s32 data )
{
	GF_ASSERT( p_wk );
	switch( id ){
	case WF2DMAP_OBJPM_X:		// x座標
		p_wk->pos.x = data;
		break;
		
	case WF2DMAP_OBJPM_Y:		// y座標
		p_wk->pos.y = data;
		break;
		
	case WF2DMAP_OBJPM_LX:		// 前のx座標
		p_wk->last_pos.x = data;
		break;
		
	case WF2DMAP_OBJPM_LY:		// 前のy座標
		p_wk->last_pos.y = data;
		break;
		
	case WF2DMAP_OBJPM_PLID:		// プレイヤーID
		p_wk->playid = data;
		break;
		
	case WF2DMAP_OBJPM_ST:		// 状態
		GF_ASSERT( data < WF2DMAP_OBJST_NUM );
		p_wk->status = data;
		break;
		
	case WF2DMAP_OBJPM_WAY:		// 方向
		GF_ASSERT( data < WF2DMAP_WAY_NUM );
		p_wk->way = data;
		break;
		
	case WF2DMAP_OBJPM_CHARA:	// キャラクター
		p_wk->charaid = data;
		break;
		
	case WF2DMAP_OBJPM_FRAME:	// 動作フレーム
		p_wk->frame = data;
		break;
		
	case WF2DMAP_OBJPM_ENDFRAME:	// 終了動作フレーム
		p_wk->endframe = data;
		break;

	case WF2DMAP_OBJPM_STFRAME:	// 今の状態になって何フレームたっているか
		p_wk->st_frame = data;
		break;
		
	default:
		// そんな定数ない
		GF_ASSERT(0);
		break;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	ワークのデータ加算
 *
 *	@param	p_wk	ワーク
 *	@param	id		加算するデータ定数
 *	@param	data	加算データ
 */
//-----------------------------------------------------------------------------
void WF2DMAP_OBJWkDataAdd( WF2DMAP_OBJWK* p_wk, WF2DMAP_OBJPARAM id, s32 data )
{
	GF_ASSERT( p_wk );
	switch( id ){
	case WF2DMAP_OBJPM_X:		// x座標
		p_wk->pos.x += data;
		break;
		
	case WF2DMAP_OBJPM_Y:		// y座標
		p_wk->pos.y += data;
		break;
		
	case WF2DMAP_OBJPM_LX:		// 前のx座標
		p_wk->last_pos.x += data;
		break;
		
	case WF2DMAP_OBJPM_LY:		// 前のy座標
		p_wk->last_pos.y += data;
		break;
		
	case WF2DMAP_OBJPM_PLID:		// プレイヤーID
		p_wk->playid += data;
		break;
		
	case WF2DMAP_OBJPM_ST:		// 状態
		GF_ASSERT( data < WF2DMAP_OBJST_NUM );
		p_wk->status += data;
		break;
		
	case WF2DMAP_OBJPM_WAY:		// 方向
		GF_ASSERT( data < WF2DMAP_WAY_NUM );
		p_wk->way += data;
		break;
		
	case WF2DMAP_OBJPM_CHARA:	// キャラクター
		p_wk->charaid += data;
		break;
		
	case WF2DMAP_OBJPM_FRAME:	// 動作フレーム
		p_wk->frame += data;
		break;
		
	case WF2DMAP_OBJPM_ENDFRAME:	// 終了動作フレーム
		p_wk->endframe += data;
		break;
		
	default:
		// そんな定数ない
		GF_ASSERT(0);
		break;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	wayの方向に１つ進んだ座標を取得する
 *
 *	@param	pos		座標
 *	@param	way		方向
 *
 *	@return	wayの方向に１つ進んだ座標
 */
//-----------------------------------------------------------------------------
WF2DMAP_POS WF2DMAP_OBJToolWayPosGet( WF2DMAP_POS pos, WF2DMAP_WAY way )
{
	static const s8 add[WF2DMAP_WAY_NUM][2] = {
		{ 0, -WF2DMAP_GRID_SIZ },
		{ 0, WF2DMAP_GRID_SIZ },
		{ -WF2DMAP_GRID_SIZ, 0 },
		{ WF2DMAP_GRID_SIZ, 0 },
	};

	pos.x += add[way][0];
	pos.y += add[way][1];

	return pos;
}

//----------------------------------------------------------------------------
/**
 *	@brief	方向の逆を取得
 *
 *	@param	way		方向
 *
 *	@return	wayの逆方向
 */
//-----------------------------------------------------------------------------
WF2DMAP_WAY WF2DMPA_OBJToolRetWayGet( WF2DMAP_WAY way )
{
	static const u8 retway[ WF2DMAP_WAY_NUM ] = {
		WF2DMAP_WAY_DOWN,
		WF2DMAP_WAY_UP,
		WF2DMAP_WAY_RIGHT,
		WF2DMAP_WAY_LEFT,
	};
	return retway[ way ];
} 




//-----------------------------------------------------------------------------
/**
 *		プライベート関数
 */
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief	空のワークを取得する
 *
 *	@param	p_sys	オブジェクトデータ管理ワーク
 *
 *	@return	空のワーク
 */
//-----------------------------------------------------------------------------
static WF2DMAP_OBJWK* WF2DMAP_OBJSysCleanWkGet( WF2DMAP_OBJSYS* p_sys )
{
	int i;

	for( i=0; i<p_sys->num; i++ ){
		if( p_sys->p_buff[i].move == FALSE ){
			return &p_sys->p_buff[i];
		}
	}
	// 空きなし
	GF_ASSERT(0);
	return NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief	座標の取得
 *
 *	@param	cp_wk	ワーク
 *
 *	@return	座標
 */
//-----------------------------------------------------------------------------
WF2DMAP_POS WF2DMAP_OBJWkMatrixGet( const WF2DMAP_OBJWK* cp_wk )
{
	return cp_wk->pos;
}

//----------------------------------------------------------------------------
/**
 *	@brief	前座標の取得
 *
 *	@param	cp_wk	ワーク
 *
 *	@return	前座標
 */
//-----------------------------------------------------------------------------
WF2DMAP_POS WF2DMAP_OBJWkLastMatrixGet( const WF2DMAP_OBJWK* cp_wk )
{
	return cp_wk->last_pos;
}

//----------------------------------------------------------------------------
/**
 *	@brief	フレームに対応した座標を取得
 *
 *	@param	cp_wk	ワーク
 *
 *	@return	フレーム数に対応した座標
 */
//-----------------------------------------------------------------------------
WF2DMAP_POS WF2DMAP_OBJWkFrameMatrixGet( const WF2DMAP_OBJWK* cp_wk )
{
	s32 frame;
	s32 framemax;
	WF2DMAP_POS  pos, last_pos;
	
	frame = WF2DMAP_OBJWkDataGet( cp_wk, WF2DMAP_OBJPM_FRAME );
	framemax = WF2DMAP_OBJWkDataGet( cp_wk, WF2DMAP_OBJPM_ENDFRAME );

	pos = WF2DMAP_OBJWkMatrixGet( cp_wk );
	last_pos = WF2DMAP_OBJWkLastMatrixGet( cp_wk );

	// 差を出す
	pos.x -= last_pos.x;
	pos.y -= last_pos.y;

	// フレーム数から今の座標を出す
	if( frame > 0 ){
		pos.x = (frame * pos.x) / framemax;
		pos.y = (frame * pos.y) / framemax;
	}else{
		// 0のときは計算が不安定なので手計算
		pos.x = 0;
		pos.y = 0;
	}

	// 前の座標を足す
	pos.x += last_pos.x;
	pos.y += last_pos.y;

	return pos;
}

//----------------------------------------------------------------------------
/**
 *	@brief	座標の設定
 *
 *	@param	p_wk	ワーク
 *	@param	pos		座標
 */
//-----------------------------------------------------------------------------
void WF2DMAP_OBJWkMatrixSet( WF2DMAP_OBJWK* p_wk, WF2DMAP_POS pos )
{
	p_wk->pos = pos;
}

//----------------------------------------------------------------------------
/**
 *	@brief	前座標の設定
 *
 *	@param	p_wk	ワーク
 *	@param	pos		前座標
 */
//-----------------------------------------------------------------------------
void WF2DMAP_OBJWkLastMatrixSet( WF2DMAP_OBJWK* p_wk, WF2DMAP_POS pos )
{
	p_wk->last_pos = pos;
}


//----------------------------------------------------------------------------
/**
 *	@brief	当たり判定を行う
 *
 *	@param	cp_wk		調べるワーク
 *	@param	cp_objsys	オブジェクトシステム
 *	@param	way			方向
 *
 *	@retval	あたったオブジェクト
 *	@retval	NULL	あたらなかった
 */
//-----------------------------------------------------------------------------
const WF2DMAP_OBJWK* WF2DMAP_OBJSysHitCheck( const WF2DMAP_OBJWK* cp_wk, const WF2DMAP_OBJSYS* cp_objsys, WF2DMAP_WAY way )
{
	int i;
	u32 objnum;
	WF2DMAP_POS pos, ck_pos;
	u32 my_playid, ck_playid;
	const WF2DMAP_OBJWK* cp_ckwk;

	// 進んだ先の座標を取得
	pos = WF2DMAP_OBJWkMatrixGet( cp_wk );
	pos = WF2DMAP_OBJToolWayPosGet( pos, way );

	// 自分のプレイヤーＩＤ取得
	my_playid = WF2DMAP_OBJWkDataGet( cp_wk, WF2DMAP_OBJPM_PLID );

	// 管理オブジェクト数取得
	objnum = WF2DMAP_OBJSysObjNumGet( cp_objsys );
	for(i=0; i<objnum; i++){
		// 当たり判定チェックするワーク取得
		cp_ckwk = WF2DMAP_OBJWkConstIdxGet( cp_objsys, i );

		// データが入っているかチェック
		if( cp_ckwk == NULL ){
			continue;
		}
		
		// プレイヤーＩＤ取得
		ck_playid = WF2DMAP_OBJWkDataGet( cp_ckwk, WF2DMAP_OBJPM_PLID );

		// プレイヤーＩＤがかぶらないオブジェクトとだけチェックする
		if( ck_playid != my_playid ){

			// 現在座標
			ck_pos = WF2DMAP_OBJWkMatrixGet( cp_ckwk );
			if( (ck_pos.x == pos.x) && (ck_pos.y == pos.y) ){
				return cp_ckwk;	// あたった！
			}

			// 前座標
			ck_pos = WF2DMAP_OBJWkLastMatrixGet( cp_ckwk );
			if( (ck_pos.x == pos.x) && (ck_pos.y == pos.y) ){
				return cp_ckwk;	// あたった！
			}
		}
	}

	// あたらない・・・
	return NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief	その位置にオブジェクトがいるかチェック
 *
 *	@param	cp_objsys		オブジェクトシステム
 *	@param	pos				座標
 */
//-----------------------------------------------------------------------------
const WF2DMAP_OBJWK* WF2DMAP_OBJSysPosHitCheck( const WF2DMAP_OBJSYS* cp_objsys, WF2DMAP_POS pos )
{
	int i;
	u32 objnum;
	WF2DMAP_POS ck_pos;
	const WF2DMAP_OBJWK* cp_ckwk;

	// 管理オブジェクト数取得
	objnum = WF2DMAP_OBJSysObjNumGet( cp_objsys );
	for(i=0; i<objnum; i++){
		// 当たり判定チェックするワーク取得
		cp_ckwk = WF2DMAP_OBJWkConstIdxGet( cp_objsys, i );

		if( cp_ckwk == NULL ){
			continue;
		}

		// 現在座標
		ck_pos = WF2DMAP_OBJWkMatrixGet( cp_ckwk );
		if( (ck_pos.x == pos.x) && (ck_pos.y == pos.y) ){
			return cp_ckwk;	// あたった！
		}

		// 前座標
		ck_pos = WF2DMAP_OBJWkLastMatrixGet( cp_ckwk );
		if( (ck_pos.x == pos.x) && (ck_pos.y == pos.y) ){
			return cp_ckwk;	// あたった！
		}
	}

	// あたらない・・・
	return NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief	オブジェクト動作初期化
 *
 *	@param	p_wk		オブジェクトワーク
 *	@param	cp_cmd		アクションコマンド
 */
//-----------------------------------------------------------------------------
static void WF2DMAP_OBJSysWkMoveInit( WF2DMAP_OBJWK* p_wk, const WF2DMAP_ACTCMD* cp_cmd )
{
	int st;
	static void (* const pInit[WF2DMAP_CMD_NUM])( WF2DMAP_OBJWK* p_wk, WF2DMAP_POS pos, WF2DMAP_WAY way ) = {
		WF2DMAP_OBJSysWkNoneInit,
		WF2DMAP_OBJSysWkTurnInit,
		WF2DMAP_OBJSysWkWalkInit,
		WF2DMAP_OBJSysWkRunInit,
		WF2DMAP_OBJSysWkBusyInit,
		WF2DMAP_OBJSysWkWallWalkInit,
		WF2DMAP_OBJSysWkSlowWalkInit,
		WF2DMAP_OBJSysWkWalk4Init,
		WF2DMAP_OBJSysWkStayWalk2Init,
		WF2DMAP_OBJSysWkStayWalk4Init,
		WF2DMAP_OBJSysWkStayWalk8Init,
		WF2DMAP_OBJSysWkStayWalk16Init,
	};
	
	pInit[cp_cmd->cmd]( p_wk, cp_cmd->pos, cp_cmd->way );
	p_wk->st_frame = 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief	オブジェクト動作メイン
 *
 *	@param	p_wk		オブジェクトワーク
 */
//-----------------------------------------------------------------------------
static void WF2DMAP_OBJSysWkMoveMain( WF2DMAP_OBJWK* p_wk )
{
	static BOOL (* const pMain[WF2DMAP_OBJST_NUM])( WF2DMAP_OBJWK* p_wk ) = {
		WF2DMAP_OBJSysWkNoneMain,
		WF2DMAP_OBJSysWkTurnMain,
		WF2DMAP_OBJSysWkWalkMain,
		WF2DMAP_OBJSysWkRunMain,
		WF2DMAP_OBJSysWkBusyMain,
		WF2DMAP_OBJSysWkWallWalkMain,
		WF2DMAP_OBJSysWkWalkMain,
		WF2DMAP_OBJSysWkWalkMain,
		WF2DMAP_OBJSysWkStayWalk2Main,
		WF2DMAP_OBJSysWkStayWalk2Main,
		WF2DMAP_OBJSysWkStayWalk2Main,
		WF2DMAP_OBJSysWkStayWalk2Main,
	};
	u32 st = WF2DMAP_OBJWkDataGet( p_wk, WF2DMAP_OBJPM_ST );
	BOOL result;
	
	result = pMain[st]( p_wk );
	p_wk->st_frame ++;

	if( result == TRUE ){
		// 中では待機状態に遷移させている
		WF2DMAP_OBJSysWkMoveEnd( p_wk );
		p_wk->st_frame = 0;
	}
}



//----------------------------------------------------------------------------
/**
 *	@brief	待機状態設定
 *
 *	@param	p_wk		オブジェクトワーク
 *	@param	pos			座標
 *	@param	way			方向
 */
//-----------------------------------------------------------------------------
static void WF2DMAP_OBJSysWkNoneInit( WF2DMAP_OBJWK* p_wk, WF2DMAP_POS pos, WF2DMAP_WAY way )
{
	GF_ASSERT( p_wk );

	// 座標を設定
	WF2DMAP_OBJWkMatrixSet( p_wk, pos );
	WF2DMAP_OBJWkLastMatrixSet( p_wk, pos );

	// 方向を設定
	WF2DMAP_OBJWkDataSet( p_wk, WF2DMAP_OBJPM_WAY, way );

	// 状態を設定
	WF2DMAP_OBJWkDataSet( p_wk, WF2DMAP_OBJPM_ST, WF2DMAP_OBJST_NONE );
}

//----------------------------------------------------------------------------
/**
 *	@brief	振り向き状態設定
 *
 *	@param	p_wk		オブジェクトワーク
 *	@param	pos			座標
 *	@param	way			方向
 */
//-----------------------------------------------------------------------------
static void WF2DMAP_OBJSysWkTurnInit( WF2DMAP_OBJWK* p_wk, WF2DMAP_POS pos, WF2DMAP_WAY way )
{
	GF_ASSERT( p_wk );

#ifdef DEBUG_STCHECK	// 一応動作中に強制定期に設定していないかチェック
	{
		u32 st = WF2DMAP_OBJWkDataGet( p_wk, WF2DMAP_OBJPM_ST );
		if( st != WF2DMAP_OBJST_NONE ){
			// NG
			GF_ASSERT(0);
		}
	}
#endif

	// 座標を設定
	WF2DMAP_OBJWkMatrixSet( p_wk, pos );
	WF2DMAP_OBJWkLastMatrixSet( p_wk, pos );

	// 方向を設定	データ的には先に振り向かせてしまう
	WF2DMAP_OBJWkDataSet( p_wk, WF2DMAP_OBJPM_WAY, way );

	// 状態を設定
	WF2DMAP_OBJWkDataSet( p_wk, WF2DMAP_OBJPM_ST, WF2DMAP_OBJST_TURN );
	
	// フレーム初期化
	WF2DMAP_OBJWkDataSet( p_wk, WF2DMAP_OBJPM_FRAME, 0 );
	WF2DMAP_OBJWkDataSet( p_wk, WF2DMAP_OBJPM_ENDFRAME, WF2DMAP_FRAME_TURN );
}

//----------------------------------------------------------------------------
/**
 *	@brief	歩き状態設定
 *
 *	@param	p_wk		オブジェクトワーク
 *	@param	pos			座標
 *	@param	way			方向
 */
//-----------------------------------------------------------------------------
static void WF2DMAP_OBJSysWkWalkInit( WF2DMAP_OBJWK* p_wk, WF2DMAP_POS pos, WF2DMAP_WAY way )
{
	WF2DMAP_POS next_pos;
	GF_ASSERT( p_wk );

#ifdef DEBUG_STCHECK	// 一応動作中に強制定期に設定していないかチェック
	{
		u32 st = WF2DMAP_OBJWkDataGet( p_wk, WF2DMAP_OBJPM_ST );
		if( st != WF2DMAP_OBJST_NONE ){
			// NG
			GF_ASSERT(0);
		}
	}
#endif

	// 座標を設定
	next_pos = WF2DMAP_OBJSysWayPosGet( pos, way );	// 次の座標取得
	WF2DMAP_OBJWkLastMatrixSet( p_wk, pos );
	WF2DMAP_OBJWkMatrixSet( p_wk, next_pos );

	// 方向を設定
	WF2DMAP_OBJWkDataSet( p_wk, WF2DMAP_OBJPM_WAY, way );

	// 状態を設定
	WF2DMAP_OBJWkDataSet( p_wk, WF2DMAP_OBJPM_ST, WF2DMAP_OBJST_WALK );
	
	// フレーム初期化
	WF2DMAP_OBJWkDataSet( p_wk, WF2DMAP_OBJPM_FRAME, 0 );
	WF2DMAP_OBJWkDataSet( p_wk, WF2DMAP_OBJPM_ENDFRAME, WF2DMAP_FRAME_WALK );
}

//----------------------------------------------------------------------------
/**
 *	@brief	走り状態設定
 *
 *	@param	p_wk		オブジェクトワーク
 *	@param	pos			座標
 *	@param	way			方向
 */
//-----------------------------------------------------------------------------
static void WF2DMAP_OBJSysWkRunInit( WF2DMAP_OBJWK* p_wk, WF2DMAP_POS pos, WF2DMAP_WAY way )
{
	WF2DMAP_POS next_pos;
	GF_ASSERT( p_wk );

#ifdef DEBUG_STCHECK	// 一応動作中に強制定期に設定していないかチェック
	{
		u32 st = WF2DMAP_OBJWkDataGet( p_wk, WF2DMAP_OBJPM_ST );
		if( st != WF2DMAP_OBJST_NONE ){
			// NG
			GF_ASSERT(0);
		}
	}
#endif

	// 座標を設定
	next_pos = WF2DMAP_OBJSysWayPosGet( pos, way );	// 次の座標取得
	WF2DMAP_OBJWkLastMatrixSet( p_wk, pos );
	WF2DMAP_OBJWkMatrixSet( p_wk, next_pos );

	// 方向を設定
	WF2DMAP_OBJWkDataSet( p_wk, WF2DMAP_OBJPM_WAY, way );

	// 状態を設定
	WF2DMAP_OBJWkDataSet( p_wk, WF2DMAP_OBJPM_ST, WF2DMAP_OBJST_RUN );
	
	// フレーム初期化
	WF2DMAP_OBJWkDataSet( p_wk, WF2DMAP_OBJPM_FRAME, 0 );
	WF2DMAP_OBJWkDataSet( p_wk, WF2DMAP_OBJPM_ENDFRAME, WF2DMAP_FRAME_RUN );
}

//----------------------------------------------------------------------------
/**
 *	@brief	忙しい状態設定
 *
 *	@param	p_wk		オブジェクトワーク
 *	@param	pos			座標
 *	@param	way			方向
 */
//-----------------------------------------------------------------------------
static void WF2DMAP_OBJSysWkBusyInit( WF2DMAP_OBJWK* p_wk, WF2DMAP_POS pos, WF2DMAP_WAY way )
{
	GF_ASSERT( p_wk );

#ifdef DEBUG_STCHECK	// 一応動作中に強制定期に設定していないかチェック
	{
		u32 st = WF2DMAP_OBJWkDataGet( p_wk, WF2DMAP_OBJPM_ST );
		if( st != WF2DMAP_OBJST_NONE ){
			// NG
			GF_ASSERT(0);
		}
	}
#endif

	// 座標を設定
	WF2DMAP_OBJWkMatrixSet( p_wk, pos );
	WF2DMAP_OBJWkLastMatrixSet( p_wk, pos );

	// 方向を設定
	WF2DMAP_OBJWkDataSet( p_wk, WF2DMAP_OBJPM_WAY, way );

	// 状態を設定
	WF2DMAP_OBJWkDataSet( p_wk, WF2DMAP_OBJPM_ST, WF2DMAP_OBJST_BUSY );
}

//----------------------------------------------------------------------------
/**
 *	@brief	壁方向歩き状態設定
 *
 *	@param	p_wk		オブジェクトワーク
 *	@param	pos			座標
 *	@param	way			方向
 */
//-----------------------------------------------------------------------------
static void WF2DMAP_OBJSysWkWallWalkInit( WF2DMAP_OBJWK* p_wk, WF2DMAP_POS pos, WF2DMAP_WAY way )
{
	GF_ASSERT( p_wk );

#ifdef DEBUG_STCHECK	// 一応動作中に強制定期に設定していないかチェック
	{
		u32 st = WF2DMAP_OBJWkDataGet( p_wk, WF2DMAP_OBJPM_ST );
		if( st != WF2DMAP_OBJST_NONE ){
			// NG
			GF_ASSERT(0);
		}
	}
#endif

	// 座標を設定
	WF2DMAP_OBJWkMatrixSet( p_wk, pos );
	WF2DMAP_OBJWkLastMatrixSet( p_wk, pos );

	// 方向を設定
	WF2DMAP_OBJWkDataSet( p_wk, WF2DMAP_OBJPM_WAY, way );

	// 状態を設定
	WF2DMAP_OBJWkDataSet( p_wk, WF2DMAP_OBJPM_ST, WF2DMAP_OBJST_WALLWALK );

	// フレーム初期化
	WF2DMAP_OBJWkDataSet( p_wk, WF2DMAP_OBJPM_FRAME, 0 );
	WF2DMAP_OBJWkDataSet( p_wk, WF2DMAP_OBJPM_ENDFRAME, WF2DMAP_FRAME_WALLWALK );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ゆっくり歩き状態設定
 *
 *	@param	p_wk		オブジェクトワーク
 *	@param	pos			座標
 *	@param	way			方向
 */
//-----------------------------------------------------------------------------
static void WF2DMAP_OBJSysWkSlowWalkInit( WF2DMAP_OBJWK* p_wk, WF2DMAP_POS pos, WF2DMAP_WAY way )
{
	WF2DMAP_POS next_pos;
	GF_ASSERT( p_wk );

#ifdef DEBUG_STCHECK	// 一応動作中に強制定期に設定していないかチェック
	{
		u32 st = WF2DMAP_OBJWkDataGet( p_wk, WF2DMAP_OBJPM_ST );
		if( st != WF2DMAP_OBJST_NONE ){
			// NG
			GF_ASSERT(0);
		}
	}
#endif

	// 座標を設定
	next_pos = WF2DMAP_OBJSysWayPosGet( pos, way );	// 次の座標取得
	WF2DMAP_OBJWkLastMatrixSet( p_wk, pos );
	WF2DMAP_OBJWkMatrixSet( p_wk, next_pos );

	// 方向を設定
	WF2DMAP_OBJWkDataSet( p_wk, WF2DMAP_OBJPM_WAY, way );

	// 状態を設定
	WF2DMAP_OBJWkDataSet( p_wk, WF2DMAP_OBJPM_ST, WF2DMAP_OBJST_SLOWWALK );
	
	// フレーム初期化
	WF2DMAP_OBJWkDataSet( p_wk, WF2DMAP_OBJPM_FRAME, 0 );
	WF2DMAP_OBJWkDataSet( p_wk, WF2DMAP_OBJPM_ENDFRAME, WF2DMAP_FRAME_SLOWWALK );
}

//----------------------------------------------------------------------------
/**
 *	@brief	4シンク	歩き
 *
 *	@param	p_wk		ワーク
 *	@param	pos			位置
 *	@param	way			方向
 */
//-----------------------------------------------------------------------------
static void WF2DMAP_OBJSysWkWalk4Init( WF2DMAP_OBJWK* p_wk, WF2DMAP_POS pos, WF2DMAP_WAY way )
{
	WF2DMAP_POS next_pos;
	GF_ASSERT( p_wk );

#ifdef DEBUG_STCHECK	// 一応動作中に強制定期に設定していないかチェック
	{
		u32 st = WF2DMAP_OBJWkDataGet( p_wk, WF2DMAP_OBJPM_ST );
		if( st != WF2DMAP_OBJST_NONE ){
			// NG
			GF_ASSERT(0);
		}
	}
#endif

	// 座標を設定
	next_pos = WF2DMAP_OBJSysWayPosGet( pos, way );	// 次の座標取得
	WF2DMAP_OBJWkLastMatrixSet( p_wk, pos );
	WF2DMAP_OBJWkMatrixSet( p_wk, next_pos );

	// 方向を設定
	WF2DMAP_OBJWkDataSet( p_wk, WF2DMAP_OBJPM_WAY, way );

	// 状態を設定
	WF2DMAP_OBJWkDataSet( p_wk, WF2DMAP_OBJPM_ST, WF2DMAP_OBJST_WALK4 );
	
	// フレーム初期化
	WF2DMAP_OBJWkDataSet( p_wk, WF2DMAP_OBJPM_FRAME, 0 );
	WF2DMAP_OBJWkDataSet( p_wk, WF2DMAP_OBJPM_ENDFRAME, WF2DMAP_FRAME_WALK4 );
}

//----------------------------------------------------------------------------
/**
 *	@brief	その場歩き２フレーム	初期化
 *
 *	@param	p_wk		オブジェクトワーク
 *	@param	pos			座標
 *	@param	way			方向
 */
//-----------------------------------------------------------------------------
static void WF2DMAP_OBJSysWkStayWalk2Init( WF2DMAP_OBJWK* p_wk, WF2DMAP_POS pos, WF2DMAP_WAY way )
{
	GF_ASSERT( p_wk );

#ifdef DEBUG_STCHECK	// 一応動作中に強制定期に設定していないかチェック
	{
		u32 st = WF2DMAP_OBJWkDataGet( p_wk, WF2DMAP_OBJPM_ST );
		if( st != WF2DMAP_OBJST_NONE ){
			// NG
			GF_ASSERT(0);
		}
	}
#endif

	// 座標を設定
	WF2DMAP_OBJWkLastMatrixSet( p_wk, pos );
	WF2DMAP_OBJWkMatrixSet( p_wk, pos );

	// 方向を設定
	WF2DMAP_OBJWkDataSet( p_wk, WF2DMAP_OBJPM_WAY, way );

	// 状態を設定
	WF2DMAP_OBJWkDataSet( p_wk, WF2DMAP_OBJPM_ST, WF2DMAP_OBJST_STAYWALK2 );
	
	// フレーム初期化
	WF2DMAP_OBJWkDataSet( p_wk, WF2DMAP_OBJPM_FRAME, 0 );
	WF2DMAP_OBJWkDataSet( p_wk, WF2DMAP_OBJPM_ENDFRAME, WF2DMAP_FRAME_WALK2 );
}

//----------------------------------------------------------------------------
/**
 *	@brief	その場歩き４フレーム	初期化
 *
 *	@param	p_wk		オブジェクトワーク
 *	@param	pos			座標
 *	@param	way			方向
 */
//-----------------------------------------------------------------------------
static void WF2DMAP_OBJSysWkStayWalk4Init( WF2DMAP_OBJWK* p_wk, WF2DMAP_POS pos, WF2DMAP_WAY way )
{
	GF_ASSERT( p_wk );

#ifdef DEBUG_STCHECK	// 一応動作中に強制定期に設定していないかチェック
	{
		u32 st = WF2DMAP_OBJWkDataGet( p_wk, WF2DMAP_OBJPM_ST );
		if( st != WF2DMAP_OBJST_NONE ){
			// NG
			GF_ASSERT(0);
		}
	}
#endif

	// 座標を設定
	WF2DMAP_OBJWkLastMatrixSet( p_wk, pos );
	WF2DMAP_OBJWkMatrixSet( p_wk, pos );

	// 方向を設定
	WF2DMAP_OBJWkDataSet( p_wk, WF2DMAP_OBJPM_WAY, way );

	// 状態を設定
	WF2DMAP_OBJWkDataSet( p_wk, WF2DMAP_OBJPM_ST, WF2DMAP_OBJST_STAYWALK4 );
	
	// フレーム初期化
	WF2DMAP_OBJWkDataSet( p_wk, WF2DMAP_OBJPM_FRAME, 0 );
	WF2DMAP_OBJWkDataSet( p_wk, WF2DMAP_OBJPM_ENDFRAME, WF2DMAP_FRAME_WALK4 );
}

//----------------------------------------------------------------------------
/**
 *	@brief	その場歩き８フレーム	初期化
 *
 *	@param	p_wk		オブジェクトワーク
 *	@param	pos			座標
 *	@param	way			方向
 */
//-----------------------------------------------------------------------------
static void WF2DMAP_OBJSysWkStayWalk8Init( WF2DMAP_OBJWK* p_wk, WF2DMAP_POS pos, WF2DMAP_WAY way )
{
	GF_ASSERT( p_wk );

#ifdef DEBUG_STCHECK	// 一応動作中に強制定期に設定していないかチェック
	{
		u32 st = WF2DMAP_OBJWkDataGet( p_wk, WF2DMAP_OBJPM_ST );
		if( st != WF2DMAP_OBJST_NONE ){
			// NG
			GF_ASSERT(0);
		}
	}
#endif

	// 座標を設定
	WF2DMAP_OBJWkLastMatrixSet( p_wk, pos );
	WF2DMAP_OBJWkMatrixSet( p_wk, pos );

	// 方向を設定
	WF2DMAP_OBJWkDataSet( p_wk, WF2DMAP_OBJPM_WAY, way );

	// 状態を設定
	WF2DMAP_OBJWkDataSet( p_wk, WF2DMAP_OBJPM_ST, WF2DMAP_OBJST_STAYWALK8 );
	
	// フレーム初期化
	WF2DMAP_OBJWkDataSet( p_wk, WF2DMAP_OBJPM_FRAME, 0 );
	WF2DMAP_OBJWkDataSet( p_wk, WF2DMAP_OBJPM_ENDFRAME, WF2DMAP_FRAME_WALK );
}

//----------------------------------------------------------------------------
/**
 *	@brief	その場歩き１６フレーム	初期化
 *
 *	@param	p_wk		オブジェクトワーク
 *	@param	pos			座標
 *	@param	way			方向
 */
//-----------------------------------------------------------------------------
static void WF2DMAP_OBJSysWkStayWalk16Init( WF2DMAP_OBJWK* p_wk, WF2DMAP_POS pos, WF2DMAP_WAY way )
{
	GF_ASSERT( p_wk );

#ifdef DEBUG_STCHECK	// 一応動作中に強制定期に設定していないかチェック
	{
		u32 st = WF2DMAP_OBJWkDataGet( p_wk, WF2DMAP_OBJPM_ST );
		if( st != WF2DMAP_OBJST_NONE ){
			// NG
			GF_ASSERT(0);
		}
	}
#endif

	// 座標を設定
	WF2DMAP_OBJWkLastMatrixSet( p_wk, pos );
	WF2DMAP_OBJWkMatrixSet( p_wk, pos );

	// 方向を設定
	WF2DMAP_OBJWkDataSet( p_wk, WF2DMAP_OBJPM_WAY, way );

	// 状態を設定
	WF2DMAP_OBJWkDataSet( p_wk, WF2DMAP_OBJPM_ST, WF2DMAP_OBJST_STAYWALK16 );
	
	// フレーム初期化
	WF2DMAP_OBJWkDataSet( p_wk, WF2DMAP_OBJPM_FRAME, 0 );
	WF2DMAP_OBJWkDataSet( p_wk, WF2DMAP_OBJPM_ENDFRAME, WF2DMAP_FRAME_SLOWWALK );
}


//----------------------------------------------------------------------------
/**
 *	@brief	待機状態メイン
 *
 *	@param	p_wk		オブジェクトワーク
 *
 *	@retval	TRUE	動作完了
 *	@retval	TRUE	動作中
 */
//-----------------------------------------------------------------------------
static BOOL WF2DMAP_OBJSysWkNoneMain( WF2DMAP_OBJWK* p_wk )
{
	// 待機なので何もしない
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	振り向き状態メイン
 *
 *	@param	p_wk		オブジェクトワーク
 *
 *	@retval	TRUE	動作完了
 *	@retval	TRUE	動作中
 */
//-----------------------------------------------------------------------------
static BOOL WF2DMAP_OBJSysWkTurnMain( WF2DMAP_OBJWK* p_wk )
{
	return WF2DMAP_OBJSysFrameCont( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	歩き状態メイン
 *
 *	@param	p_wk		オブジェクトワーク
 *
 *	@retval	TRUE	動作完了
 *	@retval	TRUE	動作中
 */
//-----------------------------------------------------------------------------
static BOOL WF2DMAP_OBJSysWkWalkMain( WF2DMAP_OBJWK* p_wk )
{
	BOOL result;
	WF2DMAP_POS pos;
	result = WF2DMAP_OBJSysFrameCont( p_wk );
	if( result == TRUE ){
		pos = WF2DMAP_OBJWkMatrixGet( p_wk );
		WF2DMAP_OBJWkLastMatrixSet( p_wk, pos );
	}
	return result;
}

//----------------------------------------------------------------------------
/**
 *	@brief	走り状態メイン
 *
 *	@param	p_wk		オブジェクトワーク
 *
 *	@retval	TRUE	動作完了
 *	@retval	TRUE	動作中
 */
//-----------------------------------------------------------------------------
static BOOL WF2DMAP_OBJSysWkRunMain( WF2DMAP_OBJWK* p_wk )
{
	BOOL result;
	WF2DMAP_POS pos;
	result = WF2DMAP_OBJSysFrameCont( p_wk );
	if( result == TRUE ){
		pos = WF2DMAP_OBJWkMatrixGet( p_wk );
		WF2DMAP_OBJWkLastMatrixSet( p_wk, pos );
	}
	return result;
}

//----------------------------------------------------------------------------
/**
 *	@brief	忙しい状態メイン
 *
 *	@param	p_wk		オブジェクトワーク
 *
 *	@retval	TRUE	動作完了
 *	@retval	TRUE	動作中
 */
//-----------------------------------------------------------------------------
static BOOL WF2DMAP_OBJSysWkBusyMain( WF2DMAP_OBJWK* p_wk )
{
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	壁歩き状態メイン
 *
 *	@param	p_wk		オブジェクトワーク
 *
 *	@retval	TRUE	動作完了
 *	@retval	TRUE	動作中
 */
//-----------------------------------------------------------------------------
static BOOL WF2DMAP_OBJSysWkWallWalkMain( WF2DMAP_OBJWK* p_wk )
{
	return WF2DMAP_OBJSysFrameCont( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	その場歩き状態メイン
 *
 *	@param	p_wk		オブジェクトワーク
 *
 *	@retval	TRUE	動作完了
 *	@retval	TRUE	動作中
 */
//-----------------------------------------------------------------------------
static BOOL WF2DMAP_OBJSysWkStayWalk2Main( WF2DMAP_OBJWK* p_wk )
{
	return WF2DMAP_OBJSysFrameCont( p_wk );
}



//----------------------------------------------------------------------------
/**
 *	@brief	wayの方向に１つ進んだ座標を取得する
 *
 *	@param	pos		座標
 *	@param	way		方向
 *
 *	@return	wayの方向に１つ進んだ座標
 */
//-----------------------------------------------------------------------------
static WF2DMAP_POS WF2DMAP_OBJSysWayPosGet( WF2DMAP_POS pos, WF2DMAP_WAY way )
{
	return WF2DMAP_OBJToolWayPosGet( pos, way );
}

//----------------------------------------------------------------------------
/**
 *	@brief	フレーム管理
 *	
 *	@param	p_wk	オブジェクトワーク
 *
 *	@retval	TRUE	終了フレームになった
 *	@retval	FALSE	まだ終わりじゃない
 */
//-----------------------------------------------------------------------------
static BOOL WF2DMAP_OBJSysFrameCont( WF2DMAP_OBJWK* p_wk )
{	
	s32 frame;
	s32 endframe;

	frame = WF2DMAP_OBJWkDataGet( p_wk, WF2DMAP_OBJPM_FRAME );
	endframe = WF2DMAP_OBJWkDataGet( p_wk, WF2DMAP_OBJPM_ENDFRAME );
	
	if( frame < endframe ){
		frame ++;
		WF2DMAP_OBJWkDataSet( p_wk, WF2DMAP_OBJPM_FRAME, frame );

		// frameを進めてendframeになった場合はTRUEを返す
		if( frame < endframe ){
			return FALSE;
		}
	}
	return TRUE;
}


//----------------------------------------------------------------------------
/**
 *	@brief	待機状態に初期化する
 *
 *	@param	p_wk	オブジェクトワーク
 */
//-----------------------------------------------------------------------------
static void WF2DMAP_OBJSysWkMoveEnd( WF2DMAP_OBJWK* p_wk )
{
	WF2DMAP_POS pos;
	WF2DMAP_WAY way;

	pos = WF2DMAP_OBJWkMatrixGet( p_wk );
	way = WF2DMAP_OBJWkDataGet( p_wk, WF2DMAP_OBJPM_WAY );
	WF2DMAP_OBJSysWkNoneInit( p_wk, pos, way );
}




