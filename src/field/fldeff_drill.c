//******************************************************************************
/**
 * 
 * @file	fldeff_drill.c
 * @brief	フィールドOBJ　ドリル
 * @author	k.ohno
 * @data	06.05.05
 *
 */
//******************************************************************************
#include "common.h"
#include "fieldsys.h"
#include "field_effect.h"
#include "fieldobj.h"
#include "fldeff_drill.h"

//==============================================================================
//	define
//==============================================================================

#define DRILL_DRAW_OFFSET (16*FX32_ONE)

#define DRILL_ON_FRAME (30)
#define DRILL_OFF_FRAME (DRILL_ON_FRAME+30)

//==============================================================================
//	typedef struct
//==============================================================================
//--------------------------------------------------------------
///	FE_DRILL_PTR定義
//--------------------------------------------------------------
typedef struct _TAG_FE_DRILL * FE_DRILL_PTR;

//--------------------------------------------------------------
///	FE_DRILL構造体
//--------------------------------------------------------------
typedef struct _TAG_FE_DRILL
{
	FE_SYS *fes;
	FRO_MDL m_rmdl;
	FRO_ANM m_ranm;
}FE_DRILL;

#define FE_DRILL_SIZE (sizeof(FE_DRILL)) ///<FE_DRILLサイズ

//--------------------------------------------------------------
///	DRILL_ADD_H構造体
//--------------------------------------------------------------
typedef struct
{
	FE_SYS *fes;					///<FE_SYS_PTR
	FE_DRILL_PTR drill;		///<FE_DRILL_PTR
}DRILL_ADD_H;

#define DRILL_ADD_H_SIZE (sizeof(DRILL_ADD_H)) ///<DRILL_ADD_Hサイズ

//--------------------------------------------------------------
///	DRILL_WORK構造体
//--------------------------------------------------------------
typedef struct
{
	FRO_ANM ranm;
	FRO_OBJ robj;
	DRILL_ADD_H head;						///<追加時のDRILL_ADD_H
}DRILL_WORK;

#define DRILL_WORK_SIZE (sizeof(DRILL_WORK))	///<DRILL_WORKサイズ

//==============================================================================
//	プロトタイプ
//==============================================================================
static void Drill_GraphicInit( FE_DRILL_PTR drill );
static void Drill_GraphicDelete( FE_DRILL_PTR drill );

static const EOA_H_NPP DATA_EoaH_Drill;

//==============================================================================
//	ドリル　システム
//==============================================================================
//--------------------------------------------------------------
/**
 * ドリル初期化
 * @param	fes		FE_SYS_PTR
 * @retval	FE_DRILL_PTR	FE_DRILL_PTR
 */
//--------------------------------------------------------------
void * FE_Drill_Init( FE_SYS *fes )
{
	FE_DRILL_PTR drill;
	
	drill = FE_AllocClearMemory( fes, FE_DRILL_SIZE, ALLOC_FR, 0 );
	drill->fes = fes;
	
	Drill_GraphicInit( drill );
	return( drill );
}

//--------------------------------------------------------------
/**
 * ドリル削除
 * @param	drill		FE_DRILL_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
void FE_Drill_Delete( void *work )
{
	FE_DRILL_PTR drill = work;
	Drill_GraphicDelete( drill );
	FE_FreeMemory( drill );
}

//==============================================================================
//	ドリル	パーツ
//==============================================================================

//==============================================================================
//	ドリル　グラフィック
//==============================================================================
//--------------------------------------------------------------
/**
 * ドリル グラフィック初期化
 * @param	drill	FE_DRILL_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Drill_GraphicInit( FE_DRILL_PTR drill )
{
	FE_FROMdl_ResSetArcLoadTexTrans(
		drill->fes, &drill->m_rmdl, 0, NARC_fldeff_ug_ef_drill_nsbmd, ALLOC_FR );
	//メインリソースのみロード
	FE_FROAnm_AnmResSetArcLoad(
		drill->fes, &drill->m_ranm, 0, NARC_fldeff_ug_ef_drill_nsbtp, ALLOC_FR );
}

//--------------------------------------------------------------
/**
 * ドリル グラフィック削除
 * @param	drill	FE_DRILL_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Drill_GraphicDelete( FE_DRILL_PTR drill )
{
	FRO_MDL_DeleteAll( &drill->m_rmdl );
	FRO_ANM_DeleteAll( &drill->m_ranm );
}

//==============================================================================
//	ドリルEOA
//==============================================================================
//--------------------------------------------------------------
/**
 * ドリル追加
 * @param	fldobj	対象となるFIELD_OBJ_PTR
 * @param	gx		表示するグリッドX座標
 * @param	gz		表示するグリッドZ座標
 * @retval	EOA_PTR	水波紋EOA_PTR
 */
//--------------------------------------------------------------
void FE_FldOBJDrillSet( FIELD_OBJ_PTR fldobj, int gx, int gy, int gz )
{
	int pri,param,dir;
	DRILL_ADD_H head;
	VecFx32 mtx;
	
	head.fes = FE_FieldOBJ_FE_SYS_Get( fldobj );
	head.drill = FE_EffectWorkGet( head.fes, FE_UG_DRILL );


    FieldOBJ_VecPosGet( fldobj, &mtx );
    dir = FieldOBJ_DirDispGet(fldobj);
    switch(dir){
      case DIR_UP:
        mtx.z -= DRILL_DRAW_OFFSET;
        break;
      case DIR_DOWN:
        mtx.z += DRILL_DRAW_OFFSET;
        break;
      case DIR_RIGHT:
        mtx.x += DRILL_DRAW_OFFSET;
        break;
      case DIR_LEFT:
        mtx.x -= DRILL_DRAW_OFFSET;
        break;
    }

    param = 0;
	pri = FieldOBJ_TCBPriGet( fldobj, FLDOBJ_TCBPRI_OFFS_AFTER );
	FE_EoaAddNpp( head.fes, &DATA_EoaH_Drill, &mtx, param, &head,pri );
}

//--------------------------------------------------------------
/**
 * EOA ドリル　初期化
 * @param	eoa		EOA_PTR
 * @param	wk		eoa work *
 * @retval	int		TRUE=正常終了。FALSE=異常終了
 */
//--------------------------------------------------------------
static int EoaDrill_Init( EOA_PTR eoa, void *wk )
{
	DRILL_WORK *work = wk;
	const DRILL_ADD_H *head = EOA_AddPtrGet( eoa );
	
	work->head = *head;
	FE_FROAnm_AnmResObjInitSame( work->head.fes, &work->ranm,
			&work->head.drill->m_rmdl, &work->head.drill->m_ranm, 0 );
	FRO_OBJ_InitAll( &work->robj, &work->head.drill->m_rmdl, &work->ranm );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * EOA ドリル　削除
 * @param	eoa		EOA_PTR
 * @param	wk		eoa work *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaDrill_Delete( EOA_PTR eoa, void *wk )
{
	DRILL_WORK *work = wk;
	FRO_ANM_DeleteAll( &work->ranm );
}

//--------------------------------------------------------------
/**
 * EOA ドリル　動作
 * @param	eoa		EOA_PTR
 * @param	wk		eoa work *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaDrill_Move( EOA_PTR eoa, void *wk )
{
	DRILL_WORK *work = wk;
	
	if( FRO_ANM_AnmEndCheck(&work->ranm) == TRUE ){
		FE_EoaDelete( eoa );
		return;
	}
	
	FRO_ANM_Play( &work->ranm, FX32_ONE, ANMLOOP_OFF );
}

//--------------------------------------------------------------
/**
 * EOA ドリル　描画
 * @param	eoa		EOA_PTR
 * @param	wk		eoa work *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaDrill_Draw( EOA_PTR eoa, void *wk )
{
	VecFx32 pos;
	DRILL_WORK *work = wk;
	EOA_MatrixGet( eoa, &pos );
	FRO_OBJ_DrawPos( &work->robj, &pos );
}

//==============================================================================
//	data
//==============================================================================
//--------------------------------------------------------------
///	ドリルEOA_H
//--------------------------------------------------------------
static const EOA_H_NPP DATA_EoaH_Drill =
{
	DRILL_WORK_SIZE,
	EoaDrill_Init,
	EoaDrill_Delete,
	EoaDrill_Move,
	EoaDrill_Draw,
};

