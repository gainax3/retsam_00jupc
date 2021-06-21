//******************************************************************************
/**
 * 
 * @file	fldeff_ripple.c
 * @brief	フィールドOBJ　水波紋
 * @author	kagaya
 * @data	05.07.13
 *
 */
//******************************************************************************
#include "common.h"
#include "fieldsys.h"
#include "field_effect.h"
#include "fieldobj.h"
#include "fldeff_ripple.h"

//==============================================================================
//	define
//==============================================================================
//#define DEBUG_RIPPLE_WRITE_OFF	//定義で水波紋描画OFF

#define RIPPLE_DRAW_Y_OFFSET (-FX32_ONE*(8))		///<水波紋描画オフセットY軸
#define RIPPLE_DRAW_Z_OFFSET (FX32_ONE*(1))

#define RIPPLE_ON_FRAME (30)
#define RIPPLE_OFF_FRAME (RIPPLE_ON_FRAME+30)

//==============================================================================
//	typedef struct
//==============================================================================
//--------------------------------------------------------------
///	FE_RIPPLE型
//--------------------------------------------------------------
typedef struct _TAG_FE_RIPPLE * FE_RIPPLE_PTR;

//--------------------------------------------------------------
///	FE_RIPPLE構造体
//--------------------------------------------------------------
typedef struct _TAG_FE_RIPPLE
{
	FE_SYS *fes;
	FRO_MDL m_rmdl;
	FRO_ANM m_ranm;
}FE_RIPPLE;

#define FE_RIPPLE_SIZE (sizeof(FE_RIPPLE)) ///<FE_RIPPLEサイズ

//--------------------------------------------------------------
///	RIPPLE_ADD_H構造体
//--------------------------------------------------------------
typedef struct
{
	FE_SYS *fes;					///<FE_SYS_PTR
	FE_RIPPLE_PTR ripple;		///<FE_RIPPLE_PTR
}RIPPLE_ADD_H;

#define RIPPLE_ADD_H_SIZE (sizeof(RIPPLE_ADD_H)) ///<RIPPLE_ADD_Hサイズ

//--------------------------------------------------------------
///	RIPPLE_WORK構造体
//--------------------------------------------------------------
typedef struct
{
	FRO_ANM ranm;
	FRO_OBJ robj;
	RIPPLE_ADD_H head;						///<追加時のRIPPLE_ADD_H
}RIPPLE_WORK;

#define RIPPLE_WORK_SIZE (sizeof(RIPPLE_WORK))	///<RIPPLE_WORKサイズ

//==============================================================================
//	プロトタイプ
//==============================================================================
static void Ripple_GraphicInit( FE_RIPPLE_PTR ripple );
static void Ripple_GraphicDelete( FE_RIPPLE_PTR ripple );

static const EOA_H_NPP DATA_EoaH_Ripple;

//==============================================================================
//	水波紋　システム
//==============================================================================
//--------------------------------------------------------------
/**
 * 水波紋初期化
 * @param	fes		FE_SYS_PTR
 * @retval	FE_RIPPLE_PTR	FE_RIPPLE_PTR
 */
//--------------------------------------------------------------
void * FE_Ripple_Init( FE_SYS *fes )
{
	FE_RIPPLE_PTR ripple;
	
	ripple = FE_AllocClearMemory( fes, FE_RIPPLE_SIZE, ALLOC_FR, 0 );
	ripple->fes = fes;
	
	Ripple_GraphicInit( ripple );
	return( ripple );
}

//--------------------------------------------------------------
/**
 * 水波紋削除
 * @param	ripple		FE_RIPPLE_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
void FE_Ripple_Delete( void *work )
{
	FE_RIPPLE_PTR ripple = work;
	Ripple_GraphicDelete( ripple );
	FE_FreeMemory( ripple );
}

//==============================================================================
//	水波紋　グラフィック
//==============================================================================
//--------------------------------------------------------------
/**
 * 水波紋 グラフィック初期化
 * @param	ripple	FE_RIPPLE_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Ripple_GraphicInit( FE_RIPPLE_PTR rip )
{
	FE_FROMdl_ResSetArcLoadTexTrans(
		rip->fes, &rip->m_rmdl, 0, NARC_fldeff_ripple_nsbmd, ALLOC_FR );
	//メインリソースのみロード
	FE_FROAnm_AnmResSetArcLoad(
		rip->fes, &rip->m_ranm, 0, NARC_fldeff_ripple_nsbtp, ALLOC_FR );
}

//--------------------------------------------------------------
/**
 * 水波紋 グラフィック削除
 * @param	ripple	FE_RIPPLE_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Ripple_GraphicDelete( FE_RIPPLE_PTR rip )
{
	FRO_MDL_DeleteAll( &rip->m_rmdl );
	FRO_ANM_DeleteAll( &rip->m_ranm );
}

//==============================================================================
//	水波紋　EOA
//==============================================================================
//--------------------------------------------------------------
/**
 * 水波紋追加
 * @param	fldobj	対象となるFIELD_OBJ_PTR
 * @param	gx		表示するグリッドX座標
 * @param	gz		表示するグリッドZ座標
 * @retval	EOA_PTR	水波紋EOA_PTR
 */
//--------------------------------------------------------------
void FE_FldOBJRippleSet( FIELD_OBJ_PTR fldobj, int gx, int gy, int gz )
{
	int pri,param;
	RIPPLE_ADD_H head;
	VecFx32 mtx;
	
	head.fes = FE_FieldOBJ_FE_SYS_Get( fldobj );
	head.ripple = FE_EffectWorkGet( head.fes, FE_FLD_RIPPLE );
	
	FieldOBJTool_GridCenterPosGet( gx, gz, &mtx );
	mtx.z += H_GRID_SIZE( gy ) + RIPPLE_DRAW_Z_OFFSET;
	
	mtx.y = H_GRID_SIZE_FX32( FieldOBJ_OldPosGY_Get(fldobj) ) + RIPPLE_DRAW_Y_OFFSET;
	
	param = 0;
	pri = FieldOBJ_TCBPriGet( fldobj, FLDOBJ_TCBPRI_OFFS_AFTER );
	FE_EoaAddNpp( head.fes, &DATA_EoaH_Ripple, &mtx, param, &head,pri );
}

//--------------------------------------------------------------
/**
 * EOA 水波紋　初期化
 * @param	eoa		EOA_PTR
 * @param	wk		eoa work *
 * @retval	int		TRUE=正常終了。FALSE=異常終了
 */
//--------------------------------------------------------------
static int EoaRipple_Init( EOA_PTR eoa, void *wk )
{
	RIPPLE_WORK *work = wk;
	const RIPPLE_ADD_H *head = EOA_AddPtrGet( eoa );
	
	work->head = *head;
	FE_FROAnm_AnmResObjInitSame( work->head.fes, &work->ranm,
			&work->head.ripple->m_rmdl, &work->head.ripple->m_ranm, 0 );
	FRO_OBJ_InitAll( &work->robj, &work->head.ripple->m_rmdl, &work->ranm );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * EOA 水波紋　削除
 * @param	eoa		EOA_PTR
 * @param	wk		eoa work *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaRipple_Delete( EOA_PTR eoa, void *wk )
{
	RIPPLE_WORK *work = wk;
	FRO_ANM_DeleteAll( &work->ranm );
}

//--------------------------------------------------------------
/**
 * EOA 水波紋　動作
 * @param	eoa		EOA_PTR
 * @param	wk		eoa work *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaRipple_Move( EOA_PTR eoa, void *wk )
{
	RIPPLE_WORK *work = wk;
	
	if( FRO_ANM_AnmEndCheck(&work->ranm) == TRUE ){
		FE_EoaDelete( eoa );
		return;
	}
	
	FRO_ANM_Play( &work->ranm, FX32_ONE, ANMLOOP_OFF );
}

//--------------------------------------------------------------
/**
 * EOA 水波紋　描画
 * @param	eoa		EOA_PTR
 * @param	wk		eoa work *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaRipple_Draw( EOA_PTR eoa, void *wk )
{
	VecFx32 pos;
	RIPPLE_WORK *work = wk;
	EOA_MatrixGet( eoa, &pos );
	FRO_OBJ_DrawPos( &work->robj, &pos );
}

//==============================================================================
//	data
//==============================================================================
//--------------------------------------------------------------
///	水波紋EOA_H
//--------------------------------------------------------------
static const EOA_H_NPP DATA_EoaH_Ripple =
{
	RIPPLE_WORK_SIZE,
	EoaRipple_Init,
	EoaRipple_Delete,
	EoaRipple_Move,
	EoaRipple_Draw,
};

