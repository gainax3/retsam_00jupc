//==============================================================================================
/**
 * @file	scratch_obj.c
 * @brief	「スクラッチ」OBJ(カーソル、アイコン)
 * @author	Satoshi Nohara
 * @date	2007.12.11
 */
//==============================================================================================
#include "common.h"
#include "src_os_print.h"
#include "system/arc_tool.h"
#include "system/arc_util.h"
#include "poketool/pokeicon.h"

#include "scratch_sys.h"
#include "scratch_clact.h"
#include "scratch_obj.h"


//==============================================================================================
//
//	構造体宣言
//
//==============================================================================================
//OBJ(カーソル、アイコン)
struct _SCRATCH_OBJ{
	u16	pause;						//ポーズフラグ
	u8	disp;						//DISP_MAIN,DISP_SUB
	u8	dummy;
	u32 work;						//汎用ワーク
	u32 counter;					//カウンター
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
SCRATCH_OBJ* ScratchObj_Create( SCRATCH_CLACT* scratch_clact, u32 data_no, u32 anm_no, u16 x, u16 y, u8 disp, u32 bg_pri, u32 pri );
void* ScratchObj_Delete( SCRATCH_OBJ* wk );
void ScratchObj_Vanish( SCRATCH_OBJ* wk, int flag );
void ScratchObj_Priority( SCRATCH_OBJ* wk, u32 Priority );
void ScratchObj_Pause( SCRATCH_OBJ* wk, int flag );
void ScratchObj_SetObjPos( SCRATCH_OBJ* wk, int x, int y );
void ScratchObj_GetObjPos( SCRATCH_OBJ* wk, int* x, int* y );
void ScratchObj_AnmChg( SCRATCH_OBJ* wk, u32 num );
void ScratchObj_IconPalChg( SCRATCH_OBJ* wk, POKEMON_PARAM* pp );
void ScratchObj_SetScaleAffine( SCRATCH_OBJ* wk, u32 no );
void ScratchObj_SetScaleAffine2( SCRATCH_OBJ* wk, u8 no );
void ScratchObj_SetScaleAffineTbl( SCRATCH_OBJ* wk, VecFx32* tbl );
void ScratchObj_PaletteNoChg( SCRATCH_OBJ* wk, u32 pltt_num );
u32 ScratchObj_PaletteNoGet( SCRATCH_OBJ* wk );
void ScratchObj_SetAnmFlag( SCRATCH_OBJ* wk, int flag );
void ScratchObj_MosaicSet( SCRATCH_OBJ* wk, BOOL flag );
BOOL ScratchObj_AnmActiveCheck( SCRATCH_OBJ* wk );


//==============================================================================================
//
//	関数
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	カーソルOBJ作成
 *
 * @param	scratch_clact	SCRATCH_CLACT型のポインタ
 * @param	data_index		データindex
 * @param	heapID			ヒープID
 *
 * @retval	"SCRATCH_OBJワークへのポインタ"
 */
//--------------------------------------------------------------
SCRATCH_OBJ* ScratchObj_Create( SCRATCH_CLACT* scratch_clact, u32 data_no, u32 anm_no, u16 x, u16 y, u8 disp, u32 bg_pri, u32 pri )
{
	SCRATCH_OBJ* wk;
	VecFx32	vec;

	wk = sys_AllocMemory( HEAPID_SCRATCH, sizeof(SCRATCH_OBJ) );		//メモリ確保
	memset( wk, 0, sizeof(SCRATCH_OBJ) );

	wk->p_clact = ScratchClact_SetActor( scratch_clact, data_no, anm_no, bg_pri, pri, disp );
	wk->disp	= disp;

#if 0
	//初期カーソル座標をセット
	vec.x = (x * FX32_ONE);
	vec.y = (y * FX32_ONE);
	CLACT_SetMatrix( wk->p_clact, &vec );
#endif
	OS_Printf( "x = %d\n", x );
	OS_Printf( "y = %d\n", y );
	ScratchObj_SetObjPos( wk, x, y );

	return wk;
}

//--------------------------------------------------------------
/**
 * @brief	カーソルOBJワーク削除
 *
 * @param	wk		SCRATCH_OBJワークのポインタ
 *
 * @retval	"NULL"
 */
//--------------------------------------------------------------
void* ScratchObj_Delete( SCRATCH_OBJ* wk )
{
	CLACT_Delete( wk->p_clact );

	//SCRATCH_OBJのメンバでメモリ確保したものがあったら開放する

	sys_FreeMemoryEz( wk );
	return NULL;
}

//--------------------------------------------------------------
/**
 * @brief	バニッシュ操作
 *
 * @param	wk		SCRATCH_OBJ型のポインタ
 * @param	flag	0:非描画 1:レンダラ描画	
 *
 * @return	none
 */
//--------------------------------------------------------------
void ScratchObj_Vanish( SCRATCH_OBJ* wk, int flag )
{
	CLACT_SetDrawFlag( wk->p_clact, flag );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	プライオリティ操作
 *
 * @param	wk			SCRATCH_OBJ型のポインタ
 * @param	Priority	プライオリティ
 *
 * @return	none
 */
//--------------------------------------------------------------
void ScratchObj_Priority( SCRATCH_OBJ* wk, u32 Priority )
{
	CLACT_DrawPriorityChg( wk->p_clact, Priority );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	ポーズ
 *
 * @param	wk		SCRATCH_OBJ型のポインタ
 * @param	flag	0:ポーズ解除 1:ポーズ
 *
 * @return	none
 */
//--------------------------------------------------------------
void ScratchObj_Pause( SCRATCH_OBJ* wk, int flag )
{
	wk->pause = flag;
	return;
}

//--------------------------------------------------------------
/**
 * @brief	カーソル位置をセット
 *
 * @param	wk		SCRATCH_OBJ型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
void ScratchObj_SetObjPos( SCRATCH_OBJ* wk, int x, int y )
{
	VecFx32	vec;

	vec.x = (x * FX32_ONE);
	vec.y = (y * FX32_ONE);

#if 1
	if( wk->disp == DISP_SUB ){
		//vec.y += SUB_SURFACE_Y;				//サブ画面
		vec.y += SCRATCH_SUB_ACTOR_DISTANCE;	//サブ画面
		//OS_Printf( "サブ画面です！\n" );
	}
#endif

	//座標を設定
	CLACT_SetMatrix( wk->p_clact, &vec );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	カーソル位置を取得
 *
 * @param	wk		SCRATCH_OBJ型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
void ScratchObj_GetObjPos( SCRATCH_OBJ* wk, int* x, int* y )
{
	const VecFx32* vec;

	vec = CLACT_GetMatrix( wk->p_clact );

	*x = (vec->x / FX32_ONE);
	*y = (vec->y / FX32_ONE);
	return;
}

//--------------------------------------------------------------
/**
 * @brief	アニメ切り替え
 *
 * @param	wk		SCRATCH_OBJワークのポインタ
 *
 * @retval	"NULL"
 */
//--------------------------------------------------------------
void ScratchObj_AnmChg( SCRATCH_OBJ* wk, u32 num )
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
 * @param	wk		SCRATCH_OBJワークのポインタ
 * @param	pp		POKEMON_PARAMワークのポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------
void ScratchObj_IconPalChg( SCRATCH_OBJ* wk, POKEMON_PARAM* pp )
{
	//パレット切り替え
	//CLACT_PaletteOffsetChg関数の結果にパレットの転送先先頭パレットナンバーを加算
	CLACT_PaletteOffsetChgAddTransPlttNo( wk->p_clact, PokeIconPalNumGetByPP(pp) );
	return;
}

//--------------------------------------------------------------
///	拡大率
//--------------------------------------------------------------
static const VecFx32 scale_tbl[] =
{
	{ FX32_ONE,				FX32_ONE,				FX32_ONE },				//等倍
	
	{ FX32_ONE+(FX32_ONE/5),FX32_ONE+(FX32_ONE/5),	FX32_ONE+(FX32_ONE/5) },
	{ FX32_ONE+(FX32_ONE/4),FX32_ONE+(FX32_ONE/4),	FX32_ONE+(FX32_ONE/4) },
	{ FX32_ONE+(FX32_ONE/3),FX32_ONE+(FX32_ONE/3),	FX32_ONE+(FX32_ONE/3) },
	{ FX32_ONE+(FX32_ONE/2),FX32_ONE+(FX32_ONE/2),	FX32_ONE+(FX32_ONE/2) },


	{ FX32_ONE+(FX32_ONE)-(FX32_ONE/5),	FX32_ONE+(FX32_ONE)-(FX32_ONE/5),FX32_ONE+(FX32_ONE)-(FX32_ONE/5) },
	//{ FX32_ONE+(FX32_ONE),	FX32_ONE+(FX32_ONE),	FX32_ONE+(FX32_ONE) },



	//{ FX32_ONE*2,			FX32_ONE*2,				FX32_ONE*2	},
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
 * @param	wk		SCRATCH_OBJワークのポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------
void ScratchObj_SetScaleAffine( SCRATCH_OBJ* wk, u32 no )
{
	u8 tbl_no;
	tbl_no = no;

	//CLACT_SetScaleAffine( wk->p_clact, &scale_tbl[tbl_no], CLACT_AFFINE_NORMAL );
	//CLACT_SetScaleAffine( wk->p_clact, &scale_tbl[tbl_no], CLACT_AFFINE_DOUBLE );	//倍角

	CLACT_SetScaleAffine( wk->p_clact, &scale_tbl[0], CLACT_AFFINE_NORMAL );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	拡大縮小
 *
 * @param	wk		SCRATCH_OBJワークのポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------
void ScratchObj_SetScaleAffine2( SCRATCH_OBJ* wk, u8 no )
{
	VecFx32 scale_tbl;

	scale_tbl.x = FX32_ONE + (FX32_ONE / no);
	scale_tbl.y = FX32_ONE + (FX32_ONE / no);
	scale_tbl.z = FX32_ONE + (FX32_ONE / no);

	//CLACT_SetScaleAffine( wk->p_clact, tbl, CLACT_AFFINE_DOUBLE );	//倍角
	CLACT_SetScaleAffine( wk->p_clact, (const VecFx32*)&scale_tbl, CLACT_AFFINE_DOUBLE );	//倍角
	return;
}

//--------------------------------------------------------------
/**
 * @brief	拡大縮小
 *
 * @param	wk		SCRATCH_OBJワークのポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------
void ScratchObj_SetScaleAffineTbl( SCRATCH_OBJ* wk, VecFx32* tbl )
{
	CLACT_SetScaleAffine( wk->p_clact, tbl, CLACT_AFFINE_DOUBLE );	//倍角
	return;
}

//--------------------------------------------------------------
/**
 * @brief	パレットNo
 *
 * @param	wk			SCRATCH_OBJワークのポインタ
 * @param	pltt_num	パレットNo
 *
 * @retval	none
 */
//--------------------------------------------------------------
void ScratchObj_PaletteNoChg( SCRATCH_OBJ* wk, u32 pltt_num )
{
	//CLACT_PaletteNoChg( wk->p_clact, pltt_num );
	
	CLACT_PaletteNoChgAddTransPlttNo( wk->p_clact, pltt_num );
	
	//CLACT_PaletteOffsetChg( wk->p_clact, pltt_num );

	return;
}

//--------------------------------------------------------------
/**
 * @brief	パレットNo取得
 *
 * @param	wk			SCRATCH_OBJワークのポインタ
 * @param	pltt_num	パレットNo
 *
 * @retval	"パレットNo"
 */
//--------------------------------------------------------------
u32 ScratchObj_PaletteNoGet( SCRATCH_OBJ* wk )
{
	return CLACT_PaletteNoGet( wk->p_clact );
}

//--------------------------------------------------------------
/**
 * @brief	オートアニメフラグのON,OFF
 *
 * @param	wk			SCRATCH_OBJワークのポインタ
 * @param	flag		0=非アニメ、1=オートアニメ
 *
 * @retval	none
 */
//--------------------------------------------------------------
void ScratchObj_SetAnmFlag( SCRATCH_OBJ* wk, int flag )
{
	CLACT_SetAnmFlag( wk->p_clact, flag );
	CLACT_SetAnmFrame( wk->p_clact, FX32_ONE );
	CLACT_AnmChg( wk->p_clact, CLACT_AnmGet(wk->p_clact) );	//アニメーションのシーケンスをチェンジ
	return;
}

//--------------------------------------------------------------
/**
 * @brief	モザイク設定
 *
 * @param	wk			SCRATCH_OBJワークのポインタ
 * @param	flag		TRUE=ON、FALSE=OFF
 *
 * @retval	none
 */
//--------------------------------------------------------------
void ScratchObj_MosaicSet( SCRATCH_OBJ* wk, BOOL flag )
{
	//幅0-15(0は通常表示)
	//モザイクの幅を設定
	//G2_SetOBJMosaicSize( task_w->mosaic.x, task_w->mosaic.x );

	CLACT_MosaicSet( wk->p_clact, flag );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	アニメーション中かチェック
 *
 * @param	wk		SCRATCH_OBJ型のポインタ
 *
 * @retval	TRUE	アニメーション中
 * @retval	FALSE	ストップ
 */
//--------------------------------------------------------------
BOOL ScratchObj_AnmActiveCheck( SCRATCH_OBJ* wk )
{
	return CLACT_AnmActiveCheck( wk->p_clact );
}


