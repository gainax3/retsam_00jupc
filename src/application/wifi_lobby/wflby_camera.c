//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wflby_camera.c
 *	@brief		WiFi広場カメラ	
 *	@author		tomoya takahashi
 *	@data		2007.11.14
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "common.h"

#include "wflby_camera.h"
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
//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
#define WFLBY_CAMERA_DIST		( 0x29aec1 )			// 距離
static const CAMERA_ANGLE sc_WFLBY_CAMERA_ANGLE = {		// アングル
	-0x29fe,0,0
};
#define WFLBY_CAMERA_TYPE		( GF_CAMERA_PERSPECTIV )// カメラのタイプ
#define WFLBY_CAMERA_PARS		( 0x05c1 )				// パース
#define WFLBY_CAMERA_NEAR		( FX32_CONST( 150 ) )	// クリッピング　ニア
#define WFLBY_CAMERA_FAR		( FX32_CONST( 900 ) )	// クリッピング　ファー
#define WFLBY_CAMERA_X_OFS		( FX32_CONST( 8 ) )		// カメラターゲットのXオフセット
#define WFLBY_CAMERA_Z_OFS		( -FX32_CONST( 32 ) )		// カメラターゲットのZオフセット

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	カメラシステム
//=====================================
typedef struct _WFLBY_CAMERA {
	GF_CAMERA_PTR			p_camera;
	const WFLBY_3DPERSON*	cp_person;
	VecFx32					target;
}WFLBY_CAMERA;

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------


//----------------------------------------------------------------------------
/**
 *	@brief	カメラ初期化
 *
 *	@param	heapID	ヒープID
 *
 *	@return	ワーク
 */
//-----------------------------------------------------------------------------
WFLBY_CAMERA* WFLBY_CAMERA_Init( u32 heapID )
{
	WFLBY_CAMERA* p_wk;
	VecFx32 target;

	p_wk = sys_AllocMemory( heapID, sizeof(WFLBY_CAMERA) );
	memset( p_wk, 0, sizeof(WFLBY_CAMERA) );

	// メモリ確保
	p_wk->p_camera = GFC_AllocCamera( heapID );

	//カメラセット
	GFC_InitCameraTDA( &p_wk->target,
				WFLBY_CAMERA_DIST,	// 距離
				&sc_WFLBY_CAMERA_ANGLE,	//
				WFLBY_CAMERA_PARS,	// 投射影の角度
				WFLBY_CAMERA_TYPE,
				TRUE,
				p_wk->p_camera );

	//カメラの有効化
	GFC_AttachCamera( p_wk->p_camera );
	
	//ニア・ファー設定
	GFC_SetCameraClip( WFLBY_CAMERA_NEAR, WFLBY_CAMERA_FAR, p_wk->p_camera );

	OS_TPrintf( "透視射影\n" );

	return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief	カメラ破棄
 *
 *	@param	p_sys	ワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_CAMERA_Exit( WFLBY_CAMERA* p_sys )
{

	GFC_PurgeCamera();
		
	GFC_FreeCamera( p_sys->p_camera );

	sys_FreeMemoryEz( p_sys );
}

//----------------------------------------------------------------------------
/**
 *	@brief	カメラ描画
 *
 *	@param	p_sys	ワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_CAMERA_Draw( WFLBY_CAMERA* p_sys )
{
	// ターゲット座標計算
	if( p_sys->cp_person ){
		WFLBY_3DOBJCONT_DRAW_Get3DMatrix( p_sys->cp_person, &p_sys->target );
		p_sys->target.x += WFLBY_CAMERA_X_OFS;
		p_sys->target.z += WFLBY_CAMERA_Z_OFS;
	}
	
	// カメラ反映
	GFC_CameraLookAt();
}

//----------------------------------------------------------------------------
/**
 *	@brief	カメラターゲット設定
 *
 *	@param	p_sys	ワーク
 *	@param	posx	ｘ座標
 *	@param	posy	ｙ座標
 */
//-----------------------------------------------------------------------------
void WFLBY_CAMERA_SetTarget( WFLBY_CAMERA* p_sys, s32 posx, s32 posy )
{
	WF2DMAP_POS pos;
	pos.x = posx;
	pos.y = posy;
	WFLBY_3DMATRIX_GetPosVec( &pos, &p_sys->target );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ターゲット座標を設定	３D
 *	
 *	@param	p_sys	システム
 *	@param	x		ｘ座標
 *	@param	y		ｙ座標
 *	@param	z		ｚ座標
 */
//-----------------------------------------------------------------------------
void WFLBY_CAMERA_SetTarget3D( WFLBY_CAMERA* p_sys, fx32 x, fx32 y, fx32 z )
{
	p_sys->target.x = x;
	p_sys->target.y = y;
	p_sys->target.z = z;
}

//----------------------------------------------------------------------------
/**
 *	@brief	プレイヤー専用座標設定
 *
 *	@param	p_sys	システム
 *	@param	x		ｘ座標
 *	@param	y		ｙ座標
 *	@param	z		ｚ座標
 */
//-----------------------------------------------------------------------------
void WFLBY_CAMERA_SetTargetPerson3D( WFLBY_CAMERA* p_sys, fx32 x, fx32 y, fx32 z )
{
	p_sys->target.x = x + WFLBY_CAMERA_X_OFS;
	p_sys->target.y = y;
	p_sys->target.z = z + WFLBY_CAMERA_Z_OFS;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ターゲットプレイヤーを設定
 *
 *	@param	p_sys		ワーク
 *	@param	cp_person	人物
 */
//-----------------------------------------------------------------------------
void WFLBY_CAMERA_SetTargetPerson( WFLBY_CAMERA* p_sys, const WFLBY_3DPERSON* cp_person )
{
	p_sys->cp_person = cp_person;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ターゲット人物を取得
 *
 *	@param	cp_sys	ワーク
 *
 *	@return	ターゲット人物	NULL:いない
 */
//-----------------------------------------------------------------------------
const WFLBY_3DPERSON* WFLBY_CAMERA_GetTargetPerson( const WFLBY_CAMERA* cp_sys )
{
	return cp_sys->cp_person;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ターゲット人物をリセット（なくす）
 *
 *	@param	p_sys	ワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_CAMERA_ResetTargetPerson( WFLBY_CAMERA* p_sys )
{
	p_sys->cp_person = NULL;
}

#ifdef WFLBY_CAMERA_DEBUG

#define WFLBY_CAMERA_DEBUG_DIST		( 0x61b89b )			// 距離
static const CAMERA_ANGLE sc_WFLBY_CAMERA_DEBUG_ANGLE = {		// アングル
	-0x239e,0,0
};
#define WFLBY_CAMERA_DEBUG_TYPE		( GF_CAMERA_ORTHO )// カメラのタイプ
#define WFLBY_CAMERA_DEBUG_PARS		( 0x0281 )				// パース
#define WFLBY_CAMERA_DEBUG_NEAR		( FX32_CONST( 150 ) )	// クリッピング　ニア
#define WFLBY_CAMERA_DEBUG_FAR		( FX32_CONST( 1735 ) )	// クリッピング　ファー

WFLBY_CAMERA* WFLBY_CAMERA_DEBUG_CameraInit( u32 heapID )
{
	WFLBY_CAMERA* p_wk;
	VecFx32 target;

	p_wk = sys_AllocMemory( heapID, sizeof(WFLBY_CAMERA) );
	memset( p_wk, 0, sizeof(WFLBY_CAMERA) );

	// メモリ確保
	p_wk->p_camera = GFC_AllocCamera( heapID );

	//カメラセット
	GFC_InitCameraTDA( &p_wk->target,
				WFLBY_CAMERA_DEBUG_DIST,	// 距離
				&sc_WFLBY_CAMERA_DEBUG_ANGLE,	//
				WFLBY_CAMERA_DEBUG_PARS,	// 投射影の角度
				WFLBY_CAMERA_DEBUG_TYPE,
				TRUE,
				p_wk->p_camera );

	//カメラの有効化
	GFC_AttachCamera( p_wk->p_camera );
	
	//ニア・ファー設定
	GFC_SetCameraClip( WFLBY_CAMERA_DEBUG_NEAR, WFLBY_CAMERA_DEBUG_FAR, p_wk->p_camera );


	OS_TPrintf( "正射影\n" );

	return p_wk;
}
#endif
