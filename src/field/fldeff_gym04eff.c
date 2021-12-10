//******************************************************************************
/**
 * 
 * @file	fldeff_gym04eff.c
 * @brief	フィールドOBJ　草ジムエフェクト
 * @author	kagaya
 * @data	05.07.13
 *
 */
//******************************************************************************
#include "common.h"
#include "fieldsys.h"
#include "field_effect.h"
#include "fieldobj.h"
#include "fldeff_gym04eff.h"

//==============================================================================
//	define
//==============================================================================
#define FUNSUI0_START_GX (1)
#define FUNSUI0_START_GZ (19)
#define FUNSUI0_SIZE_GX (4)
#define FUNSUI0_SIZE_GZ (1)
#define FUNSUI0_END_GX (FUNSUI0_START_GX+FUNSUI0_SIZE_GX)
#define FUNSUI0_END_GZ (FUNSUI0_START_GZ+FUNSUI0_SIZE_GZ)
#define FUNSUI1_START_GX (18)
#define FUNSUI1_START_GZ (19)
#define FUNSUI1_SIZE_GX (4)
#define FUNSUI1_SIZE_GZ (1)
#define FUNSUI1_END_GX (FUNSUI1_START_GX+FUNSUI1_SIZE_GX)
#define FUNSUI1_END_GZ (FUNSUI1_START_GZ+FUNSUI1_SIZE_GZ)

#define FUNSUI0_NUM_MAX (FUNSUI0_SIZE_GX*FUNSUI0_SIZE_GZ)
#define FUNSUI1_NUM_MAX (FUNSUI1_SIZE_GX*FUNSUI1_SIZE_GZ)
#define FUNSUI_NUM_MAX (FUNSUI0_NUM_MAX+FUNSUI1_NUM_MAX)

//==============================================================================
//	typedef struct
//==============================================================================
//--------------------------------------------------------------
///	FE_GYM04EFF型
//--------------------------------------------------------------
typedef struct _TAG_FE_GYM04EFF * FE_GYM04EFF_PTR;

//--------------------------------------------------------------
///	FE_GYM04EFF構造体
//--------------------------------------------------------------
typedef struct _TAG_FE_GYM04EFF
{
	FE_SYS *fes;
	FRO_MDL m_rmdl;
	FRO_ANM m_ranm_a;
	FRO_ANM m_ranm_b;
	EOA_PTR eoa_eff[FUNSUI_NUM_MAX];
}FE_GYM04EFF;

#define FE_GYM04EFF_SIZE (sizeof(FE_GYM04EFF)) ///<FE_GYM04EFFサイズ

//--------------------------------------------------------------
///	GYM04EFF_ADD_H構造体
//--------------------------------------------------------------
typedef struct
{
	FE_SYS *fes;				///<FE_SYS_PTR
	FE_GYM04EFF_PTR gym04eff;	///<FE_GYM04EFF_PTR
}GYM04EFF_ADD_H;

#define GYM04EFF_ADD_H_SIZE (sizeof(GYM04EFF_ADD_H)) ///<GYM04EFF_ADD_Hサイズ

//--------------------------------------------------------------
///	GYM04EFF_WORK構造体
//--------------------------------------------------------------
typedef struct
{
	int anm_end_del_flag;
	FRO_ANM ranm;
	FRO_OBJ robj;
	GYM04EFF_ADD_H head;						///<追加時のGYM04EFF_ADD_H
}GYM04EFF_WORK;

#define GYM04EFF_WORK_SIZE (sizeof(GYM04EFF_WORK))	///<GYM04EFF_WORKサイズ

//==============================================================================
//	プロトタイプ
//==============================================================================
static void Gym04Eff_GraphicInit( FE_GYM04EFF_PTR gym04eff );
static void Gym04Eff_GraphicDelete( FE_GYM04EFF_PTR gym04eff );

static const EOA_H_NPP DATA_EoaH_Gym04Eff;

//==============================================================================
//	草ジムエフェクト　システム
//==============================================================================
//--------------------------------------------------------------
/**
 * 草ジムエフェクト初期化
 * @param	fes		FE_SYS_PTR
 * @retval	FE_GYM04EFF_PTR	FE_GYM04EFF_PTR
 */
//--------------------------------------------------------------
void * FE_Gym04Eff_Init( FE_SYS *fes )
{
	FE_GYM04EFF_PTR gym04eff;
	
	gym04eff = FE_AllocClearMemory( fes, FE_GYM04EFF_SIZE, ALLOC_FR, 0 );
	gym04eff->fes = fes;
	
	Gym04Eff_GraphicInit( gym04eff );
	return( gym04eff );
}

//--------------------------------------------------------------
/**
 * 草ジムエフェクト削除
 * @param	gym04eff		FE_GYM04EFF_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
void FE_Gym04Eff_Delete( void *work )
{
	FE_GYM04EFF_PTR gym04eff = work;
	Gym04Eff_GraphicDelete( gym04eff );
	FE_FreeMemory( gym04eff );
}

//==============================================================================
//	草ジムエフェクト	パーツ
//==============================================================================

//==============================================================================
//	草ジムエフェクト　グラフィック
//==============================================================================
//--------------------------------------------------------------
/**
 * 草ジムエフェクト グラフィック初期化
 * @param	gym04eff	FE_GYM04EFF_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Gym04Eff_GraphicInit( FE_GYM04EFF_PTR gym04 )
{
	FE_FROMdl_ResSetArcLoadTexTrans(
		gym04->fes, &gym04->m_rmdl, 0,
		NARC_fldeff_gym04_hunsui_nsbmd, ALLOC_FR );
	
	//メインリソースのみロード
	FE_FROAnm_AnmResSetArcLoad(
		gym04->fes, &gym04->m_ranm_a, 0,
		NARC_fldeff_gym04_hunsui_a_nsbtp, ALLOC_FR );
	FE_FROAnm_AnmResSetArcLoad(
		gym04->fes, &gym04->m_ranm_b, 0,
		NARC_fldeff_gym04_hunsui_b_nsbtp, ALLOC_FR );
}

//--------------------------------------------------------------
/**
 * 草ジムエフェクト グラフィック削除
 * @param	gym04eff	FE_GYM04EFF_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Gym04Eff_GraphicDelete( FE_GYM04EFF_PTR gym04 )
{
	FRO_MDL_DeleteAll( &gym04->m_rmdl );
	FRO_ANM_DeleteAll( &gym04->m_ranm_a );
	FRO_ANM_DeleteAll( &gym04->m_ranm_b );
}

//==============================================================================
//	草ジムエフェクト　EOA
//==============================================================================
//--------------------------------------------------------------
/**
 * 草ジムエフェクト追加
 * @param	fsys	FIELDSYS_WORK *
 * @param	type	GYM04EFF_FUNSUI_LEFT等
 * @param	nothing
 */
//--------------------------------------------------------------
void FE_FldOBJGym04EffSet( FIELDSYS_WORK *fsys, int type )
{
	int x,z;
	VecFx32 pos = {0,0,0};
	FE_SYS *fes;
	FE_GYM04EFF *gym04;
	EOA_PTR *eoa;
	GYM04EFF_ADD_H head;
	
	fes = fsys->fes;
	
	if( FE_EffectRegistCheck(fes,FE_FLD_GYM04EFF) == FALSE ){
		FE_EffectRegist( fes, FE_FLD_GYM04EFF );
	}
	
	gym04 = FE_EffectWorkGet( fes, FE_FLD_GYM04EFF );
	
	head.fes = fes;
	head.gym04eff = gym04;
	
	if( type != GYM04EFF_FUNSUI_RIGHT ){
		eoa = gym04->eoa_eff;
	
		for( z = FUNSUI0_START_GZ; z < FUNSUI0_END_GZ; z++ ){
			for( x = FUNSUI0_START_GX; x < FUNSUI0_END_GX; x++, eoa++ ){
				pos.x = GRID_SIZE_FX32( x ) + GRID_HALF_FX32;
				pos.z = GRID_SIZE_FX32( z ) + GRID_HALF_FX32;
				pos.y = FieldOBJTool_GetHeightExpand( fsys, &pos, FALSE );
				*eoa = FE_EoaAddNpp(
					fes, &DATA_EoaH_Gym04Eff, &pos, 0, &head, 0 );
			}
		}
	}
	
	if( type != GYM04EFF_FUNSUI_LEFT ){
		eoa = &gym04->eoa_eff[FUNSUI0_NUM_MAX];
		
		for( z = FUNSUI1_START_GZ; z < FUNSUI1_END_GZ; z++ ){
			for( x = FUNSUI1_START_GX; x < FUNSUI1_END_GX; x++, eoa++ ){
				pos.x = GRID_SIZE_FX32( x ) + GRID_HALF_FX32;
				pos.z = GRID_SIZE_FX32( z ) + GRID_HALF_FX32;
				pos.y = FieldOBJTool_GetHeightExpand( fsys, &pos, FALSE );
				*eoa = FE_EoaAddNpp(
					fes, &DATA_EoaH_Gym04Eff, &pos, 0, &head, 0 );
			}
		}
	}
}

//--------------------------------------------------------------
/**
 * 草ジムエフェクト　噴水終了
 * @param	fes		FIELDSYS_WORK *
 * @param	type	GYM04EFF_FUNSUI_LEFT等
 * @retval	nothing
 */
//--------------------------------------------------------------
void FE_FldOBJGym04EffEndSet( FIELDSYS_WORK *fsys, int type )
{
	int i,max;
	FE_SYS *fes;
	FE_GYM04EFF *gym04;
	EOA_PTR *eoa;
	GYM04EFF_WORK *work;
	
	fes = fsys->fes;
	GF_ASSERT( FE_EffectRegistCheck(fes,FE_FLD_GYM04EFF) == TRUE );
	
	gym04 = FE_EffectWorkGet( fes, FE_FLD_GYM04EFF );
	
	if( type == GYM04EFF_FUNSUI_LEFT ){
		eoa = gym04->eoa_eff;
		max = FUNSUI0_NUM_MAX;
	}else{
		eoa = &gym04->eoa_eff[FUNSUI0_NUM_MAX];
		max = FUNSUI1_NUM_MAX;
	}
	
	for( i = 0; i < max; i++, eoa++ ){
		work = EOA_LocalWorkGet( *eoa );
		work->anm_end_del_flag = TRUE;
		FRO_ANM_AnmObjFree( &work->ranm );
		FRO_ANM_Clear( &work->ranm );
		FE_FROAnm_AnmResObjInitSame( fes, &work->ranm,
			&work->head.gym04eff->m_rmdl, &work->head.gym04eff->m_ranm_b, 0 );
		FRO_OBJ_InitAll(
			&work->robj, &work->head.gym04eff->m_rmdl, &work->ranm );
	}
}

//--------------------------------------------------------------
/**
 * EOA 草ジムエフェクト　初期化
 * @param	eoa		EOA_PTR
 * @param	wk		eoa work *
 * @retval	int		TRUE=正常終了。FALSE=異常終了
 */
//--------------------------------------------------------------
static int EoaGym04Eff_Init( EOA_PTR eoa, void *wk )
{
	GYM04EFF_WORK *work = wk;
	const GYM04EFF_ADD_H *head = EOA_AddPtrGet( eoa );
	
	work->head = *head;
	FE_FROAnm_AnmResObjInitSame( work->head.fes, &work->ranm,
			&work->head.gym04eff->m_rmdl, &work->head.gym04eff->m_ranm_a, 0 );
	FRO_OBJ_InitAll( &work->robj, &work->head.gym04eff->m_rmdl, &work->ranm );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * EOA 草ジムエフェクト　削除
 * @param	eoa		EOA_PTR
 * @param	wk		eoa work *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaGym04Eff_Delete( EOA_PTR eoa, void *wk )
{
	GYM04EFF_WORK *work = wk;
	FRO_ANM_DeleteAll( &work->ranm );
}

//--------------------------------------------------------------
/**
 * EOA 草ジムエフェクト　動作
 * @param	eoa		EOA_PTR
 * @param	wk		eoa work *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaGym04Eff_Move( EOA_PTR eoa, void *wk )
{
	ANMLOOPTYPE type = ANMLOOP_ON;
	GYM04EFF_WORK *work = wk;
	
	if( work->anm_end_del_flag == TRUE ){
		if( FRO_ANM_AnmEndCheck(&work->ranm) == TRUE ){
			FE_EoaDelete( eoa );
			return;
		}
		
		type = ANMLOOP_OFF;
	}
	
	FRO_ANM_Play( &work->ranm, FX32_ONE, type );
}

//--------------------------------------------------------------
/**
 * EOA 草ジムエフェクト　描画
 * @param	eoa		EOA_PTR
 * @param	wk		eoa work *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaGym04Eff_Draw( EOA_PTR eoa, void *wk )
{
	VecFx32 pos;
	GYM04EFF_WORK *work = wk;
	EOA_MatrixGet( eoa, &pos );
	FRO_OBJ_DrawPos( &work->robj, &pos );
}

//==============================================================================
//	data
//==============================================================================
//--------------------------------------------------------------
///	草ジムエフェクトEOA_H
//--------------------------------------------------------------
static const EOA_H_NPP DATA_EoaH_Gym04Eff =
{
	GYM04EFF_WORK_SIZE,
	EoaGym04Eff_Init,
	EoaGym04Eff_Delete,
	EoaGym04Eff_Move,
	EoaGym04Eff_Draw,
};

