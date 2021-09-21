//******************************************************************************
/**
 * 
 * @file	fldeff_trapsmog.c
 * @brief	フィールドOBJ　煙
 * @author	k.ohno
 * @data	06.05.05
 *
 */
//******************************************************************************
#include "common.h"
#include "fieldsys.h"
#include "field_effect.h"
#include "fieldobj.h"
#include "fldeff_trapsmog.h"

#include "system/snd_tool.h"
#include "underground/ug_snd_def.h"

//==============================================================================
//	define
//==============================================================================

#define TRAPSMOG_DRAW_Y_OFFSET (0)
#define TRAPSMOG_DRAW_Z_OFFSET (FX32_ONE*2)

#define TRAPSMOG_ON_FRAME (30)
#define TRAPSMOG_OFF_FRAME (TRAPSMOG_ON_FRAME+30)

//==============================================================================
//	typedef struct
//==============================================================================
//--------------------------------------------------------------
///	FE_TRAPSMOG_PTR定義
//--------------------------------------------------------------
typedef struct _TAG_FE_TRAPSMOG * FE_TRAPSMOG_PTR;

//--------------------------------------------------------------
///	FE_TRAPSMOG構造体
//--------------------------------------------------------------
typedef struct _TAG_FE_TRAPSMOG
{
	FE_SYS *fes;
	FRO_MDL m_rmdl;
	FRO_ANM m_ranm;
}FE_TRAPSMOG;

#define FE_TRAPSMOG_SIZE (sizeof(FE_TRAPSMOG)) ///<FE_TRAPSMOGサイズ

//--------------------------------------------------------------
///	TRAPSMOG_ADD_H構造体
//--------------------------------------------------------------
typedef struct
{
	FE_SYS *fes;					///<FE_SYS_PTR
	FE_TRAPSMOG_PTR trapsmog;		///<FE_TRAPSMOG_PTR
}TRAPSMOG_ADD_H;

#define TRAPSMOG_ADD_H_SIZE (sizeof(TRAPSMOG_ADD_H)) ///<TRAPSMOG_ADD_Hサイズ

//--------------------------------------------------------------
///	TRAPSMOG_WORK構造体
//--------------------------------------------------------------
typedef struct
{
	FRO_ANM ranm;
	FRO_OBJ robj;
	TRAPSMOG_ADD_H head;						///<追加時のTRAPSMOG_ADD_H
}TRAPSMOG_WORK;

#define TRAPSMOG_WORK_SIZE (sizeof(TRAPSMOG_WORK))	///<TRAPSMOG_WORKサイズ

//==============================================================================
//	プロトタイプ
//==============================================================================
static void Trapsmog_GraphicInit( FE_TRAPSMOG_PTR trapsmog );
static void Trapsmog_GraphicDelete( FE_TRAPSMOG_PTR trapsmog );

static const EOA_H_NPP DATA_EoaH_Trapsmog;

//==============================================================================
//	煙　システム
//==============================================================================
//--------------------------------------------------------------
/**
 * 煙初期化
 * @param	fes		FE_SYS_PTR
 * @retval	FE_TRAPSMOG_PTR	FE_TRAPSMOG_PTR
 */
//--------------------------------------------------------------
void * FE_Trapsmog_Init( FE_SYS *fes )
{
	FE_TRAPSMOG_PTR trapsmog;
	
	trapsmog = FE_AllocClearMemory( fes, FE_TRAPSMOG_SIZE, ALLOC_FR, 0 );
	trapsmog->fes = fes;
	
	Trapsmog_GraphicInit( trapsmog );
	return( trapsmog );
}

//--------------------------------------------------------------
/**
 * 煙削除
 * @param	trapsmog		FE_TRAPSMOG_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
void FE_Trapsmog_Delete( void *work )
{
	FE_TRAPSMOG_PTR trapsmog = work;
	Trapsmog_GraphicDelete( trapsmog );
	FE_FreeMemory(trapsmog );
}

//==============================================================================
//	煙	パーツ
//==============================================================================

//==============================================================================
//	煙　グラフィック
//==============================================================================
//--------------------------------------------------------------
/**
 * 煙 グラフィック初期化
 * @param	trapsmog	FE_TRAPSMOG_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Trapsmog_GraphicInit( FE_TRAPSMOG_PTR trapsmog )
{
	FE_FROMdl_ResSetArcLoadTexTrans(
		trapsmog->fes, &trapsmog->m_rmdl, 0, NARC_fldeff_ug_ef_smoke_nsbmd, ALLOC_FR );
	//メインリソースのみロード
	FE_FROAnm_AnmResSetArcLoad(
		trapsmog->fes, &trapsmog->m_ranm, 0, NARC_fldeff_ug_ef_smoke_nsbtp, ALLOC_FR );
}

//--------------------------------------------------------------
/**
 * 煙 グラフィック削除
 * @param	trapsmog	FE_TRAPSMOG_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Trapsmog_GraphicDelete( FE_TRAPSMOG_PTR trapsmog )
{
	FRO_MDL_DeleteAll( &trapsmog->m_rmdl );
	FRO_ANM_DeleteAll( &trapsmog->m_ranm );
}

//==============================================================================
//	煙EOA
//==============================================================================
//--------------------------------------------------------------
/**
 * 煙追加
 * @param	fsys	フィールドに貼り付け 自動終了EFFECT
 * @param	gx		表示するグリッドX座標
 * @param	gz		表示するグリッドZ座標
 * @retval	none
 */
//--------------------------------------------------------------
void FE_FldOBJTrapsmogSet(  FIELDSYS_WORK *fsys, int gx, int gy, int gz )
{
	int pri,param;
	TRAPSMOG_ADD_H head;
	VecFx32 mtx;
	
	head.fes =  fsys->fes;
	head.trapsmog = FE_EffectWorkGet( head.fes, FE_UG_TRAP_SMOG );

    mtx.x = GRID_SIZE_FX32( gx ) + (FX32_ONE*8);
    mtx.y = (FX32_ONE * 10);
    mtx.z = GRID_SIZE_FX32( gz );

	param = 0;
	FE_EoaAddNpp( head.fes, &DATA_EoaH_Trapsmog, &mtx, param, &head, 0xff );
    Snd_SePlay(UGT_SE_TRAPEND);
}

//--------------------------------------------------------------
/**
 * EOA 煙　初期化
 * @param	eoa		EOA_PTR
 * @param	wk		eoa work *
 * @retval	int		TRUE=正常終了。FALSE=異常終了
 */
//--------------------------------------------------------------
static int EoaTrapsmog_Init( EOA_PTR eoa, void *wk )
{
	TRAPSMOG_WORK *work = wk;
	const TRAPSMOG_ADD_H *head = EOA_AddPtrGet( eoa );
	
	work->head = *head;
	
	FE_FROAnm_AnmResObjInitSame( work->head.fes, &work->ranm,
			&work->head.trapsmog->m_rmdl, &work->head.trapsmog->m_ranm, 0 );
	FRO_OBJ_InitAll( &work->robj, &work->head.trapsmog->m_rmdl, &work->ranm );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * EOA 煙　削除
 * @param	eoa		EOA_PTR
 * @param	wk		eoa work *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaTrapsmog_Delete( EOA_PTR eoa, void *wk )
{
	TRAPSMOG_WORK *work = wk;
	FRO_ANM_DeleteAll( &work->ranm );
}

//--------------------------------------------------------------
/**
 * EOA 煙　動作
 * @param	eoa		EOA_PTR
 * @param	wk		eoa work *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaTrapsmog_Move( EOA_PTR eoa, void *wk )
{
	TRAPSMOG_WORK *work = wk;
	
	if( FRO_ANM_AnmEndCheck(&work->ranm) == TRUE ){
		FE_EoaDelete( eoa );
		return;
	}
	
	FRO_ANM_Play( &work->ranm, FX32_ONE, ANMLOOP_OFF );
}

//--------------------------------------------------------------
/**
 * EOA 煙　描画
 * @param	eoa		EOA_PTR
 * @param	wk		eoa work *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaTrapsmog_Draw( EOA_PTR eoa, void *wk )
{
    TRAPSMOG_WORK *work = wk;
    
    {
		VecFx32 vec,scale = { FX32_ONE, FX32_ONE, FX32_ONE };
		MtxFx33 rot	  = { FX32_ONE, 0,0,0, FX32_ONE, 0,0,0,FX32_ONE};
        // カメラ逆行列を入れる
        const MtxFx43* pCamInv = NNS_G3dGlbGetInvCameraMtx();
        MtxFx33 Mat33;
		
        Rot360_to_MtxFx33(&rot, 90, 0, 0);
        MTX_Copy43To33(pCamInv, &Mat33);
        MTX_Concat33(&rot, &Mat33, &rot);
		
		EOA_MatrixGet( eoa, &vec );
		FRO_OBJ_Draw( &work->robj, &vec, &scale, &rot );
	}
}

//==============================================================================
//	data
//==============================================================================
//--------------------------------------------------------------
///	煙EOA_H
//--------------------------------------------------------------
static const EOA_H_NPP DATA_EoaH_Trapsmog =
{
	TRAPSMOG_WORK_SIZE,
	EoaTrapsmog_Init,
	EoaTrapsmog_Delete,
	EoaTrapsmog_Move,
	EoaTrapsmog_Draw,
};

