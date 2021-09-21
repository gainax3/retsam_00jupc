//******************************************************************************
/**
 * 
 * @file	fldeff_hide.c
 * @brief	フィールドOBJ隠れ蓑
 * @author	kagaya
 * @data	05.07.13
 *
 */
//******************************************************************************
#include "common.h"
#include "fieldsys.h"
#include "field_effect.h"
#include "fieldobj.h"
#include "fldeff_hide.h"

#include "pl_bugfix.h"

//==============================================================================
//	define
//==============================================================================

//==============================================================================
//	typedef struct
//==============================================================================
//--------------------------------------------------------------
///	FE_HIDE型
//--------------------------------------------------------------
typedef struct _TAG_FE_HIDE * FE_HIDE_PTR;

//--------------------------------------------------------------
///	FE_HIDE構造体
//--------------------------------------------------------------
typedef struct _TAG_FE_HIDE
{
	FE_SYS *fes;
	FRO_MDL rmdl[HIDE_MAX];
	FRO_OBJ robj[HIDE_MAX];
}FE_HIDE;

#define FE_HIDE_SIZE (sizeof(FE_HIDE)) ///<FE_HIDEサイズ

//--------------------------------------------------------------
///	HIDE_ADD_H構造体
//--------------------------------------------------------------
typedef struct
{
	HIDETYPE type;
	FE_SYS *fes;
	FE_HIDE *hide;
	FIELD_OBJ_PTR fldobj;
	FRO_OBJ *robj;
}HIDE_ADD_H;

//--------------------------------------------------------------
///	HIDE_WORK構造体
//--------------------------------------------------------------
typedef struct
{
	int seq_no;
	int vanish_sw;
	int obj_id;
	int zone_id;
	HIDE_ADD_H head;
}HIDE_WORK;

#define HIDE_WORK_SIZE (sizeof(HIDE_WORK))

//==============================================================================
//	プロトタイプ
//==============================================================================
static void Hide_GraphicInit( FE_HIDE_PTR hide );
static void Hide_GraphicDelete( FE_HIDE_PTR hide );
static FRO_OBJ * Hide_TypeOBJGet( FE_HIDE_PTR hide, HIDETYPE type );

static const EOA_H_NPP DATA_EoaH_Hide;

static const u32 DATA_3DModelArcID_Hide[HIDE_MAX];

//==============================================================================
//	隠れ蓑　システム
//==============================================================================
//--------------------------------------------------------------
/**
 * 隠れ蓑初期化
 * @param	fes		FE_SYS_PTR
 * @retval	FE_HIDE_PTR		FE_HIDE_PTR
 */
//--------------------------------------------------------------
void * FE_Hide_Init( FE_SYS *fes )
{
	FE_HIDE_PTR hide;
	
	hide = FE_AllocClearMemory( fes, FE_HIDE_SIZE, ALLOC_FR, 0 );
	hide->fes = fes;
	
	Hide_GraphicInit( hide );
	return( hide );
}

//--------------------------------------------------------------
/**
 * 隠れ蓑削除
 * @param	hide		FE_HIDE_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
void FE_Hide_Delete( void *work )
{
	FE_HIDE_PTR hide = work;
	Hide_GraphicDelete( hide );
	FE_FreeMemory( hide );
}

//==============================================================================
//	隠れ蓑　グラフィック
//==============================================================================
//--------------------------------------------------------------
/**
 * 隠れ蓑 グラフィック初期化
 * @param	hide		FE_HIDE_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Hide_GraphicInit( FE_HIDE_PTR hide )
{
	int i;
	
	for( i = 0; i < HIDE_MAX; i++ ){
		FE_FROMdl_ResSetArcLoadTexTrans(
			hide->fes, &hide->rmdl[i], 0, DATA_3DModelArcID_Hide[i], ALLOC_FR );
		FRO_OBJ_InitInMdl( &hide->robj[i], &hide->rmdl[i] );
	}
}

//--------------------------------------------------------------
/**
 * 隠れ蓑　グラフィック削除
 * @param	hide		FE_HIDE_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Hide_GraphicDelete( FE_HIDE_PTR hide )
{
	int i;
	
	for( i = 0; i < HIDE_MAX; i++ ){
		FRO_MDL_DeleteAll( &hide->rmdl[i] );
	}
}

//--------------------------------------------------------------
/**
 * 指定タイプのレンダ取得
 * @param	hide	FE_HIDE_PTR
 * @param	type	HIDETYPE
 * @param	attr	HIDE_ATTR
 * @retval	NNSG3dRenderObj 指定のレンダ
 */
//--------------------------------------------------------------
static FRO_OBJ * Hide_TypeOBJGet( FE_HIDE_PTR hide, HIDETYPE type )
{
	return( &hide->robj[type] );
}

//==============================================================================
//	隠れ蓑　EOA
//==============================================================================
//--------------------------------------------------------------
/**
 * フィールドOBJ隠れ蓑追加
 * @param	fldobj	FIELD_OBJ_PTR
 * @param	type	HIDETYPE
 * @retval	eoa		EOA_PTR
 */
//--------------------------------------------------------------
EOA_PTR FE_FldOBJHide_Add( FIELD_OBJ_PTR fldobj, HIDETYPE type )
{
	VecFx32 mtx;
	EOA_PTR eoa;
	HIDE_ADD_H head;
	FE_SYS *fes;
	
	fes = FE_FieldOBJ_FE_SYS_Get( fldobj );
	
	{
		FIELDSYS_WORK *fsys = FE_FieldSysWorkGet( fes );
		int gx = FieldOBJ_NowPosGX_Get( fldobj );
		int gz = FieldOBJ_NowPosGZ_Get( fldobj );
		FieldOBJTool_GridCenterPosGet( gx, gz, &mtx );
		FieldOBJTool_GetHeight( fsys, &mtx );
	}
	
	{
		head.type = type;
		head.fes = fes;
		head.hide = FE_EffectWorkGet( fes, FE_FLD_HIDE );
		head.fldobj = fldobj;
		head.robj = Hide_TypeOBJGet( head.hide, type );
	}
	
	{
		int pri = FieldOBJ_TCBPriGet( fldobj, FLDOBJ_TCBPRI_OFFS_AFTER );
		eoa = FE_EoaAddNpp( fes, &DATA_EoaH_Hide, &mtx, 0, &head, pri );
	}
	
	return( eoa );
}

//--------------------------------------------------------------
/**
 * EOA 隠れ蓑　初期化
 * @param	eoa	EOA_PTR
 * @param	wk	eoa work *
 * @retval	int TRUE=正常終了。FALSE=異常終了
 */
//--------------------------------------------------------------
static int EoaHide_Init( EOA_PTR eoa, void *wk )
{
	HIDE_WORK *work = wk;
	const HIDE_ADD_H *head = EOA_AddPtrGet( eoa );
	FIELD_OBJ_PTR fldobj = head->fldobj;
	
	work->head = *head;
	work->obj_id = FieldOBJ_OBJIDGet( fldobj );
	work->zone_id = FieldOBJ_ZoneIDGet( fldobj );
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * EOA 隠れ蓑　削除
 * @param	eoa		EOA_PTR
 * @param	wk		eoa work *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaHide_Delete( EOA_PTR eoa, void *wk )
{
	HIDE_WORK *work = wk;
	FIELD_OBJ_PTR fldobj = work->head.fldobj;
	
	if( FieldOBJ_CheckSameID(fldobj,work->obj_id,work->zone_id) == TRUE ){
		FieldOBJ_MoveHideEoaPtrSet( fldobj, NULL );
	}
}

//--------------------------------------------------------------
/**
 * EOA 隠れ蓑　動作
 * @param	eoa		EOA_PTR
 * @param	wk		eoa work *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaHide_Move( EOA_PTR eoa, void *wk )
{
	HIDE_WORK *work = wk;
	
	#if PL_S0792_080711_FIX
	{
		VecFx32 pos;
		EOA_MatrixGet( eoa, &pos );
		pos.y = FieldOBJ_VecPosYGet( work->head.fldobj );
		EOA_MatrixSet( eoa, &pos );
	}
	#endif
}

//--------------------------------------------------------------
/**
 * EOA 隠れ蓑　描画
 * @param	eoa		EOA_PTR
 * @param	wk		eoa work *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaHide_Draw( EOA_PTR eoa, void *wk )
{
	HIDE_WORK *work = wk;
	
	if( work->vanish_sw == TRUE ){
		return;
	}
	
	{
		VecFx32 pos;
		EOA_MatrixGet( eoa, &pos );
		FRO_OBJ_DrawPos( work->head.robj, &pos );
	}
}

//--------------------------------------------------------------
///	隠れ蓑EOA_H
//--------------------------------------------------------------
static const EOA_H_NPP DATA_EoaH_Hide =
{
	HIDE_WORK_SIZE,
	EoaHide_Init,
	EoaHide_Delete,
	EoaHide_Move,
	EoaHide_Draw,
};

//==============================================================================
//	data
//==============================================================================
//--------------------------------------------------------------
///	隠れ蓑imdテーブル　並びはHIDETYPEの値に一致
//--------------------------------------------------------------
static const u32 DATA_3DModelArcID_Hide[HIDE_MAX] =
{
	NARC_fldeff_nin_hide_s_nsbmd,
	NARC_fldeff_nin_hide_b_nsbmd,
	NARC_fldeff_nin_hide_g_nsbmd,
	NARC_fldeff_nin_hide_k_nsbmd,
};

