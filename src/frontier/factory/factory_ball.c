//==============================================================================================
/**
 * @file	factory_ball.c
 * @brief	「バトルファクトリー」ボール
 * @author	Satoshi Nohara
 * @date	2007.03.14
 */
//==============================================================================================
#include "common.h"
#include "src_os_print.h"
#include "system/arc_tool.h"
#include "system/arc_util.h"

#include "factory_sys.h"
#include "factory_clact.h"
#include "factory_ball.h"


//==============================================================================================
//
//	構造体宣言
//
//==============================================================================================
//ボールOBJ
struct _FACTORY_BALL{
	u16	decide_flag;				//決定したフラグ
	u16	dmy;						//
	int	init_x;						//初期位置
	int	init_y;						//初期位置
	CLACT_WORK_PTR p_clact;			//セルアクターワークポインタ
};

#define FACTORY_BALL_OFFSET_X	(248)//(216)		//レンタルボール最初の右へ隠すオフセット


//==============================================================================================
//
//	プロトタイプ宣言
//
//==============================================================================================
FACTORY_BALL*	FactoryBall_Create( FACTORY_CLACT* factory_clact, int x, int y, u32 heapID );
void*			FactoryBall_Delete( FACTORY_BALL* wk );
void			FactoryBall_Vanish( FACTORY_BALL* wk, int flag );
VecFx32			FactoryBall_Move( FACTORY_BALL* wk, int x, int y );
void			FactoryBall_SetPos( FACTORY_BALL* wk, int x, int y );
const VecFx32*	FactoryBall_GetPos( FACTORY_BALL* wk );
void			FactoryBall_SetDecideFlag( FACTORY_BALL* wk, u8 hit );
u8				FactoryBall_GetDecideFlag( FACTORY_BALL* wk );
BOOL			FactoryBall_Main( FACTORY_BALL* wk );
void			FactoryBall_Decide( FACTORY_BALL* wk );
void			FactoryBall_Cancel( FACTORY_BALL* wk );
void			FactoryBall_Entrance( FACTORY_BALL* wk );
int				FactoryBall_GetInitX( FACTORY_BALL* wk );
int				FactoryBall_GetInitY( FACTORY_BALL* wk );
BOOL			FactoryBall_AnmActiveCheck( FACTORY_BALL* wk );
void			FactoryBall_AnmChg( FACTORY_BALL* wk, u32 num );
void			FactoryBall_PalChg( FACTORY_BALL* wk, u32 pal );


//==============================================================================================
//
//	関数
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	ボールOBJ作成
 *
 * @param	factory_clact	FACTORY_CLACT型のポインタ
 * @param	data_index		データindex
 * @param	heapID			ヒープID
 *
 * @retval	"FACTORY_BALLワークへのポインタ"
 */
//--------------------------------------------------------------
FACTORY_BALL* FactoryBall_Create( FACTORY_CLACT* factory_clact, int x, int y, u32 heapID )
{
	FACTORY_BALL* wk;

	wk = sys_AllocMemory( heapID, sizeof(FACTORY_BALL) );		//メモリ確保
	memset( wk, 0, sizeof(FACTORY_BALL) );

	wk->decide_flag	= 0;				//決定したフラグ
	wk->init_x = x;						//初期位置
	wk->init_y = y;						//初期位置

	//現状、タイプは固定(data_no,anm_no,pri)
	wk->p_clact = FactoryClact_SetActor( factory_clact, 0, ANM_BALL_STOP, 0, 0, DISP_MAIN );

	FactoryBall_SetPos( wk, x, y );		//座標セット
	
	//パレットナンバーを変更
	CLACT_PaletteNoChg( wk->p_clact, PAL_BALL_STOP );

	return wk;
}

//--------------------------------------------------------------
/**
 * @brief	ボールOBJワーク削除
 *
 * @param	wk		FACTORY_BALLワークのポインタ
 *
 * @retval	"NULL"
 */
//--------------------------------------------------------------
void* FactoryBall_Delete( FACTORY_BALL* wk )
{
	CLACT_Delete( wk->p_clact );

	//FACTORY_BALLのメンバでメモリ確保したものがあったら開放する

	sys_FreeMemoryEz( wk );
	return NULL;
}

//--------------------------------------------------------------
/**
 * @brief	バニッシュ操作
 *
 * @param	wk		FACTORY_BALL型のポインタ
 * @param	flag	0:非描画 1:レンダラ描画	
 *
 * @return	none
 */
//--------------------------------------------------------------
void FactoryBall_Vanish( FACTORY_BALL* wk, int flag )
{
	CLACT_SetDrawFlag( wk->p_clact, flag );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	移動
 *
 * @param	wk		FACTORY_BALL型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
VecFx32 FactoryBall_Move( FACTORY_BALL* wk, int x, int y )
{
	VecFx32	vec;

	vec = *( CLACT_GetMatrix(wk->p_clact) );
	vec.x += (x * FX32_ONE);
	vec.y += (y * FX32_ONE);
	CLACT_SetMatrix( wk->p_clact, &vec );
	return *( CLACT_GetMatrix(wk->p_clact) );
}

//--------------------------------------------------------------
/**
 * @brief	座標セット
 *
 * @param	wk		FACTORY_BALL型のポインタ
 * @param	x		X座標
 * @param	y		Y座標
 *
 * @return	none
 */
//--------------------------------------------------------------
void FactoryBall_SetPos( FACTORY_BALL* wk, int x, int y )
{
	VecFx32	vec;

	vec.x = (x * FX32_ONE);
	vec.y = (y * FX32_ONE);

	//vec.y += SUB_SURFACE_Y;						//1=サブ画面に変更

	CLACT_SetMatrix( wk->p_clact, &vec );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	座標取得
 *
 * @param	wk		FACTORY_BALL型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
const VecFx32* FactoryBall_GetPos( FACTORY_BALL* wk )
{
	return CLACT_GetMatrix( wk->p_clact );
}

//--------------------------------------------------------------
/**
 * @brief	決定したフラグの状態を設定
 *
 * @param	wk		FACTORY_BALL型のポインタ
 * @param	sw		""
 *
 * @retval	none
 */
//--------------------------------------------------------------
void FactoryBall_SetDecideFlag( FACTORY_BALL* wk, u8 flag )
{
	wk->decide_flag = flag;
	return;
}

//--------------------------------------------------------------
/**
 * @brief	決定したフラグの状態を取得(レンタルモード時しか使用不可)
 *
 * @param	wk		FACTORY_BALL型のポインタ
 *
 * @retval	""
 */
//--------------------------------------------------------------
u8 FactoryBall_GetDecideFlag( FACTORY_BALL* wk )
{
	return wk->decide_flag;
}

//--------------------------------------------------------------
/**
 * @brief	メイン
 *
 * @param	wk		FACTORY_BALL型のポインタ
 *
 * @retval	"FALSE = 削除してよい、TRUE = 何かしらの処理を継続中"
 */
//--------------------------------------------------------------
BOOL FactoryBall_Main( FACTORY_BALL* wk )
{
	//エラー回避
	//if( wk == NULL ){
	//	return;
	//}

	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief	決定した状態に変更
 *
 * @param	factory_clact	FACTORY_CLACT型のポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------
void FactoryBall_Decide( FACTORY_BALL* wk )
{
	FactoryBall_SetDecideFlag( wk, 1 );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	キャンセル状態に変更
 *
 * @param	factory_clact	FACTORY_CLACT型のポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------
void FactoryBall_Cancel( FACTORY_BALL* wk )
{
	FactoryBall_SetDecideFlag( wk, 0 );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	入場位置にセットする
 *
 * @param	wk		FACTORY_BALL型のポインタ
 *
 * @return	"TRUE = 入場完了、FALSE = まだ"
 */
//--------------------------------------------------------------
void FactoryBall_Entrance( FACTORY_BALL* wk )
{
	VecFx32	vec;
	const VecFx32* p_vec;

	p_vec = FactoryBall_GetPos( wk );		//現在の座標取得

	vec.x = ((p_vec->x / FX32_ONE)+FACTORY_BALL_OFFSET_X) * FX32_ONE;
	vec.y = p_vec->y;

	CLACT_SetMatrix( wk->p_clact, &vec );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	初期 X位置取得
 *
 * @param	wk		FACTORY_BALL型のポインタ
 *
 * @return	"X"
 */
//--------------------------------------------------------------
int FactoryBall_GetInitX( FACTORY_BALL* wk )
{
	return wk->init_x;
}

//--------------------------------------------------------------
/**
 * @brief	初期 Y位置取得
 *
 * @param	wk		FACTORY_BALL型のポインタ
 *
 * @return	"X"
 */
//--------------------------------------------------------------
int FactoryBall_GetInitY( FACTORY_BALL* wk )
{
	return wk->init_y;
}

//--------------------------------------------------------------
/**
 * @brief	アニメーション中かチェック
 *
 * @param	wk		FACTORY_BALL型のポインタ
 *
 * @retval	TRUE	アニメーション中
 * @retval	FALSE	ストップ
 */
//--------------------------------------------------------------
BOOL FactoryBall_AnmActiveCheck( FACTORY_BALL* wk )
{
	return CLACT_AnmActiveCheck( wk->p_clact );
}

//--------------------------------------------------------------
/**
 * @brief	アニメ切り替え
 *
 * @param	wk		FACTORY_BALLワークのポインタ
 *
 * @retval	"NULL"
 */
//--------------------------------------------------------------
void FactoryBall_AnmChg( FACTORY_BALL* wk, u32 num )
{
	CLACT_SetAnmFrame( wk->p_clact, FX32_ONE );
	//CLACT_AnmFrameSet( wk->p_clact, 0 );	//アニメーションフレームをセット
	CLACT_AnmChg( wk->p_clact, num );		//アニメーションのシーケンスをチェンジする
	return;
}

//--------------------------------------------------------------
/**
 * @brief	パレット切り替え
 *
 * @param	wk		FACTORY_BALLワークのポインタ
 *
 * @retval	"NULL"
 */
//--------------------------------------------------------------
void FactoryBall_PalChg( FACTORY_BALL* wk, u32 pal )
{
	CLACT_PaletteNoChg( wk->p_clact, pal );
	return;
}


