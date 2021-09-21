//==============================================================================================
/**
 * @file	stage_obj.c
 * @brief	「バトルステージ」OBJ(カーソル、アイコン)
 * @author	Satoshi Nohara
 * @date	2007.03.20
 */
//==============================================================================================
#include "common.h"
#include "src_os_print.h"
#include "system/arc_tool.h"
#include "system/arc_util.h"
#include "poketool/pokeicon.h"

#include "stage_sys.h"
#include "stage_clact.h"
#include "stage_obj.h"


//==============================================================================================
//
//	構造体宣言
//
//==============================================================================================
//OBJ(カーソル、アイコン)
struct _STAGE_OBJ{
	u32	pause;						//ポーズフラグ
	u32 work;						//汎用ワーク
	u32 counter;					//カウンター
	const u8* anm_tbl;				//カーソルアニメテーブルのポインタ
	CLACT_WORK_PTR p_clact;			//セルアクターワークポインタ
};

//カーソルのパレット
enum{
	PAL_OBJ_MOVE = 0,
	PAL_OBJ_STOP,
};


//==============================================================================================
//
//	プロトタイプ宣言
//
//==============================================================================================
STAGE_OBJ* StageObj_Create( STAGE_CLACT* stage_clact, u32 data_no, u32 anm_no, u16 x, u16 y, const u8* anm_tbl );
void* StageObj_Delete( STAGE_OBJ* wk );
void StageObj_Vanish( STAGE_OBJ* wk, int flag );
void StageObj_Pause( STAGE_OBJ* wk, int flag );
void StageObj_SetObjPos( STAGE_OBJ* wk, u16 x, u16 y );
void StageObj_AnmChg( STAGE_OBJ* wk, u32 num );
void StageObj_IconPalChg( STAGE_OBJ* wk, POKEMON_PARAM* pp );


//==============================================================================================
//
//	関数
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	カーソルOBJ作成
 *
 * @param	stage_clact	STAGE_CLACT型のポインタ
 * @param	data_index		データindex
 * @param	heapID			ヒープID
 *
 * @retval	"STAGE_OBJワークへのポインタ"
 */
//--------------------------------------------------------------
STAGE_OBJ* StageObj_Create( STAGE_CLACT* stage_clact, u32 data_no, u32 anm_no, u16 x, u16 y, const u8* anm_tbl )
{
	STAGE_OBJ* wk;
	VecFx32	vec;

	wk = sys_AllocMemory( HEAPID_STAGE, sizeof(STAGE_OBJ) );		//メモリ確保
	memset( wk, 0, sizeof(STAGE_OBJ) );

	wk->anm_tbl		= anm_tbl;				//カーソルのアニメテーブルのポインタ

	//アニメテーブルの指定がある時
	if( anm_tbl != NULL ){
		wk->p_clact = StageClact_SetActor( stage_clact, data_no, anm_no, 0, DISP_MAIN );
	}else{
		wk->p_clact = StageClact_SetActor( stage_clact, data_no, anm_no, 0, DISP_MAIN );
	}

	//初期カーソル座標をセット
	vec.x = (x * FX32_ONE);
	vec.y = (y * FX32_ONE);
	CLACT_SetMatrix( wk->p_clact, &vec );

	return wk;
}

//--------------------------------------------------------------
/**
 * @brief	カーソルOBJワーク削除
 *
 * @param	wk		STAGE_OBJワークのポインタ
 *
 * @retval	"NULL"
 */
//--------------------------------------------------------------
void* StageObj_Delete( STAGE_OBJ* wk )
{
	CLACT_Delete( wk->p_clact );

	//STAGE_OBJのメンバでメモリ確保したものがあったら開放する

	sys_FreeMemoryEz( wk );
	return NULL;
}

//--------------------------------------------------------------
/**
 * @brief	バニッシュ操作
 *
 * @param	wk		STAGE_OBJ型のポインタ
 * @param	flag	0:非描画 1:レンダラ描画	
 *
 * @return	none
 */
//--------------------------------------------------------------
void StageObj_Vanish( STAGE_OBJ* wk, int flag )
{
	CLACT_SetDrawFlag( wk->p_clact, flag );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	ポーズ
 *
 * @param	wk		STAGE_OBJ型のポインタ
 * @param	flag	0:ポーズ解除 1:ポーズ
 *
 * @return	none
 */
//--------------------------------------------------------------
void StageObj_Pause( STAGE_OBJ* wk, int flag )
{
	wk->pause = flag;
	return;
}

//--------------------------------------------------------------
/**
 * @brief	カーソル位置をセット
 *
 * @param	wk		STAGE_OBJ型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
void StageObj_SetObjPos( STAGE_OBJ* wk, u16 x, u16 y )
{
	VecFx32	vec;

	//アニメを切り替える
	if( wk->anm_tbl != NULL ){
		CLACT_AnmChgCheck( wk->p_clact, wk->anm_tbl[0] );
	}

	//座標を取得
	vec = *( CLACT_GetMatrix(wk->p_clact) );
	vec.x = (x * FX32_ONE);
	vec.y = (y * FX32_ONE);

	//座標を設定
	CLACT_SetMatrix( wk->p_clact, &vec );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	アニメ切り替え
 *
 * @param	wk		STAGE_OBJワークのポインタ
 *
 * @retval	"NULL"
 */
//--------------------------------------------------------------
void StageObj_AnmChg( STAGE_OBJ* wk, u32 num )
{
	CLACT_SetAnmFrame( wk->p_clact, FX32_ONE );
	//CLACT_AnmFrameSet( wk->p_clact, 0 );	//アニメーションフレームをセット
	CLACT_AnmChgCheck( wk->p_clact, num );		//アニメーションのシーケンスをチェンジする
	return;
}

//--------------------------------------------------------------
/**
 * @brief	アイコンアニメ
 *
 * @param	wk		STAGE_OBJワークのポインタ
 * @param	pp		POKEMON_PARAMワークのポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------
void StageObj_IconPalChg( STAGE_OBJ* wk, POKEMON_PARAM* pp )
{
	//パレット切り替え
	//CLACT_PaletteOffsetChg関数の結果にパレットの転送先先頭パレットナンバーを加算
	CLACT_PaletteOffsetChgAddTransPlttNo( wk->p_clact, PokeIconPalNumGetByPP(pp) );
	return;
}


