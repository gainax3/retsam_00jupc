//==============================================================================================
/**
 * @file	factory_obj.c
 * @brief	「バトルファクトリー」OBJ(ウィンドウ)
 * @author	Satoshi Nohara
 * @date	2007.10.10
 */
//==============================================================================================
#include "common.h"
#include "src_os_print.h"
#include "system/arc_tool.h"
#include "system/arc_util.h"

#include "factory_sys.h"
#include "factory_clact.h"
#include "factory_obj.h"


//==============================================================================================
//
//	構造体宣言
//
//==============================================================================================
//OBJ
struct _FACTORY_OBJ{
	int	init_x;						//初期位置
	int	init_y;						//初期位置
	CLACT_WORK_PTR p_clact;			//セルアクターワークポインタ
};


//==============================================================================================
//
//	プロトタイプ宣言
//
//==============================================================================================
FACTORY_OBJ*	FactoryObj_Create( FACTORY_CLACT* factory_clact, u32 anm_no, int x, int y, u32 heapID );
void*			FactoryObj_Delete( FACTORY_OBJ* wk );
void			FactoryObj_Vanish( FACTORY_OBJ* wk, int flag );
VecFx32			FactoryObj_Move( FACTORY_OBJ* wk, int x, int y );
void			FactoryObj_SetPos( FACTORY_OBJ* wk, int x, int y );
const VecFx32*	FactoryObj_GetPos( FACTORY_OBJ* wk );
BOOL			FactoryObj_Main( FACTORY_OBJ* wk );
int				FactoryObj_GetInitX( FACTORY_OBJ* wk );
int				FactoryObj_GetInitY( FACTORY_OBJ* wk );
BOOL			FactoryObj_AnmActiveCheck( FACTORY_OBJ* wk );
void			FactoryObj_AnmChg( FACTORY_OBJ* wk, u32 num );


//==============================================================================================
//
//	関数
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	OBJ作成
 *
 * @param	factory_clact	FACTORY_CLACT型のポインタ
 * @param	data_index		データindex
 * @param	heapID			ヒープID
 *
 * @retval	"FACTORY_OBJワークへのポインタ"
 */
//--------------------------------------------------------------
FACTORY_OBJ* FactoryObj_Create( FACTORY_CLACT* factory_clact, u32 anm_no, int x, int y, u32 heapID )
{
	FACTORY_OBJ* wk;

	wk = sys_AllocMemory( heapID, sizeof(FACTORY_OBJ) );		//メモリ確保
	memset( wk, 0, sizeof(FACTORY_OBJ) );

	wk->init_x = x;						//初期位置
	wk->init_y = y;						//初期位置

	//現状、タイプは固定(data_no,anm_no,pri)
	wk->p_clact = FactoryClact_SetActor( factory_clact, 0, anm_no, 0, 1, DISP_MAIN );

	FactoryObj_SetPos( wk, x, y );		//座標セット
	
	return wk;
}

//--------------------------------------------------------------
/**
 * @brief	OBJワーク削除
 *
 * @param	wk		FACTORY_OBJワークのポインタ
 *
 * @retval	"NULL"
 */
//--------------------------------------------------------------
void* FactoryObj_Delete( FACTORY_OBJ* wk )
{
	CLACT_Delete( wk->p_clact );

	//FACTORY_OBJのメンバでメモリ確保したものがあったら開放する

	sys_FreeMemoryEz( wk );
	return NULL;
}

//--------------------------------------------------------------
/**
 * @brief	バニッシュ操作
 *
 * @param	wk		FACTORY_OBJ型のポインタ
 * @param	flag	0:非描画 1:レンダラ描画	
 *
 * @return	none
 */
//--------------------------------------------------------------
void FactoryObj_Vanish( FACTORY_OBJ* wk, int flag )
{
	CLACT_SetDrawFlag( wk->p_clact, flag );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	移動
 *
 * @param	wk		FACTORY_OBJ型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
VecFx32 FactoryObj_Move( FACTORY_OBJ* wk, int x, int y )
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
 * @param	wk		FACTORY_OBJ型のポインタ
 * @param	x		X座標
 * @param	y		Y座標
 *
 * @return	none
 */
//--------------------------------------------------------------
void FactoryObj_SetPos( FACTORY_OBJ* wk, int x, int y )
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
 * @param	wk		FACTORY_OBJ型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
const VecFx32* FactoryObj_GetPos( FACTORY_OBJ* wk )
{
	return CLACT_GetMatrix( wk->p_clact );
}

//--------------------------------------------------------------
/**
 * @brief	メイン
 *
 * @param	wk		FACTORY_OBJ型のポインタ
 *
 * @retval	"FALSE = 削除してよい、TRUE = 何かしらの処理を継続中"
 */
//--------------------------------------------------------------
BOOL FactoryObj_Main( FACTORY_OBJ* wk )
{
	//エラー回避
	//if( wk == NULL ){
	//	return;
	//}

	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief	初期 X位置取得
 *
 * @param	wk		FACTORY_OBJ型のポインタ
 *
 * @return	"X"
 */
//--------------------------------------------------------------
int FactoryObj_GetInitX( FACTORY_OBJ* wk )
{
	return wk->init_x;
}

//--------------------------------------------------------------
/**
 * @brief	初期 Y位置取得
 *
 * @param	wk		FACTORY_OBJ型のポインタ
 *
 * @return	"X"
 */
//--------------------------------------------------------------
int FactoryObj_GetInitY( FACTORY_OBJ* wk )
{
	return wk->init_y;
}

//--------------------------------------------------------------
/**
 * @brief	アニメーション中かチェック
 *
 * @param	wk		FACTORY_OBJ型のポインタ
 *
 * @retval	TRUE	アニメーション中
 * @retval	FALSE	ストップ
 */
//--------------------------------------------------------------
BOOL FactoryObj_AnmActiveCheck( FACTORY_OBJ* wk )
{
	return CLACT_AnmActiveCheck( wk->p_clact );
}

//--------------------------------------------------------------
/**
 * @brief	アニメ切り替え
 *
 * @param	wk		FACTORY_OBJワークのポインタ
 *
 * @retval	"NULL"
 */
//--------------------------------------------------------------
void FactoryObj_AnmChg( FACTORY_OBJ* wk, u32 num )
{
	CLACT_SetAnmFrame( wk->p_clact, FX32_ONE );
	//CLACT_AnmFrameSet( wk->p_clact, 0 );	//アニメーションフレームをセット
	CLACT_AnmChg( wk->p_clact, num );		//アニメーションのシーケンスをチェンジする
	return;
}


