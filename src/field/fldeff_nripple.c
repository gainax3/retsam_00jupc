//******************************************************************************
/**
 * 
 * @file	fldeff_nripple.c
 * @brief	フィールドOBJ　沼地沼波紋
 * @author	kagaya
 * @data	05.07.13
 *
 */
//******************************************************************************
#include "common.h"
#include "fieldsys.h"
#include "field_effect.h"
#include "fieldobj.h"
#include "fldeff_nripple.h"

//==============================================================================
//	define
//==============================================================================
//#define DEBUG_NRIPPLE_WRITE_OFF	//定義で沼波紋描画OFF

#define NRIPPLE_DRAW_Y_OFFSET (-FX32_ONE*(8))		///<沼波紋描画オフセットY軸
#define NRIPPLE_DRAW_Z_OFFSET (FX32_ONE*(0))

#define NRIPPLE_ON_FRAME (30)
#define NRIPPLE_OFF_FRAME (RIPPLE_ON_FRAME+30)

//==============================================================================
//	typedef struct
//==============================================================================
//--------------------------------------------------------------
///	FE_NRIPPLE型
//--------------------------------------------------------------
typedef struct _TAG_FE_NRIPPLE * FE_NRIPPLE_PTR;

//--------------------------------------------------------------
///	FE_NRIPPLE構造体
//--------------------------------------------------------------
typedef struct _TAG_FE_NRIPPLE
{
	FE_SYS *fes;
	FRO_MDL m_rmdl;
	FRO_ANM m_ranm;
}FE_NRIPPLE;

#define FE_NRIPPLE_SIZE (sizeof(FE_NRIPPLE)) ///<FE_NRIPPLEサイズ

//--------------------------------------------------------------
///	NRIPPLE_ADD_H構造体
//--------------------------------------------------------------
typedef struct
{
	FE_SYS *fes;					///<FE_SYS_PTR
	FE_NRIPPLE_PTR ripple;		///<FE_RIPPLE_PTR
}NRIPPLE_ADD_H;

#define NRIPPLE_ADD_H_SIZE (sizeof(RIPPLE_ADD_H)) ///<RIPPLE_ADD_Hサイズ

//--------------------------------------------------------------
///	NRIPPLE_WORK構造体
//--------------------------------------------------------------
typedef struct
{
	FRO_ANM ranm;
	FRO_OBJ robj;
	NRIPPLE_ADD_H head;						///<追加時のRIPPLE_ADD_H
}NRIPPLE_WORK;

#define NRIPPLE_WORK_SIZE (sizeof(NRIPPLE_WORK))	///<NRIPPLE_WORKサイズ

//==============================================================================
//	プロトタイプ
//==============================================================================
static void NRipple_GraphicInit( FE_NRIPPLE_PTR ripple );
static void NRipple_GraphicDelete( FE_NRIPPLE_PTR ripple );

static const EOA_H_NPP DATA_EoaH_NRipple;

//==============================================================================
//	沼波紋　システム
//==============================================================================
//--------------------------------------------------------------
/**
 * 沼波紋初期化
 * @param	fes		FE_SYS_PTR
 * @retval	FE_NRIPPLE_PTR	FE_RIPPLE_PTR
 */
//--------------------------------------------------------------
void * FE_NRipple_Init( FE_SYS *fes )
{
	FE_NRIPPLE_PTR ripple;
	
	ripple = FE_AllocClearMemory( fes, FE_NRIPPLE_SIZE, ALLOC_FR, 0 );
	ripple->fes = fes;
	
	NRipple_GraphicInit( ripple );
	return( ripple );
}

//--------------------------------------------------------------
/**
 * 沼波紋削除
 * @param	ripple		FE_NRIPPLE_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
void FE_NRipple_Delete( void *work )
{
	FE_NRIPPLE_PTR ripple = work;
	NRipple_GraphicDelete( ripple );
	FE_FreeMemory( ripple );
}

//==============================================================================
//	沼波紋	パーツ
//==============================================================================

//==============================================================================
//	沼波紋　グラフィック
//==============================================================================
//--------------------------------------------------------------
/**
 * 沼波紋 グラフィック初期化
 * @param	ripple	FE_NRIPPLE_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
static void NRipple_GraphicInit( FE_NRIPPLE_PTR rip )
{
	FE_FROMdl_ResSetArcLoadTexTrans(
		rip->fes, &rip->m_rmdl, 0, NARC_fldeff_numa_ripple_nsbmd, ALLOC_FR );
	//メインリソースのみロード
	FE_FROAnm_AnmResSetArcLoad(
		rip->fes, &rip->m_ranm, 0, NARC_fldeff_numa_ripple_nsbtp, ALLOC_FR );
}

//--------------------------------------------------------------
/**
 * 沼波紋 グラフィック削除
 * @param	ripple	FE_NRIPPLE_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
static void NRipple_GraphicDelete( FE_NRIPPLE_PTR rip )
{
	FRO_MDL_DeleteAll( &rip->m_rmdl );
	FRO_ANM_DeleteAll( &rip->m_ranm );
}

//==============================================================================
//	沼波紋　EOA
//==============================================================================
//--------------------------------------------------------------
/**
 * 沼波紋追加
 * @param	fldobj	対象となるFIELD_OBJ_PTR
 * @param	gx		表示するグリッドX座標
 * @param	gz		表示するグリッドZ座標
 * @retval	EOA_PTR	沼波紋EOA_PTR
 */
//--------------------------------------------------------------
void FE_FldOBJNRippleSet( FIELD_OBJ_PTR fldobj, int gx, int gy, int gz )
{
	int pri,param;
	NRIPPLE_ADD_H head;
	VecFx32 mtx;
	
	head.fes = FE_FieldOBJ_FE_SYS_Get( fldobj );
	head.ripple = FE_EffectWorkGet( head.fes, FE_FLD_NRIPPLE );
	
	FieldOBJTool_GridCenterPosGet( gx, gz, &mtx );
	mtx.z += H_GRID_SIZE( gy ) + NRIPPLE_DRAW_Z_OFFSET;
	
	mtx.y = H_GRID_SIZE_FX32( FieldOBJ_OldPosGY_Get(fldobj) ) + NRIPPLE_DRAW_Y_OFFSET;
	
	param = 0;
	pri = FieldOBJ_TCBPriGet( fldobj, FLDOBJ_TCBPRI_OFFS_AFTER );
	FE_EoaAddNpp( head.fes, &DATA_EoaH_NRipple, &mtx, param, &head,pri );
}

//--------------------------------------------------------------
/**
 * EOA 沼波紋　初期化
 * @param	eoa		EOA_PTR
 * @param	wk		eoa work *
 * @retval	int		TRUE=正常終了。FALSE=異常終了
 */
//--------------------------------------------------------------
static int EoaNRipple_Init( EOA_PTR eoa, void *wk )
{
	NRIPPLE_WORK *work = wk;
	const NRIPPLE_ADD_H *head = EOA_AddPtrGet( eoa );
	
	work->head = *head;
	FE_FROAnm_AnmResObjInitSame( work->head.fes, &work->ranm,
			&work->head.ripple->m_rmdl, &work->head.ripple->m_ranm, 0 );
	FRO_OBJ_InitAll( &work->robj, &work->head.ripple->m_rmdl, &work->ranm );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * EOA 沼波紋　削除
 * @param	eoa		EOA_PTR
 * @param	wk		eoa work *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaNRipple_Delete( EOA_PTR eoa, void *wk )
{
	NRIPPLE_WORK *work = wk;
	FRO_ANM_DeleteAll( &work->ranm );
}

//--------------------------------------------------------------
/**
 * EOA 沼波紋　動作
 * @param	eoa		EOA_PTR
 * @param	wk		eoa work *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaNRipple_Move( EOA_PTR eoa, void *wk )
{
	NRIPPLE_WORK *work = wk;
	
	if( FRO_ANM_AnmEndCheck(&work->ranm) == TRUE ){
		FE_EoaDelete( eoa );
		return;
	}
	
	FRO_ANM_Play( &work->ranm, FX32_ONE, ANMLOOP_OFF );
}

//--------------------------------------------------------------
/**
 * EOA 沼波紋　描画
 * @param	eoa		EOA_PTR
 * @param	wk		eoa work *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaNRipple_Draw( EOA_PTR eoa, void *wk )
{
	VecFx32 pos;
	NRIPPLE_WORK *work = wk;
	EOA_MatrixGet( eoa, &pos );
	FRO_OBJ_DrawPos( &work->robj, &pos );
}

//==============================================================================
//	data
//==============================================================================
//--------------------------------------------------------------
///	沼波紋EOA_H
//--------------------------------------------------------------
static const EOA_H_NPP DATA_EoaH_NRipple =
{
	NRIPPLE_WORK_SIZE,
	EoaNRipple_Init,
	EoaNRipple_Delete,
	EoaNRipple_Move,
	EoaNRipple_Draw,
};

