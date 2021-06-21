//==============================================================================================
/**
 * @file	castle_obj.c
 * @brief	「バトルキャッスル」OBJ(カーソル、アイコン)
 * @author	Satoshi Nohara
 * @date	2007.07.05
 */
//==============================================================================================
#include "common.h"
#include "src_os_print.h"
#include "system/arc_tool.h"
#include "system/arc_util.h"
#include "system/snd_tool.h"
#include "poketool/pokeicon.h"

#include "castle_sys.h"
#include "castle_clact.h"
#include "castle_obj.h"
#include "frontier/frontier_tool.h"


//==============================================================================================
//
//	構造体宣言
//
//==============================================================================================
//OBJ(カーソル、アイコン)
struct _CASTLE_OBJ{
	//u16 init_x;
	//u16 init_y;
	s16 init_x;
	s16 init_y;
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
CASTLE_OBJ* CastleObj_Create( CASTLE_CLACT* castle_clact, u32 char_no, u32 pltt_no, u32 cell_no, u32 anm_no, s16 x, s16 y, int bg_pri, const u8* anm_tbl );
void* CastleObj_Delete( CASTLE_OBJ* wk );
void CastleObj_Vanish( CASTLE_OBJ* wk, int flag );
u16 CastleObj_GetObjX( CASTLE_OBJ* wk );
u16 CastleObj_GetObjY( CASTLE_OBJ* wk );
void CastleObj_SetObjPos( CASTLE_OBJ* wk, u16 x, u16 y );
void CastleObj_AnmChg( CASTLE_OBJ* wk, u32 num );
void CastleObj_IconPalChg( CASTLE_OBJ* wk, POKEMON_PARAM* pp );
u16 CastleObj_GetAnmFrame( CASTLE_OBJ* wk );
void CastleObj_SetInitPosOffset( CASTLE_OBJ* wk, int offset_x, int offset_y );
void CastleObj_PokeIconAnmChg( CASTLE_OBJ* wk, u8 anm );
void CastleObj_PokeIconPosSet( CASTLE_OBJ* wk, u8 flag );
BOOL CastleObj_AnmActiveCheck( CASTLE_OBJ* wk );
void CastleObj_PriorityChg( CASTLE_OBJ* wk, u8 pri );
void CastleObj_SetInitXY( CASTLE_OBJ* wk, s16 x, s16 y );
void CastleObj_RankUpEff( CASTLE_OBJ* wk, u16 x, u16 y );


//==============================================================================================
//
//	関数
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	カーソルOBJ作成
 *
 * @param	castle_clact	CASTLE_CLACT型のポインタ
 * @param	data_index		データindex
 * @param	heapID			ヒープID
 *
 * @retval	"CASTLE_OBJワークへのポインタ"
 */
//--------------------------------------------------------------
CASTLE_OBJ* CastleObj_Create( CASTLE_CLACT* castle_clact, u32 char_no, u32 pltt_no, u32 cell_no, u32 anm_no, s16 x, s16 y, int bg_pri, const u8* anm_tbl )
{
	CASTLE_OBJ* wk;
	VecFx32	vec;

	wk = sys_AllocMemory( HEAPID_CASTLE, sizeof(CASTLE_OBJ) );		//メモリ確保
	memset( wk, 0, sizeof(CASTLE_OBJ) );

	wk->init_x		= x;
	wk->init_y		= y;

	wk->p_clact = CastleClact_SetActor( castle_clact, char_no, pltt_no, cell_no, anm_no, 
										0, bg_pri, DISP_MAIN );

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
 * @param	wk		CASTLE_OBJワークのポインタ
 *
 * @retval	"NULL"
 */
//--------------------------------------------------------------
void* CastleObj_Delete( CASTLE_OBJ* wk )
{
	if( wk == NULL ){
		OS_Printf( "CastleObj_Delete 引数がNULL\n" );
		GF_ASSERT( 0 );
		return NULL;
	}

	CLACT_Delete( wk->p_clact );

	//CASTLE_OBJのメンバでメモリ確保したものがあったら開放する

	sys_FreeMemoryEz( wk );
	return NULL;
}

//--------------------------------------------------------------
/**
 * @brief	バニッシュ操作
 *
 * @param	wk		CASTLE_OBJ型のポインタ
 * @param	flag	0:非描画 1:レンダラ描画	
 *
 * @return	none
 */
//--------------------------------------------------------------
void CastleObj_Vanish( CASTLE_OBJ* wk, int flag )
{
	CLACT_SetDrawFlag( wk->p_clact, flag );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	カーソルX位置を取得
 *
 * @param	wk		CASTLE_OBJ型のポインタ
 *
 * @return	"X"
 */
//--------------------------------------------------------------
u16 CastleObj_GetObjX( CASTLE_OBJ* wk )
{
	VecFx32	vec;
	vec = *( CLACT_GetMatrix(wk->p_clact) );
	return (vec.x / FX32_ONE);
}

//--------------------------------------------------------------
/**
 * @brief	カーソルY位置を取得
 *
 * @param	wk		CASTLE_OBJ型のポインタ
 *
 * @return	"X"
 */
//--------------------------------------------------------------
u16 CastleObj_GetObjY( CASTLE_OBJ* wk )
{
	VecFx32	vec;
	vec = *( CLACT_GetMatrix(wk->p_clact) );
	return (vec.y / FX32_ONE);
}

//--------------------------------------------------------------
/**
 * @brief	カーソル位置をセット
 *
 * @param	wk		CASTLE_OBJ型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
void CastleObj_SetObjPos( CASTLE_OBJ* wk, u16 x, u16 y )
{
	VecFx32	vec;

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
 * @param	wk		CASTLE_OBJワークのポインタ
 *
 * @retval	"NULL"
 */
//--------------------------------------------------------------
void CastleObj_AnmChg( CASTLE_OBJ* wk, u32 num )
{
	CLACT_SetAnmFrame( wk->p_clact, FX32_ONE );
	//CLACT_AnmFrameSet( wk->p_clact, 0 );		//アニメーションフレームをセット
	//CLACT_AnmChgCheck( wk->p_clact, num );		//アニメーションのシーケンスをチェンジする
	//必ず変更(07.11.28)
	CLACT_AnmChg( wk->p_clact, num );			//アニメーションのシーケンスをチェンジする
	return;
}

//--------------------------------------------------------------
/**
 * @brief	アイコンアニメ
 *
 * @param	wk		CASTLE_OBJワークのポインタ
 * @param	pp		POKEMON_PARAMワークのポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------
void CastleObj_IconPalChg( CASTLE_OBJ* wk, POKEMON_PARAM* pp )
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
 * @param	wk		CASTLE_OBJワークのポインタ
 *
 * @retval	"アニメフレーム"
 */
//--------------------------------------------------------------
u16 CastleObj_GetAnmFrame( CASTLE_OBJ* wk )
{
	return CLACT_AnmFrameGet( wk->p_clact );
}

//--------------------------------------------------------------
/**
 * @brief	初期座標に指定したオフセットを足して座標セット
 *
 * @param	wk		CASTLE_OBJワークのポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------
void CastleObj_SetInitPosOffset( CASTLE_OBJ* wk, int offset_x, int offset_y )
{
	CastleObj_SetObjPos( wk, wk->init_x+offset_x, wk->init_y+offset_y );
	return;
}

//--------------------------------------------------------------
/**
 * ポケモンアイコンアニメ切り替え
 *
 * @param	awk		アクターワーク
 * @param	anm		アニメ番号
 *
 * @return	none
 */
//--------------------------------------------------------------
void CastleObj_PokeIconAnmChg( CASTLE_OBJ* wk, u8 anm )
{
	Frontier_PokeIconAnmChg( wk->p_clact, anm );
	return;
}

//--------------------------------------------------------------
/**
 * ポケモンアイコンアニメ切り替え
 *
 * @param	awk		アクターワーク
 * @param	flag	1=選択しているポケモン
 *
 * @return	none
 */
//--------------------------------------------------------------
void CastleObj_PokeIconPosSet( CASTLE_OBJ* wk, u8 flag )
{
	Frontier_PokeIconPosSet( wk->p_clact, wk->init_x, wk->init_y, flag );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	アニメーション中かチェック
 *
 * @param	wk		CASTLE_OBJ型のポインタ
 *
 * @retval	TRUE	アニメーション中
 * @retval	FALSE	ストップ
 */
//--------------------------------------------------------------
BOOL CastleObj_AnmActiveCheck( CASTLE_OBJ* wk )
{
	return CLACT_AnmActiveCheck( wk->p_clact );
}

//--------------------------------------------------------------
/**
 * @brief	BG面との優先度を変更
 *
 * @param	wk		CASTLE_OBJ型のポインタ
 * @param	pri		優先度
 *
 * @retval	none
 */
//--------------------------------------------------------------
void CastleObj_PriorityChg( CASTLE_OBJ* wk, u8 pri )
{
	CLACT_BGPriorityChg( wk->p_clact, pri );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	初期XYをセット
 *
 * @param	wk		CASTLE_OBJ型のポインタ
 * @param	pri		優先度
 *
 * @retval	none
 */
//--------------------------------------------------------------
void CastleObj_SetInitXY( CASTLE_OBJ* wk, s16 x, s16 y )
{
	wk->init_x = x;
	wk->init_y = y;
	return;
}

//--------------------------------------------------------------
/**
 * @brief	ランクアップエフェクト表示
 *
 * @param	wk		CASTLE_OBJ型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
void CastleObj_RankUpEff( CASTLE_OBJ* wk, u16 x, u16 y )
{
	CastleObj_AnmChg( wk, CASTLE_ANM_RANK_UP );
	CastleObj_SetObjPos( wk, x, y );
	CastleObj_Vanish( wk, CASTLE_VANISH_OFF );				//表示
	Snd_SePlay( SEQ_SE_DP_PIRORIRO2 );
	Snd_SePlay( SEQ_SE_DP_DANSA4 );
	return;
}



