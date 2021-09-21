//==============================================================================================
/**
 * @file	roulette_obj.c
 * @brief	「バトルルーレット」OBJ(カーソル、アイコン)
 * @author	Satoshi Nohara
 * @date	2007.09.05
 */
//==============================================================================================
#include "common.h"
#include "src_os_print.h"
#include "system/arc_tool.h"
#include "system/arc_util.h"
#include "poketool/pokeicon.h"

#include "roulette_sys.h"
#include "roulette_clact.h"
#include "roulette_obj.h"


//==============================================================================================
//
//	構造体宣言
//
//==============================================================================================
//OBJ(カーソル、アイコン)
struct _ROULETTE_OBJ{
	u32	pause;						//ポーズフラグ
	u32 work;						//汎用ワーク

	u16 counter;					//カウンター
	u8	dummy;
	u8	disp;

	u16 init_x;
	u16 init_y;
	CLACT_WORK_PTR p_clact;			//セルアクターワークポインタ
};


//==============================================================================================
//
//	プロトタイプ宣言
//
//==============================================================================================
ROULETTE_OBJ* RouletteObj_Create( ROULETTE_CLACT* roulette_clact, u32 char_no, u32 pltt_no, u32 cell_no, u32 anm_no, u16 x, u16 y, u32 pri, int bg_pri, u8 disp );
static ROULETTE_OBJ* RouletteObj_CreateSub( ROULETTE_CLACT* roulette_clact, u32 char_no, u32 pltt_no, u32 cell_no, u32 anm_no, u32 pri, int bg_pri, u8 disp );
void* RouletteObj_Delete( ROULETTE_OBJ* wk );
void RouletteObj_Vanish( ROULETTE_OBJ* wk, int flag );
void RouletteObj_Pause( ROULETTE_OBJ* wk, int flag );
u16 RouletteObj_GetObjX( ROULETTE_OBJ* wk );
u16 RouletteObj_GetObjY( ROULETTE_OBJ* wk );
void RouletteObj_SetObjPos( ROULETTE_OBJ* wk, u16 x, u16 y );
void RouletteObj_AnmChg( ROULETTE_OBJ* wk, u32 num );
void RouletteObj_IconPalChg( ROULETTE_OBJ* wk, POKEMON_PARAM* pp );
u16 RouletteObj_GetAnmFrame( ROULETTE_OBJ* wk );
void RouletteObj_SetInitPosOffset( ROULETTE_OBJ* wk, int offset_x, int offset_y );
void RouletteObj_SetScaleAffine( ROULETTE_OBJ* wk, u32 no );
void RouletteObj_SetAnmFlag( ROULETTE_OBJ* wk, u8 flag );


//==============================================================================================
//
//	関数
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	カーソルOBJ作成
 *
 * @param	roulette_clact	ROULETTE_CLACT型のポインタ
 * @param	data_index		データindex
 * @param	heapID			ヒープID
 *
 * @retval	"ROULETTE_OBJワークへのポインタ"
 */
//--------------------------------------------------------------
ROULETTE_OBJ* RouletteObj_Create( ROULETTE_CLACT* roulette_clact, u32 char_no, u32 pltt_no, u32 cell_no, u32 anm_no, u16 x, u16 y, u32 pri, int bg_pri, u8 disp )
{
	ROULETTE_OBJ* wk;
	VecFx32	vec;

	wk = sys_AllocMemory( HEAPID_ROULETTE, sizeof(ROULETTE_OBJ) );		//メモリ確保
	memset( wk, 0, sizeof(ROULETTE_OBJ) );

	//アニメテーブルの指定がある時
	wk->p_clact = RouletteClact_SetActor(	roulette_clact, char_no, pltt_no, cell_no, anm_no, 
											pri, bg_pri, disp );

	wk->init_x		= x;
	wk->init_y		= y;
	wk->disp		= disp;

	//初期カーソル座標をセット
	vec.x = (x * FX32_ONE);
	vec.y = (y * FX32_ONE);

	if( disp == DISP_SUB ){
		vec.y += SUB_SURFACE_Y;				//座標を補正
	}

	CLACT_SetMatrix( wk->p_clact, &vec );

	return wk;
}

//--------------------------------------------------------------
/**
 * @brief	カーソルOBJワーク削除
 *
 * @param	wk		ROULETTE_OBJワークのポインタ
 *
 * @retval	"NULL"
 */
//--------------------------------------------------------------
void* RouletteObj_Delete( ROULETTE_OBJ* wk )
{
	CLACT_Delete( wk->p_clact );

	//ROULETTE_OBJのメンバでメモリ確保したものがあったら開放する

	sys_FreeMemoryEz( wk );
	return NULL;
}

//--------------------------------------------------------------
/**
 * @brief	バニッシュ操作
 *
 * @param	wk		ROULETTE_OBJ型のポインタ
 * @param	flag	0:非描画 1:レンダラ描画	
 *
 * @return	none
 */
//--------------------------------------------------------------
void RouletteObj_Vanish( ROULETTE_OBJ* wk, int flag )
{
	CLACT_SetDrawFlag( wk->p_clact, flag );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	ポーズ
 *
 * @param	wk		ROULETTE_OBJ型のポインタ
 * @param	flag	0:ポーズ解除 1:ポーズ
 *
 * @return	none
 */
//--------------------------------------------------------------
void RouletteObj_Pause( ROULETTE_OBJ* wk, int flag )
{
	wk->pause = flag;
	return;
}

//--------------------------------------------------------------
/**
 * @brief	カーソルX位置を取得
 *
 * @param	wk		ROULETTE_OBJ型のポインタ
 *
 * @return	"X"
 */
//--------------------------------------------------------------
u16 RouletteObj_GetObjX( ROULETTE_OBJ* wk )
{
	VecFx32	vec;
	vec = *( CLACT_GetMatrix(wk->p_clact) );
	return (vec.x / FX32_ONE);
}

//--------------------------------------------------------------
/**
 * @brief	カーソルY位置を取得
 *
 * @param	wk		ROULETTE_OBJ型のポインタ
 *
 * @return	"X"
 */
//--------------------------------------------------------------
u16 RouletteObj_GetObjY( ROULETTE_OBJ* wk )
{
	VecFx32	vec;
	vec = *( CLACT_GetMatrix(wk->p_clact) );
	return (vec.y / FX32_ONE);
}

//--------------------------------------------------------------
/**
 * @brief	カーソル位置をセット
 *
 * @param	wk		ROULETTE_OBJ型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
void RouletteObj_SetObjPos( ROULETTE_OBJ* wk, u16 x, u16 y )
{
	VecFx32	vec;

	//座標を取得
	vec = *( CLACT_GetMatrix(wk->p_clact) );
	vec.x = (x * FX32_ONE);
	vec.y = (y * FX32_ONE);

	if( wk->disp == DISP_SUB ){
		vec.y += SUB_SURFACE_Y;				//座標を補正
	}

	//座標を設定
	CLACT_SetMatrix( wk->p_clact, &vec );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	アニメ切り替え
 *
 * @param	wk		ROULETTE_OBJワークのポインタ
 *
 * @retval	"NULL"
 */
//--------------------------------------------------------------
void RouletteObj_AnmChg( ROULETTE_OBJ* wk, u32 num )
{
	CLACT_SetAnmFrame( wk->p_clact, FX32_ONE );
	//CLACT_AnmFrameSet( wk->p_clact, 0 );		//アニメーションフレームをセット
	CLACT_AnmChgCheck( wk->p_clact, num );		//アニメーションのシーケンスをチェンジする(Check)
	return;
}

//--------------------------------------------------------------
/**
 * @brief	アイコンアニメ
 *
 * @param	wk		ROULETTE_OBJワークのポインタ
 * @param	pp		POKEMON_PARAMワークのポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------
void RouletteObj_IconPalChg( ROULETTE_OBJ* wk, POKEMON_PARAM* pp )
{
	//パレット切り替え
	//CLACT_PaletteOffsetChg関数の結果にパレットの転送先先頭パレットナンバーを加算
	CLACT_PaletteOffsetChgAddTransPlttNo( wk->p_clact, PokeIconPalNumGetByPP(pp) );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	アニメフレーム取得
 *
 * @param	wk		ROULETTE_OBJワークのポインタ
 *
 * @retval	"アニメフレーム"
 */
//--------------------------------------------------------------
u16 RouletteObj_GetAnmFrame( ROULETTE_OBJ* wk )
{
	return CLACT_AnmFrameGet( wk->p_clact );
}

//--------------------------------------------------------------
/**
 * @brief	初期座標に指定したオフセットを足して座標セット
 *
 * @param	wk		ROULETTE_OBJワークのポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------
void RouletteObj_SetInitPosOffset( ROULETTE_OBJ* wk, int offset_x, int offset_y )
{
	RouletteObj_SetObjPos( wk, wk->init_x+offset_x, wk->init_y+offset_y );
	return;
}

//--------------------------------------------------------------
///	拡大率
//--------------------------------------------------------------
static const VecFx32 scale_tbl[] =
{
	{ FX32_ONE,				FX32_ONE, FX32_ONE },				//等倍
	
	//{ FX32_ONE*2,			FX32_ONE*2,				FX32_ONE*2	},
	{ FX32_ONE+(FX32_ONE/2),FX32_ONE+(FX32_ONE/2),	FX32_ONE+(FX32_ONE/2) },
	{ FX32_ONE+(FX32_ONE/4),FX32_ONE+(FX32_ONE/4),	FX32_ONE+(FX32_ONE/4) },
	{ FX32_ONE+(FX32_ONE/5),FX32_ONE+(FX32_ONE/5),	FX32_ONE+(FX32_ONE/5) },
	{ FX32_ONE+(FX32_ONE/7),FX32_ONE+(FX32_ONE/7),	FX32_ONE+(FX32_ONE/7) },

	{ FX32_ONE-(FX32_ONE/2),FX32_ONE-(FX32_ONE/2),	FX32_ONE-(FX32_ONE/2) },
	{ FX32_ONE-(FX32_ONE/4),FX32_ONE-(FX32_ONE/4),	FX32_ONE-(FX32_ONE/4) },
	{ FX32_ONE-(FX32_ONE/5),FX32_ONE-(FX32_ONE/5),	FX32_ONE-(FX32_ONE/5) },
	{ FX32_ONE-(FX32_ONE/7),FX32_ONE-(FX32_ONE/7),	FX32_ONE-(FX32_ONE/7) },
};

//--------------------------------------------------------------
/**
 * @brief	拡大縮小
 *
 * @param	wk		ROULETTE_OBJワークのポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------
void RouletteObj_SetScaleAffine( ROULETTE_OBJ* wk, u32 no )
{
	u8 tbl_no;
	tbl_no = no;

#if 0
	if( sys.cont & PAD_BUTTON_L ){
		//拡大
		if( sys.cont & PAD_KEY_UP ){
			tbl_no = 1;
		}else if( sys.cont & PAD_KEY_LEFT ){
			tbl_no = 2;
		}else if( sys.cont & PAD_KEY_DOWN ){
			tbl_no = 3;
		}else if( sys.cont & PAD_KEY_RIGHT ){
			tbl_no = 4;
		}else{
			tbl_no = 0;
		}
	}else{
		//縮小
		if( sys.cont & PAD_KEY_UP ){
			tbl_no = 5;
		}else if( sys.cont & PAD_KEY_LEFT ){
			tbl_no = 6;
		}else if( sys.cont & PAD_KEY_DOWN ){
			tbl_no = 7;
		}else if( sys.cont & PAD_KEY_RIGHT ){
			tbl_no = 8;
		}else{
			tbl_no = 0;
		}
	}
#endif

	//CLACT_SetScaleAffine( wk->p_clact, &scale_tbl[tbl_no], CLACT_AFFINE_NORMAL );
	CLACT_SetScaleAffine( wk->p_clact, &scale_tbl[tbl_no], CLACT_AFFINE_DOUBLE );

	return;
}

//--------------------------------------------------------------
/**
 * @brief	オートアニメフラグ設定
 *
 * @param	wk		ROULETTE_OBJワークのポインタ
 * @param	flag	0=非アニメ、1=アニメ
 *
 * @retval	none
 */
//--------------------------------------------------------------
void RouletteObj_SetAnmFlag( ROULETTE_OBJ* wk, u8 flag )
{
	CLACT_SetAnmFlag( wk->p_clact, flag );
	return;
}


