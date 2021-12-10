//******************************************************************************
/**
 * 
 * @file	fldeff_gflash.c
 * @brief	フィールドOBJ　地面フラッシュ
 * @author	k.ohno
 * @data	06.05.05
 *
 */
//******************************************************************************
#include "common.h"
#include "fieldsys.h"
#include "field_effect.h"
#include "fieldobj.h"
#include "fldeff_gflash.h"

//==============================================================================
//	define
//==============================================================================

#define GFLASH_DRAW_Y_OFFSET (0)
#define GFLASH_DRAW_Z_OFFSET (0)

#define GFLASH_ON_FRAME (30)
#define GFLASH_OFF_FRAME (GFLASH_ON_FRAME+30)

//==============================================================================
//	typedef struct
//==============================================================================
//--------------------------------------------------------------
///	FE_GFLASH型
//--------------------------------------------------------------
typedef struct _TAG_FE_GFLASH * FE_GFLASH_PTR;

//--------------------------------------------------------------
///	FE_GFLASH構造体
//--------------------------------------------------------------
typedef struct _TAG_FE_GFLASH
{
	int heap_id;
	FE_SYS *fes;
	FRO_MDL m_rmdl;
	FRO_ANM m_ranm;
}FE_GFLASH;

#define FE_GFLASH_SIZE (sizeof(FE_GFLASH)) ///<FE_GFLASHサイズ

//--------------------------------------------------------------
///	GFLASH_ADD_H構造体
//--------------------------------------------------------------
typedef struct
{
	FE_SYS *fes;					///<FE_SYS_PTR
	FE_GFLASH_PTR gflash;		///<FE_GFLASH_PTR
}GFLASH_ADD_H;

#define GFLASH_ADD_H_SIZE (sizeof(GFLASH_ADD_H)) ///<GFLASH_ADD_Hサイズ

//--------------------------------------------------------------
///	GFLASH_WORK構造体
//--------------------------------------------------------------
typedef struct
{
	FRO_ANM ranm;
	FRO_OBJ robj;
	GFLASH_ADD_H head;						///<追加時のGFLASH_ADD_H
}GFLASH_WORK;

#define GFLASH_WORK_SIZE (sizeof(GFLASH_WORK))	///<GFLASH_WORKサイズ

//==============================================================================
//	プロトタイプ
//==============================================================================
static FE_GFLASH_PTR GFlash_AllocMemory( int heap_id );
static void GFlash_FreeMemory( FE_GFLASH_PTR gflash );

static void GFlash_GraphicInit( FE_GFLASH_PTR gflash );
static void GFlash_GraphicDelete( FE_GFLASH_PTR gflash );

static const EOA_H_NPP DATA_EoaH_GFlash;

//==============================================================================
//	地面フラッシュ　システム
//==============================================================================
//--------------------------------------------------------------
/**
 * 地面フラッシュ初期化
 * @param	fes		FE_SYS_PTR
 * @param	heap_id	ヒープID
 * @retval	FE_GFLASH_PTR	FE_GFLASH_PTR
 */
//--------------------------------------------------------------
void * FE_GFlash_Init( FE_SYS *fes )
{
	FE_GFLASH_PTR gflash;
	
	gflash = FE_AllocClearMemory( fes, FE_GFLASH_SIZE, ALLOC_FR, 0 );
	gflash->fes = fes;
	
	GFlash_GraphicInit( gflash );
	return( gflash );
}

//--------------------------------------------------------------
/**
 * 地面フラッシュ削除
 * @param	gflash		FE_GFLASH_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
void FE_GFlash_Delete( void *work )
{
	FE_GFLASH_PTR gflash = work;
	GFlash_GraphicDelete( gflash );
	FE_FreeMemory( gflash );
}

//==============================================================================
//	地面フラッシュ	パーツ
//==============================================================================

//==============================================================================
//	地面フラッシュ　グラフィック
//==============================================================================
//--------------------------------------------------------------
/**
 * 地面フラッシュ グラフィック初期化
 * @param	gflash	FE_GFLASH_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
static void GFlash_GraphicInit( FE_GFLASH_PTR gflash )
{
	FE_FROMdl_ResSetArcLoadTexTrans(
		gflash->fes, &gflash->m_rmdl, 0, NARC_fldeff_ug_ef_gflash_nsbmd, ALLOC_FR );
	//メインリソースのみロード
	FE_FROAnm_AnmResSetArcLoad(
		gflash->fes, &gflash->m_ranm, 0, NARC_fldeff_ug_ef_gflash_nsbtp, ALLOC_FR );
}

//--------------------------------------------------------------
/**
 * 地面フラッシュ グラフィック削除
 * @param	gflash	FE_GFLASH_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
static void GFlash_GraphicDelete( FE_GFLASH_PTR gflash )
{
	FRO_MDL_DeleteAll( &gflash->m_rmdl );
	FRO_ANM_DeleteAll( &gflash->m_ranm );
}

//==============================================================================
//	地面フラッシュEOA
//==============================================================================
//--------------------------------------------------------------
/**
 * 地面フラッシュ追加
 * @param	fldobj	対象となるFIELD_OBJ_PTR
 * @param	gx		表示するグリッドX座標
 * @param	gz		表示するグリッドZ座標
 * @retval	none
 */
//--------------------------------------------------------------
void FE_FldOBJGFlashSet( FIELDSYS_WORK *fsys, int gx, int gz )
{
	int pri,param;
	GFLASH_ADD_H head;
	VecFx32 mtx;

	head.fes = fsys->fes;
//	head.fes = FE_FieldOBJ_FE_SYS_PTR_Get( fldobj );
	head.gflash = FE_EffectWorkGet( head.fes, FE_UG_TRAP_GFLASH );

	mtx.x = GRID_SIZE_FX32( gx ) + GRID_HALF_FX32;
	mtx.y = 0;
	mtx.z = GRID_SIZE_FX32( gz ) + GRID_HALF_FX32;

	param = 0;
//	pri = FE_FldOBJ_TCBPriGet( fldobj, 1 );
	FE_EoaAddNpp( head.fes, &DATA_EoaH_GFlash, &mtx, param, &head,0xff );
}

//--------------------------------------------------------------
/**
 * EOA 地面フラッシュ　初期化
 * @param	eoa		EOA_PTR
 * @param	wk		eoa work *
 * @retval	int		TRUE=正常終了。FALSE=異常終了
 */
//--------------------------------------------------------------
static int EoaGFlash_Init( EOA_PTR eoa, void *wk )
{
	GFLASH_WORK *work = wk;
	const GFLASH_ADD_H *head = EOA_AddPtrGet( eoa );
	
	work->head = *head;
	
	FE_FROAnm_AnmResObjInitSame( work->head.fes, &work->ranm,
			&work->head.gflash->m_rmdl, &work->head.gflash->m_ranm, 0 );
	FRO_OBJ_InitAll( &work->robj, &work->head.gflash->m_rmdl, &work->ranm );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * EOA 地面フラッシュ　削除
 * @param	eoa		EOA_PTR
 * @param	wk		eoa work *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaGFlash_Delete( EOA_PTR eoa, void *wk )
{
	GFLASH_WORK *work = wk;
	FRO_ANM_DeleteAll( &work->ranm );
}

//--------------------------------------------------------------
/**
 * EOA 地面フラッシュ　動作
 * @param	eoa		EOA_PTR
 * @param	wk		eoa work *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaGFlash_Move( EOA_PTR eoa, void *wk )
{
	GFLASH_WORK *work = wk;
	
	if( FRO_ANM_AnmEndCheck(&work->ranm) == TRUE ){
		FE_EoaDelete( eoa );
		return;
	}
	
	FRO_ANM_Play( &work->ranm, FX32_ONE, ANMLOOP_OFF );
}

//--------------------------------------------------------------
/**
 * EOA 地面フラッシュ　描画
 * @param	eoa		EOA_PTR
 * @param	wk		eoa work *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaGFlash_Draw( EOA_PTR eoa, void *wk )
{
	VecFx32 pos;
	GFLASH_WORK *work = wk;
	EOA_MatrixGet( eoa, &pos );
	FRO_OBJ_DrawPos( &work->robj, &pos );
}

//==============================================================================
//	data
//==============================================================================
//--------------------------------------------------------------
///	地面フラッシュEOA_H
//--------------------------------------------------------------
static const EOA_H_NPP DATA_EoaH_GFlash =
{
	GFLASH_WORK_SIZE,
	EoaGFlash_Init,
	EoaGFlash_Delete,
	EoaGFlash_Move,
	EoaGFlash_Draw,
};

